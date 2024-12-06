#include <stdlib.h>
#include <sys/time.h>
#include <lvgl.h>
#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "drivers/sdl/lv_sdl_mouse.h"

long long current_timestamp_hal_windowsLinux() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\r\n", milliseconds);
    return milliseconds;
}
/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void * data)
{
    (void)data;

    long long lastTimestamp = current_timestamp_hal_windowsLinux();
    long long newTimestamp = 0;
    while(1) {
        // we don't use this blackbox
        // SDL_Delay(5);   /*Sleep for 5 millisecond*/
        // lv_tick_inc(5); /*Tell lvgl that 5 milliseconds were elapsed*/
        
        newTimestamp = current_timestamp_hal_windowsLinux();
        if ((newTimestamp - lastTimestamp) > 5) {
          lv_tick_inc(newTimestamp - lastTimestamp);
          lastTimestamp = newTimestamp;
        }
    }

    return 0;
}

static lv_draw_buf_t draw_buf;

void init_lvgl_HAL() {
  // Workaround for sdl2 `-m32` crash
  // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
  #ifndef WIN32
    setenv("DBUS_FATAL_WARNINGS", "0", 1);
  #endif

  // Initialize the display --------------------------------------------------------------------------
  lv_display_t * disp = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);

  // Initialize the mouse as input device ------------------------------------------------------------
  lv_indev_t* lvMouse = lv_sdl_mouse_create();

  // /* Add the keyboard as input device
  //  * did not work */
  // static lv_indev_drv_t indev_drv_keyboard;
  // lv_indev_drv_init( &indev_drv_keyboard );
  // indev_drv_keyboard.type = LV_INDEV_TYPE_KEYPAD;
  // indev_drv_keyboard.read_cb = sdl_keyboard_read;  /*This function will be called periodically (by the library) to get the keyboard events*/
  // lv_indev_t *keyboard_device = lv_indev_drv_register( &indev_drv_keyboard );
  // lv_group_t *group = lv_group_create();
  // lv_indev_set_group(keyboard_device, group);
  // lv_group_add_obj(group, lv_scr_act());
  // lv_group_add_obj(group, tabview);
  // lv_group_add_obj(group, lv_tabview_get_content(tabview));
  // lv_group_add_obj(group, tabs);
  //
  // need to be in a loop
  // printf("last key: %d\n",lv_indev_get_key(keyboard_device));

  // Get the SDL window via an event
  SDL_Event aWindowIdFinder;
  SDL_PollEvent(&aWindowIdFinder);
  SDL_Window *mSimWindow = SDL_GetWindowFromID(aWindowIdFinder.window.windowID);
  
  SDL_SetWindowTitle(mSimWindow, "OMOTE simulator");

  /* Tick init.
   * You have to call 'lv_tick_inc()' in periodically to inform lvgl about how much time were elapsed
   * Create an SDL thread to do this*/
  SDL_CreateThread(tick_thread, "tick", NULL);

}
