/* See the end of file for license information. */

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

#define WHIM_TRUE (whim_bool)1
#define WHIM_FALSE (whim_bool)0

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
    #define WHIM_ASSERT(cond, message) assert(((void)message, cond))
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

#ifdef WHIM_IMPLEMENTATION // X11 Backend
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <poll.h>

// NOTE: WHIM_API will be used in the future, for now it doesn't do anything
#define WHIM_API(func, postfix) func
#define WHIM_UTIL static inline

#ifdef __has_feature
#    if(__has_feature(address_sanitizer) || __has_feature(thread_sanitizer))
#    define WHIM_SANITIZERS_ON
#    endif
#elif !defined(WHIM_SANITIZERS_ON) && (defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__))
#define WHIM_SANITIZERS_ON
#endif

#if __STDC_VERSION__ >= 201112L
#    define WHIM_ANONYMIZE(x) x
#else
#    define WHIM_CONCAT_(x, y) x ## y
#    define WHIM_CONCAT(x, y) WHIM_CONCAT_(x, y)
#    define WHIM_ANONYMIZE(x) x WHIM_CONCAT(WhimAnonymous, __LINE__)
#endif

struct whim_xcb__iovec { void* base; int len; };
struct whim_xcb__req_t { size_t count; void *ext; whim_u8 opcode, isvoid; };

typedef union { whim_u8 as_8[4]; whim_u16 as_16[2]; whim_u32 as_32; } WhimPayload;
#define WHIM_ARRLEN(x) (sizeof(x) / sizeof(*x))

#define WHIM_SEND_REQUEST(payload, payload_size) x11.sendRequest(x11.connection, 2, (struct whim_xcb__iovec[2]){{payload, payload_size}}, (struct whim_xcb__req_t[1]){1, 0, 0, 1});

struct WhimWin {
    whim_u32 window_id;
    WhimRect rect;
};

static struct WhimX11State {
    void *handle;
    void (*disconnect)(void*);
    whim_u32 (*generateID)(void*);
    int (*sendRequest)(void*, int, struct whim_xcb__iovec*, const struct whim_xcb__req_t*);
    void* (*checkEventQueue)(void*);
    void* (*getReply)(void*, unsigned int, void**);
    void (*free)(void*);
    int (*flush)(void*);

    void *connection;

    struct WhimX11Atoms {
        whim_u32 wm_protocols, close, name, utf8_str;
    } atoms;

    whim_u32 screens, root_window, fd;
    whim_u8 xkb_opcode;
} x11;

WHIM_UTIL whim_bool whimPollReceive(WhimPayload *receiver, whim_u32 size)
{
    enum { POLL_TIMEOUT = -1 };
    struct pollfd fd_poll[1] = {x11.fd, POLLIN};

    if(poll(fd_poll, 1, POLL_TIMEOUT) <= 0)
        return 0;

    if(read(x11.fd, receiver, size) <= 0)
        return 0;

    return 1;
}

#define X11_INTERN_ATOM(str) x11String8Req(16, str, sizeof(str) - 1)
#define X11_QUERY_EXT(str) x11String8Req(98, str, sizeof(str) - 1)
WHIM_UTIL void x11String8Req(whim_u8 req, char *str, whim_u32 str_len)
{
    WhimPayload buffer[2] = {{req}}; buffer->as_16[1] = WHIM_ARRLEN(buffer) + (str_len + 3) / 4, buffer[1].as_16[0] = str_len;

    struct whim_xcb__iovec vecs[4] = {{&buffer, sizeof(buffer)}, {0}, {str, str_len}, {&buffer, -str_len & 3}};
    x11.sendRequest(x11.connection, 2, vecs, (struct whim_xcb__req_t[1]){4, 0, 0, 1});
}

WHIM_UTIL whim_u32* x11ScreenOfDisplay(void *con, int screen)
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

WHIM_UTIL void x11RoundtripAtoms(void)
{
    WhimPayload receiver[8];
    whim_u32 *atoms[] = {&x11.atoms.wm_protocols, &x11.atoms.close, &x11.atoms.name, &x11.atoms.utf8_str};

    for(size_t i = 0; i < WHIM_ARRLEN(atoms); i++) {
        if(!whimPollReceive(receiver, sizeof receiver))
            continue;

        *atoms[i] = receiver[2].as_32;
    }
}

WHIM_UTIL void x11RoundtripExtensions(void)
{
    enum {XKB_MAJOR = 1, XKB_MINOR = 0};
    WhimPayload xkb_receiver[8];

    if(whimPollReceive(xkb_receiver, sizeof xkb_receiver)) do { // Initialize xkb
        if(!xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // xkb is not present

        whim_u8 opcode = xkb_receiver[2].as_8[1];

        WhimPayload buffer[2] = {{opcode, 0 /* XkbUseExtension */}}; buffer->as_16[1] = WHIM_ARRLEN(buffer);
        WhimPayload buffer2[7] = {{opcode, 21 /* XkbSetAutorepeat */}}; buffer2->as_16[1] = WHIM_ARRLEN(buffer2);

        buffer[1].as_16[0] = XKB_MAJOR, buffer[1].as_16[1] = XKB_MINOR;
        buffer2[1].as_16[0] = 256, buffer2[2].as_32 = 1, buffer2[3].as_32 = 1;

        WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
        WHIM_SEND_REQUEST(&buffer2, sizeof(buffer2));

        x11.flush(x11.connection);

        if(!whimPollReceive(xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // Can't use xkb

        x11.xkb_opcode = opcode;

        if(!whimPollReceive(xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // Autorepeat unsupported
    } while(0);
}

WHIM_API(whim_bool whimInit, X11)(enum WhimInitFlags flags)
{
    if(!(x11.handle = dlopen("libxcb.so.1", RTLD_LAZY | RTLD_LOCAL)))
        goto LIBRARY_FAIL;

    void* (*x11Connect)(char*, int*) = dlsym(x11.handle, "xcb_connect");
    x11.disconnect = dlsym(x11.handle, "xcb_disconnect");

    int screen;
    if(!(x11.connection = x11Connect(0, &screen)))
        goto CONNECT_FAIL;

    whim_u32* root_window_ptr = x11ScreenOfDisplay(x11.connection, screen);
    if(!root_window_ptr)
        goto X11_FAIL;

    x11.sendRequest = dlsym(x11.handle, "xcb_send_request");
    x11.flush = dlsym(x11.handle, "xcb_flush");

    X11_INTERN_ATOM("WM_PROTOCOLS"), X11_INTERN_ATOM("WM_DELETE_WINDOW"), X11_INTERN_ATOM("_NET_WM_NAME"), X11_INTERN_ATOM("UTF8_STRING");
    X11_QUERY_EXT("XKEYBOARD"); //X11_QUERY_EXT("RANDR");

    x11.flush(x11.connection);

    int (*x11GetFd)(void*) = dlsym(x11.handle, "xcb_get_file_descriptor");
    x11.checkEventQueue = dlsym(x11.handle, "xcb_poll_for_queued_event");
    x11.generateID = dlsym(x11.handle, "xcb_generate_id");
    x11.getReply = dlsym(x11.handle, "xcb_wait_for_reply");

#ifdef WHIM_SANITIZERS_ON
    x11.free = dlsym(0, "free");
#else
    x11.free = dlsym(x11.handle, "free");
#endif

    x11.fd = x11GetFd(x11.connection);
    x11.root_window = *root_window_ptr;

    x11RoundtripAtoms();
    x11RoundtripExtensions();

    return WHIM_TRUE;

X11_FAIL:
    x11.disconnect(x11.connection);
CONNECT_FAIL:
    dlclose(x11.handle);
LIBRARY_FAIL:
    return WHIM_FALSE;
}

WHIM_UTIL void x11CreateWindow(WhimWin *win, whim_u32 parent, whim_u32 clear_color)
{
    enum { WIN_BACKGROUND = 0x00000002, WIN_EVENTS = 0x00000800,
           EVENT_KEY_PRESS = 1, EVENT_KEY_RELEASE = 2};
    WhimPayload buffer[10] = {{1}};
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[1].as_32 = win->window_id;

    buffer[2].as_32 = parent;
    buffer[4].as_16[1] = buffer[4].as_16[0] = 1;
    buffer[7].as_32 = WIN_BACKGROUND | WIN_EVENTS;
    buffer[8].as_32 = clear_color;
    buffer[9].as_32 = EVENT_KEY_PRESS | EVENT_KEY_RELEASE;

    WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
}

WHIM_UTIL void x11MapWindow(WhimWin* win, whim_bool should_map)
{
    WhimPayload buffer[2] = {{should_map ? 8 : 10}}; buffer->as_16[1] = WHIM_ARRLEN(buffer); buffer[1].as_32 = win->window_id;
    WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
}

WHIM_UTIL void x11ReadjustWindow(WhimWin* win)
{
    WhimPayload buffer[7] = {{12}}; // ConfigureWindow
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[1].as_32 = win->window_id;

    buffer[2].as_32 = 1 | 2 | 4 | 8; buffer[3].as_32 = win->rect.x1; buffer[4].as_32 = win->rect.y1; buffer[5].as_32 = win->rect.x2; buffer[6].as_32 = win->rect.y2;

    WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
}

WHIM_UTIL void x11ChangeProperty(WhimWin* win, whim_u32 property, whim_u32 type, whim_u8 format, whim_u32 data_length, const void* data)
{
    whim_u32 data_bytes = data_length * format / 8;
    WhimPayload buffer[6] = {{18}};
    buffer->as_16[1] = WHIM_ARRLEN(buffer) + (data_bytes + 3) / 4;
    buffer[1].as_32 = win->window_id;

    buffer[2].as_32 = property;
    buffer[3].as_32 = type;
    buffer[4].as_8[0] = format;
    buffer[5].as_32 = data_length;

    struct whim_xcb__iovec vecs[4] = {{&buffer, sizeof(buffer)}, {0}, {(void*)data, data_bytes}, {&buffer, -data_bytes & 3}};
    x11.sendRequest(x11.connection, 2, vecs, (struct whim_xcb__req_t[1]){4, 0, 0, 1});
}

WHIM_API(WhimWin* whimWinCreate, X11)(WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags)
{
    WhimWin* win = WHIM_MALLOC(sizeof(*win));
    if(!win) return 0;

    win->window_id = x11.generateID(x11.connection);
    win->rect = rect;

    (void)flags;

    x11CreateWindow(win, x11.root_window, WHIM_COLOR_TO_HEX(clear_color));
    x11MapWindow(win, WHIM_TRUE);
    x11ReadjustWindow(win);
    x11ChangeProperty(win, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11ChangeProperty(win, x11.atoms.wm_protocols, 4, 32, 1, &x11.atoms.close);
    x11.flush(x11.connection);

    return win;
}

WHIM_API(void whimWinSetTitle, X11)(WhimWin *window, const char *title) {
    x11ChangeProperty(window, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11.flush(x11.connection);
}

WHIM_API(void whimWinDestroy, X11)(WhimWin *window)
{
    WhimPayload buffer[2] = {{4}}; buffer->as_16[1] = 2; buffer[1].as_32 = window->window_id;
    WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
    WHIM_FREE(window);
}

WHIM_API(void whimDeinit, X11)(void)
{
    x11.disconnect(x11.connection);
    dlclose(x11.handle);
}

WHIM_UTIL void x11ParseEvent(WhimPayload receiver[], WhimEvent *event)
{
    enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    switch(receiver[0].as_8[0] & (whim_u8)~0x80) {
        case KEY_PRESS:
            event->as_key = (struct WhimKeyEvent){WHIM_EVENT_KEY, receiver[0].as_8[1], 1};
            break;
        case KEY_RELEASE:
            event->as_key = (struct WhimKeyEvent){WHIM_EVENT_KEY, receiver[0].as_8[1], 0};
            break;
        case CLIENT_MESSAGE:
            if(receiver[3].as_32 == x11.atoms.close)
                event->as_close = (struct WhimCloseEvent){WHIM_EVENT_CLOSE, receiver[1].as_32};
            break;
    }
}

WHIM_API(void whimPollEvents, X11)(WhimEvent *event)
{
    WHIM_ASSERT(event, "Invalid event");

    /* NOTICE: WHIM assumes that xcb events have the same layout as the payload, technically this breaks strict aliasing */
    WhimPayload receiver[8];
    WhimPayload *queued_event = (WhimPayload*)x11.checkEventQueue(x11.connection);

    if(queued_event) {
        x11ParseEvent(queued_event, event);
        x11.free(queued_event);
        return;
    }

    int bytes_read = read(x11.fd, &receiver, sizeof(receiver));
    if(bytes_read <= 0) {
        WHIM_ASSERT(bytes_read != 0, "TODO: Handle connection close");
        WHIM_ASSERT(errno == EAGAIN, "TODO: Handle read errors");

        event->type = WHIM_EVENT_NONE;
        return;
    }

    //if(receiver->as_8[0] == 0) printf("Error happened: %d \n", receiver->as_8[1]);

    WHIM_ASSERT(receiver->as_8[0] != 0, "TODO: Create proper error handling");
    WHIM_ASSERT(receiver->as_8[0] != 1, "TODO: How do even handle replies here?");

    x11ParseEvent(receiver, event);
}

// Xkb Specifics

static struct {
        WhimPayload *ptr;
        whim_u16 indices[256 - 8];
} xkb_keymap;

static void xkbGetMap(int map) // 2 = KeySym map
{
/*
    Some useful info:
        sequence = receiver->as_16[1]
        length = receiver[1].as_32
        keycode-range = receiver[2].as_8[2] : receiver[2].as_8[3]
        present = receiver[6].as_16[0]
        keysym: first = receiver[4].as_8[1], total = receiver[4].as_16[1], nSyms = receiver[5].as_8[0]
*/

    WhimPayload buffer[7] = {x11.xkb_opcode, 8}; // GetMap
    buffer->as_16[1] = WHIM_ARRLEN(buffer); // length
    buffer[1].as_16[0] = 256; // deviceSpec
    buffer[1].as_16[1] = map; // full

    int seq = WHIM_SEND_REQUEST(&buffer, sizeof(buffer));
    x11.flush(x11.connection);

    WhimPayload *receiver = x11.getReply(x11.connection, seq, 0);

    int nSyms = receiver[5].as_8[0];
    WhimPayload *cursor = xkb_keymap.ptr = receiver + 10;
    for(int i = 0, index = 0; i < nSyms; ++i) {
        WHIM_ASSERT(((whim_u16)cursor[1].as_8[0] * cursor[1].as_8[1]) == cursor[1].as_16[1], "Size mismatch, wrong indices");
        xkb_keymap.indices[i] = index;
        index += cursor[1].as_16[1] + 2;
        cursor = xkb_keymap.ptr + index;
    }
}

static whim_u32 xkbKeycodeToKesysym(whim_u32 keycode, whim_u8 group, whim_u32 level)
{
    whim_u32 offset = xkb_keymap.indices[keycode - 8];
    whim_u8 group_info = xkb_keymap.ptr[offset + 1].as_8[0];
    whim_u8 width = xkb_keymap.ptr[offset + 1].as_8[1];

    whim_u8 num_groups = group_info & 0x0f;
    if(group >= num_groups)
        switch(group_info & 0xc0) {
        default: // Wrap
                group %= num_groups; break;
        case 64: // Clamp
                group = num_groups - 1; break;
        case 128: // Redirect
                group = (group_info & 48) / 16; break;
        }

    if(level >= width)
        level = 0;

    whim_u32 entry = group * width + level;
    WHIM_ASSERT(xkb_keymap.ptr[offset + 1].as_16[1] >= entry, "keycodeToKeysym: Out of bounds");

    return xkb_keymap.ptr[offset + 2 + entry].as_32;
}

whim_u32 encodeUtf8(whim_u32 c, char* out) {
    whim_u32 cur = 0;

    if(c <= 128)
        out[cur++] = c;
    else if(c < 2048)
        out[cur++] = 0xC0 | (c >> 6), out[cur++] = 0x80 | (c & 0x3F);
    else if(c < 65536)
        out[cur++] = 0xE0 | (c >> 12), out[cur++] = 0x80 | ((c >> 6) & 0x3F),
        out[cur++] = 0x80 | (c & 0x3F);
    else
        out[cur++] = 0xF0 | (c >> 18), out[cur++] = 0x80 | ((c >> 12) & 0x3F),
        out[cur++] = 0x80 | ((c >> 6) & 0x3F), out[cur++] = 0x80 | (c & 0x3F);

    return cur;
}

whim_u8 keysymToUtf8(whim_u32 keysym, char out_buffer[static 5]) {
    if(keysym <= 0x00ff) // Full Latin-1 coverages
        return encodeUtf8(keysym, out_buffer);

    // Unknown characters are replaced with replacement ?
    out_buffer[0] = 0xEF, out_buffer[1] = 0xBF, out_buffer[2] = 0xBD;
    return 3;
}

#endif
#endif

/*
MIT No Attribution

Copyright 2026 steampuker

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
