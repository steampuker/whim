<p align="center">
  <img src="https://repository-images.githubusercontent.com/1104319221/78588b57-8941-45ce-9744-2f56128c0b84" alt="whim logo"/>
</p>

# whim
whim is a C99 single-header library which stands for Window Handling & Input Management library. Its primary purpose is to generalize window and graphics context creation, whilst also being as lightweight as possible.

## Getting started.

Download `whim.h`, include in your project and define `WHIM_IMPLEMENTATION` in one of the translation units.

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

    WhimWin* win = whimWinCreate(&(WhimRect){320, 180, 600, 600},
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

whim is in an early stage of development, as of now it supports only X11 through libxcb and it doesn't support any graphics context out of the box.

Vulkan seems to work though, if you can create a context.

C++ is not supported.

PRs are welcome!

<details>
<summary><h2>Development</h2></summary>

> Previously, all development happened directly inside `whim.h`. As of May 2026, this is no longer accepted.

Clone the repository, along with the `source/` directory. The single-header file is composed of different implementation files, you can safely change them.

Once you are done with changes, open a command-line inside the `source/` directory and call:

```
lua merge.lua
```

This will merge implementation files into one in `../whim.h`.

> If you don't have the Lua interpreter, the `bootstrap/` folder contains a minimal C implementation, powered by [minilua](https://github.com/edubart/minilua). Simply compile lua.c as an executable (or even run it with `tcc -run`).

</details>

## Related

whim is greatly inspired by [RGFW](https://github.com/ColleagueRiley/RGFW), [GLFW](https://github.com/glfw/glfw) and [SDL](https://github.com/libsdl-org/SDL).
