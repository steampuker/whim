#include <dlfcn.h>
#include <stdio.h>
#define WHIM_IMPLEMENTATION
#include "../whim.h"


int main(void) {
    if(!whimInit(WHIM_INIT_NONE)) {
        fprintf(stderr, "Could not initialize Whim\n");
        return -1;
    }
    xkbSelectEvents();
    xkbGetMap(2);

    WhimWin* win = whimWinCreate((WhimRect){320, 180, 600, 600},
                                     "Hello Window!",
                                     WHIM_HEX_TO_COLOR(0xff120a26),
                                     WHIM_WINDOW_CENTERED);
    WhimEvent ev;
    whim_bool should_run = 1;
    while(should_run) {
        whimPollEvents(&ev);
        switch(ev.type) {
            case WHIM_EVENT_KEY: ;
                int mods = x11.extensions.xkb_state.mods;
                int level = !!(mods & 1);
                whim_u32 keysym = xkbKeycodeToKesysym(ev.as_key.keycode, x11.extensions.xkb_state.group, level);
                char symbuf[5] = {0};
                int len = keysymToUtf8(keysym, symbuf);
                printf("Key event: %d -> %.*s, is pressed: %s\n",
                        ev.as_key.keycode,
                        len,
                        symbuf,
                        ev.as_key.is_pressed ? "true" : "false");
                break;
            case WHIM_EVENT_CLOSE:
                should_run = 0; break;
        }
    }

    whimWinDestroy(win);
    whimDeinit();
}
