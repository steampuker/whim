![logo](https://repository-images.githubusercontent.com/1104319221/78588b57-8941-45ce-9744-2f56128c0b84)
# whim
whim is a C99 single-header library which stands for Window Handling & Input Management library. Its primary purpose is to generalize window and graphics context creation, whilst also being as lightweight as possible.

## Getting started.

Download whim.h, include in your project and define `WHIM_IMPLEMENTATION` in one of translation units.

Example app:
```c
#include <stdio.h>

#define WHIM_IMPLEMENTATION
#include "whim.h"

int main(void) {
    if(!whimInit(WHIM_INIT_NONE)) {
        fprintf(stderr, "Could not initialize Whim\n");
        return -1;
    }

    WhimWin* win = whimWinCreate((WhimRect){320, 180, 600, 600},
                                     "Hello Window!",
                                     WHIM_HEX_TO_COLOR(0xff120a26),
                                     WHIM_WINDOW_CENTERED);

    WhimEvent ev;
    whim_bool should_run = 1;
    while(should_run) {
        whimPollEvents(&ev);
        switch(ev.type) {
            case WHIM_EVENT_KEY:
                printf("Key event: %d, is pressed: %s\n", ev.as_key.keycode, ev.as_key.is_pressed ? "true" : "false");
                break;
            case WHIM_EVENT_CLOSE:
                should_run = 0; break;
        }
    }

    whimWinDestroy(win);
    whimDeinit();
}
```

## Feedback

whim is in an early stage of development, as of now it supports X11 through libxcb and it doesn't support any graphics context out of the box. 

Vulkan seems to work though.

C++ is not supported at the moment.

PRs are welcome!

## Related

whim is greatly inspired by [RGFW](https://github.com/ColleagueRiley/RGFW), [GLFW](https://github.com/glfw/glfw) and [SDL](https://github.com/libsdl-org/SDL).
