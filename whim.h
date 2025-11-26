/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/

#ifndef WHIM_H
#define WHIM_H
#if !defined(WHIM_NO_STDINT)
#   include <stdint.h>
    typedef _Bool whim_bool;
    typedef int8_t   whim_i8;
    typedef uint8_t  whim_u8;
    typedef int16_t  whim_i16;
    typedef uint16_t whim_u16;
    typedef int32_t  whim_i32;
    typedef uint32_t whim_u32;
    typedef int64_t  whim_i64;
    typedef uint64_t whim_u64;
#else
    typedef _Bool whim_bool;
    typedef   signed char  whim_i8;
    typedef unsigned char  whim_u8;
    typedef   signed short whim_i16;
    typedef unsigned short whim_u16;
    typedef   signed int   whim_i32;
    typedef unsigned int   whim_u32;

#   if defined(_MSC_VER) && (_MSC_VER < 1600)
        typedef   signed __int64 whim_i64;
        typedef unsigned __int64 whim_u64;
#   else
        typedef   signed long long whim_i64;
        typedef unsigned long long whim_u64;
#   endif
#endif

#ifndef WHIM_MALLOC
    #include <stdlib.h>
    #define WHIM_MALLOC malloc
    #define WHIM_FREE free
#endif

#ifndef WHIM_STRLEN
    #include <string.h>
    #define WHIM_STRLEN strlen
#endif

#ifndef WHIM_ASSERT
    #include <assert.h>
    #define WHIM_ASSERT(x) assert(x)
#endif

enum WhimInitFlags {
    WHIM_INIT_NONE = 0,
};

enum WhimWinFlags {
    WHIM_WINDOW_CENTERED = (1U << 0),
    WHIM_WINDOW_NOT_RESIZABLE = (1U << 1),
    WHIM_WINDOW_UNDECORATED = (1U << 2),
    WHIM_WINDOW_TOPMOST = (1U << 3),
    WHIM_WINDOW_MAXIMIZED = (1U << 4),
    WHIM_WINDOW_TRANSPARENT = (1U << 5)
};

#define WHIM_COLOR_TO_HEX(x) (const whim_u32){((x.b) | (x.g << 8) | (x.r << 16)) & 0xFFFFFFFF}
#define WHIM_HEX_TO_COLOR(x) (WhimColor){(x & 0xFF) >> 16, (x & 0xFF) >> 8, (x & 0xFF)}

typedef struct WhimColor {
    whim_u8 r, g, b;
} WhimColor;

typedef struct WhimVec2 {
    whim_u32 x, y;
} WhimVec2;

typedef struct WhimRect {
    whim_u32 x1, y1, x2, y2;
} WhimRect;

enum {
    WHIM_EVENT_NONE,
    WHIM_EVENT_KEY,
    WHIM_EVENT_CLOSE
};

struct WhimKeyEvent {
    whim_u32 type;
    whim_u8 keycode;
    whim_bool is_pressed;
};

struct WhimCloseEvent {
    whim_u32 type;
    whim_u32 window_id;
};

// struct WhimMouseMotionEvent {};
// struct WhimMouseButtonEvent {};

typedef union WhimEvent {
    whim_u32 type;
    struct WhimKeyEvent as_key;
    struct WhimCloseEvent as_close;
    // struct WhimMouseMotionEvent as_mouse_motion;
    // struct WhimMouseButtonEvent as_mouse_button;
} WhimEvent;

typedef struct WhimWin WhimWin;

whim_bool whimInit(enum WhimInitFlags);
void whimDeinit(void);

void whimPollEvents(WhimEvent *event);

WhimWin* whimWinCreate(WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags);
void whimWinDestroy(WhimWin *window);

void whimWinSetTitle(WhimWin *window, const char *title);
void whimWinSetPosition(WhimWin *window, WhimVec2 position);
void whimWinSetSize(WhimWin *window, WhimVec2 size);
void whimWinSetSizeLimits(WhimWin *window, WhimVec2 min_size, WhimVec2 max_size);
void whimWinSetClearColor(WhimWin *window, WhimColor color);
whim_bool whimWinShouldClose(WhimWin *window);


#ifdef WHIM_IMPLEMENTATION

/* X11 backend */
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#if defined(__has_feature)
#    if(__has_feature(address_sanitizer) || __has_feature(thread_sanitizer))
#    define WHIM_SANITIZERS_ON
#    endif
#elif !defined(WHIM_SANITIZERS_ON) && (defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__))
#define WHIM_SANITIZERS_ON
#endif

struct whim_xcb_req_t { size_t count; struct whim_xcb_ext_t *ext; whim_u8 opcode, isvoid; };
struct whim_xcb_iovec { void* base; int len; };

#define WHIM_8BIT_PAYLOAD(N) union Whim8BitPayload { whim_u8 as_8[N]; whim_u16 as_16[(N / 2) | 1]; whim_u32 as_32[(N / 4) | 1]; }
#define WHIM_16BIT_PAYLOAD(N) union Whim16BitPayload { whim_u8 as_8[N * 2]; whim_u16 as_16[N]; whim_u32 as_32[(N / 2) | 1]; }
#define WHIM_32BIT_PAYLOAD(N) union Whim32BitPayload { whim_u8 as_8[N * 4]; whim_u16 as_16[N * 2]; whim_u32 as_32[N]; }

#define WHIM_SEND_REQUEST(payload, payload_size) x11.sendRequest(x11.connection, 2, (struct whim_xcb_iovec[2]){{payload, payload_size}}, &(struct whim_xcb_req_t){1, 0, 0, 1});

#ifndef WHIM_RING_BUF_SIZE
#define WHIM_RING_BUF_SIZE 8U
#endif

struct WhimWin {
    WhimRect rect;
    whim_u32 window_id;
};

static struct WhimX11State {
    void *handle;
    void* (*connect)(char*, int*);
    void (*disconnect)(void*);
    int (*getFileDescriptor)(void*);
    whim_u32 (*generateID)(void*);
    int (*sendRequest)(void*, int, struct whim_xcb_iovec*, const struct whim_xcb_req_t*);
    void* (*checkEventQueue)(void*);
    void* (*getReply)(void*, unsigned int, void**);
    void (*free)(void*);
    int (*flush)(void*);

    void *connection;

    struct WhimAtoms {
        whim_u32 wm_protocols, close, name, utf8_str;
    } atoms;
    whim_u32 root_window, fd;
} x11;

static inline void x11InternAtom(char *str, whim_u32 str_len)
{
    WHIM_32BIT_PAYLOAD(2) buffer = {{16}}; // InternAtom
    buffer.as_16[1] = 2 + (str_len + 3) / 4,
    buffer.as_16[2] = str_len;

    struct whim_xcb_iovec vecs[4] = {{&buffer, sizeof(buffer.as_8)}, {0}, {str, str_len}, {&buffer, -str_len & 3}};
    x11.sendRequest(x11.connection, 2,
                    vecs,
                    &(struct whim_xcb_req_t){4, 0, 0, 1});
}

static whim_u32 *x11ScreenOfDisplay(void *con, int screen)
{
    struct xcb_iterator {whim_u32 *data; int rem; int index;} iter;
    void* (*const getSetup)(void*) = dlsym(x11.handle, "xcb_get_setup");
    void (*const screenNext)(struct xcb_iterator*) = dlsym(x11.handle, "xcb_screen_next");
    struct xcb_iterator (*rootsIterator)(const void*) = dlsym(x11.handle, "xcb_setup_roots_iterator");

    iter = rootsIterator(getSetup(con));
    for (; iter.rem; screen -= 1, screenNext(&iter))
        if(screen <= 0)
            return iter.data;

    return 0;
}

whim_bool whimInit(enum WhimInitFlags flags)
{
    if(!(x11.handle = dlopen("libxcb.so.1", RTLD_LAZY | RTLD_LOCAL)))
        goto lib_fail;

    x11.connect = dlsym(x11.handle, "xcb_connect");
    x11.disconnect = dlsym(x11.handle, "xcb_disconnect");

    int screen;
    if(!(x11.connection = x11.connect(0, &screen)))
        goto connect_fail;

    whim_u32* root_window_ptr = x11ScreenOfDisplay(x11.connection, screen);
    if(!root_window_ptr)
        goto x11_fail;

    x11.sendRequest = dlsym(x11.handle, "xcb_send_request");
    x11.flush = dlsym(x11.handle, "xcb_flush");

    x11InternAtom("WM_PROTOCOLS", sizeof("WM_PROTOCOLS") - 1);
    x11InternAtom("WM_DELETE_WINDOW", sizeof("WM_DELETE_WINDOW") - 1);
    x11InternAtom("_NET_WM_NAME", sizeof("_NET_WM_NAME") - 1);
    x11InternAtom("UTF8_STRING", sizeof("UTF8_STRING") - 1);
    x11.flush(x11.connection);

    x11.getFileDescriptor = dlsym(x11.handle, "xcb_get_file_descriptor");
    x11.checkEventQueue = dlsym(x11.handle, "xcb_poll_for_queued_event");
    x11.generateID = dlsym(x11.handle, "xcb_generate_id");
    x11.getReply = dlsym(x11.handle, "xcb_wait_for_reply");

#ifdef WHIM_SANITIZERS_ON
    x11.free = dlsym(0, "free");
#else
    x11.free = dlsym(x11.handle, "free");
#endif

    x11.fd = x11.getFileDescriptor(x11.connection);
    x11.root_window = *root_window_ptr;

    WHIM_8BIT_PAYLOAD(32) receiver;
    int current_atom = 0;
    whim_u32 *atoms[] = {&x11.atoms.wm_protocols, &x11.atoms.close, &x11.atoms.name, &x11.atoms.utf8_str};
    while(current_atom < (sizeof(atoms) / sizeof(*atoms))) {
        if(read(x11.fd, &receiver, sizeof(receiver.as_8)) <= 0) continue;
        *atoms[current_atom++] = receiver.as_32[2];
    }
    return 1;

x11_fail:
    x11.disconnect(x11.connection);
connect_fail:
    dlclose(x11.handle);
lib_fail:
    return 0;
}

static void x11CreateWindow(WhimWin *win, whim_u32 parent, whim_u32 clear_color)
{
    enum { WIN_BACKGROUND = 0x00000002, WIN_EVENTS = 0x00000800,
           EVENT_KEY_PRESS = 1, EVENT_KEY_RELEASE = 2, EVENT_STRUCTURE_NOTIF = 131072, EVENT_PROPERTY_CHANGE = 4194304};
    WHIM_32BIT_PAYLOAD(10) buffer = {{1}}; // CreateWindow
    buffer.as_16[1] = 10; // request_length
    buffer.as_32[1] = win->window_id;

    buffer.as_32[2] = parent;
    // buffer.as_16[6 + 0] = win->rect.x;
    // buffer.as_16[6 + 1] = win->rect.y;
    buffer.as_16[6 + 2] = 1; // win->rect.w;
    buffer.as_16[6 + 3] = 1; // win->rect.h;
    // buffer.as_16[6 + 4] = 0; // border_width
    // buffer.as_16[6 + 5] = 0; // class
    // buffer.as_32[6] = 0; // visual_id
    buffer.as_32[7] = WIN_BACKGROUND | WIN_EVENTS; // flags
    buffer.as_32[8] = clear_color;
    buffer.as_32[9] = EVENT_KEY_PRESS | EVENT_KEY_RELEASE;// | EVENT_PROPERTY_CHANGE | EVENT_STRUCTURE_NOTIF;

    WHIM_SEND_REQUEST(&buffer, sizeof(buffer.as_8));
}

static void x11MapWindow(WhimWin* win)
{
    WHIM_32BIT_PAYLOAD(2) buffer = {{8}}; // MapWindow
    buffer.as_16[1] = 2;
    buffer.as_32[1] = win->window_id;

    WHIM_SEND_REQUEST(&buffer, sizeof(buffer.as_8));
}

static inline void x11ReadjustWindow(WhimWin* win)
{
    WHIM_32BIT_PAYLOAD(7) buffer = {{12}}; // ConfigureWindow
    buffer.as_16[1] = 2 + 5;
    buffer.as_32[1] = win->window_id;

    buffer.as_32[2] = 1 | 2 | 4 | 8;
    buffer.as_32[3] = win->rect.x1;
    buffer.as_32[4] = win->rect.y1;
    buffer.as_32[5] = win->rect.x2;
    buffer.as_32[6] = win->rect.y2;

    WHIM_SEND_REQUEST(&buffer, sizeof(buffer.as_8));
}

static inline void x11ChangeProperty(WhimWin* win, whim_u32 property, whim_u32 type, whim_u8 format, whim_u32 data_length, const void* data)
{
    WHIM_32BIT_PAYLOAD(6) buffer = {{18}}; // ChangeProperty
    buffer.as_16[1] = 6 + (data_length * format / 8 + 3) / 4;
    buffer.as_32[1] = win->window_id;

    buffer.as_32[2] = property;
    buffer.as_32[3] = type;
    buffer.as_8[16] = format;
    buffer.as_32[5] = data_length;

    struct whim_xcb_iovec vecs[4] = {{&buffer, sizeof(buffer.as_8)}, {0}, {(void*)data, data_length * format / 8}, {&buffer, -(data_length * format / 8) & 3}};
    struct whim_xcb_req_t request = {4, 0, 18, 1};
    x11.sendRequest(x11.connection, 2, vecs, &request);
}

WhimWin* whimWinCreate(WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags)
{
    WhimWin* win = WHIM_MALLOC(sizeof(*win));
    if(!win) return 0;

    win->window_id = x11.generateID(x11.connection);
    win->rect = rect;

    (void)flags;

    x11CreateWindow(win, x11.root_window, WHIM_COLOR_TO_HEX(clear_color));
    x11MapWindow(win);
    x11ReadjustWindow(win);
    //x11ChangeProperty(win, 39, 31, 8, strlen(title), title);
    x11ChangeProperty(win, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11ChangeProperty(win, x11.atoms.wm_protocols, 4, 32, 1, &x11.atoms.close);
    x11.flush(x11.connection);

    return win;
}

void whimWinSetTitle(WhimWin *window, const char *title) {
    x11ChangeProperty(window, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11.flush(x11.connection);
}

void whimWinDestroy(WhimWin *window)
{
    WHIM_32BIT_PAYLOAD(2) buffer = {{4}}; // DestroyWindow
    buffer.as_16[1] = 2;
    buffer.as_32[1] = window->window_id;
    WHIM_SEND_REQUEST(&buffer, sizeof(buffer.as_8));
    WHIM_FREE(window);
}

void whimDeinit(void)
{
    x11.disconnect(x11.connection);
    dlclose(x11.handle);
}

union WhimPayloadReceiver { whim_u8 *as_8; whim_u16 *as_16; whim_u32 *as_32; };
static void x11ParseEvent(union WhimPayloadReceiver receiver, WhimEvent *event)
{
    enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    switch(receiver.as_8[0] & (whim_u8)~0x80) {
        case KEY_PRESS:
            event->as_key = (struct WhimKeyEvent){ WHIM_EVENT_KEY, receiver.as_8[1], 1};
            break;
        case KEY_RELEASE:
            event->as_key = (struct WhimKeyEvent){ WHIM_EVENT_KEY, receiver.as_8[1], 0};
            break;
        case CLIENT_MESSAGE:
            if(receiver.as_32[3] == x11.atoms.close)
                event->as_close = (struct WhimCloseEvent){ WHIM_EVENT_CLOSE, receiver.as_32[1]};
            break;
    }
}

void whimPollEvents(WhimEvent *event)
{
    WHIM_ASSERT(event);

    /* NOTICE: WHIM assumes that xcb events have the same layout as the payload, technically this breaks strict aliasing */
    void* queued_event = x11.checkEventQueue(x11.connection);
    if(queued_event) {
        x11ParseEvent((union WhimPayloadReceiver){(whim_u8*)queued_event}, event);
        x11.free(queued_event);
        return;
    }

    WHIM_8BIT_PAYLOAD(32) receiver;
    int bytes_read = read(x11.fd, &receiver, sizeof(receiver.as_8));
    if(bytes_read <= 0) {
        if(bytes_read == 0)
            WHIM_ASSERT(((void)"TODO: Handle connection close", 0));
        if(errno != EAGAIN)
            WHIM_ASSERT(((void)"TODO: Handle read errors", 0));

        event->type = WHIM_EVENT_NONE;
        return;
    }

    switch(receiver.as_8[0]) {
        case 0: WHIM_ASSERT(((void)"TODO: Create proper error handling", 0));
        case 1: WHIM_ASSERT(((void)"TODO: How do even handle replies here?", 0));
        default: x11ParseEvent((union WhimPayloadReceiver){receiver.as_8}, event);
    }
}

#endif

#endif
