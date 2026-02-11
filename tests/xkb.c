#include <dlfcn.h>
#define WHIM_IMPLEMENTATION
#include "../whim.h"

#include <stdio.h>

int main(void) {
    if(!whimInit(WHIM_INIT_NONE)) {
        fprintf(stderr, "Could not initialize Whim\n");
        return -1;
    }

    xkbGetMap(2);

    WhimWin* win = whimWinCreate((WhimRect){320, 180, 600, 600},
                                     "Hello Window!",
                                     WHIM_HEX_TO_COLOR(0xff120a26),
                                     WHIM_WINDOW_CENTERED);
    WhimEvent ev;
    whim_bool should_run = 1;
    int group = 0, level = 0;
    while(should_run) {
        whimPollEvents(&ev);
        switch(ev.type) {
            case WHIM_EVENT_KEY: ;
                whim_u32 keysym = xkbKeycodeToKesysym(ev.as_key.keycode, group, level);
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
