/* See the end of file for license information. */

#ifndef WHIM_H
#define WHIM_H
#if !defined(WHIM_NO_STDINT)
    #include <stdint.h>
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

    #if defined(_MSC_VER) && (_MSC_VER < 1600)
        typedef   signed __int64 whim_i64;
        typedef unsigned __int64 whim_u64;
    #else
        typedef   signed long long whim_i64;
        typedef unsigned long long whim_u64;
    #endif
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

#define WHIM_COLOR_TO_HEX(x) (const whim_u32){((x.b) | (x.g << 8) | (x.r << 16)) & 0xFFFFFFFF}
#define WHIM_HEX_TO_COLOR(x) (WhimColor){(x & 0xFF) >> 16, (x & 0xFF) >> 8, (x & 0xFF)}

typedef struct { whim_u8 r, g, b; } WhimColor;
typedef struct { whim_u32 x, y; } WhimVec2;
typedef struct { whim_u32 x1, y1, x2, y2; } WhimRect;

enum WhimInitFlags {
    WHIM_INIT_NONE = 0,
};

enum WhimWinFlags {
    WHIM_WINDOW_CENTERED      = (1U << 0),
    WHIM_WINDOW_NOT_RESIZABLE = (1U << 1),
    WHIM_WINDOW_UNDECORATED   = (1U << 2),
    WHIM_WINDOW_TOPMOST       = (1U << 3),
    WHIM_WINDOW_MAXIMIZED     = (1U << 4),
    WHIM_WINDOW_TRANSPARENT   = (1U << 5)
};

typedef struct WhimWin WhimWin;
typedef struct WhimHook WhimHook;

enum {
    WHIM_EVENT_NONE,
    WHIM_EVENT_KEY,
    WHIM_EVENT_CLOSE
};

struct WhimEventNone {
    whim_u32 type;
    whim_bool internal;
};

struct WhimEventKey {
    whim_u32 type;
    whim_u32 keysym;
    whim_u8 keycode;
    whim_bool is_pressed;
};

struct WhimEventClose {
    whim_u32 type;
    whim_u32 window_id;
};

// struct WhimMouseMotionEvent {};
// struct WhimMouseButtonEvent {};

typedef union WhimEvent {
    whim_u32 type;
    struct WhimEventNone none;
    struct WhimEventKey as_key;
    struct WhimEventClose as_close;
    // struct WhimMouseMotionEvent as_mouse_motion;
    // struct WhimMouseButtonEvent as_mouse_button;
} WhimEvent;

enum WhimKeyCode {
    WHIM_KC_1 = 2,
    WHIM_KC_2 = 3,
    WHIM_KC_3 = 4,
    WHIM_KC_4 = 5,
    WHIM_KC_5 = 6,
    WHIM_KC_6 = 7,
    WHIM_KC_7 = 8,
    WHIM_KC_8 = 9,
    WHIM_KC_9 = 10,
    WHIM_KC_0 = 11,

    WHIM_KC_A = 30,
    WHIM_KC_B = 48,
    WHIM_KC_C = 46,
    WHIM_KC_D = 32,
    WHIM_KC_E = 18,
    WHIM_KC_F = 33,
    WHIM_KC_G = 34,
    WHIM_KC_H = 35,
    WHIM_KC_I = 23,
    WHIM_KC_J = 36,
    WHIM_KC_K = 37,
    WHIM_KC_L = 38,
    WHIM_KC_M = 50,
    WHIM_KC_N = 49,
    WHIM_KC_O = 24,
    WHIM_KC_P = 25,
    WHIM_KC_Q = 16,
    WHIM_KC_R = 19,
    WHIM_KC_S = 31,
    WHIM_KC_T = 20,
    WHIM_KC_U = 22,
    WHIM_KC_V = 47,
    WHIM_KC_W = 17,
    WHIM_KC_X = 45,
    WHIM_KC_Y = 21,
    WHIM_KC_Z = 44,

    WHIM_KC_MINUS       = 12,
    WHIM_KC_EQUAL       = 13,
    WHIM_KC_BACKSPACE   = 14,

    WHIM_KC_BRACE_LEFT  = 26,
    WHIM_KC_BRACE_RIGHT = 27,

    WHIM_KC_DOT         = 52,
    WHIM_KC_COMMA       = 51,
    WHIM_KC_SEMICOLON   = 39,
    WHIM_KC_APOSTROPHE  = 40,
    WHIM_KC_GRAVE       = 41,
    WHIN_KC_SLASH       = 53,
    WHIM_KC_BACKSLASH   = 43,

    WHIM_KC_ESC         = 1,
    WHIM_KC_TAB         = 15,
    WHIM_KC_ENTER       = 28,
    WHIM_KC_SPACE       = 57,
    WHIM_KC_SHIFT_RIGHT = 54,
    WHIM_KC_SHIFT_LEFT  = 42,
    WHIM_KC_CTRL_RIGHT  = 97,
    WHIM_KC_CTRL_LEFT   = 29,
    WHIM_KC_ALT_LEFT    = 56,
    WHIM_KC_ALT_RIGHT   = 100,
};

whim_bool whimInit(enum WhimInitFlags);
void whimDeinit(void);

void whimPollEvents(WhimEvent *event);

WhimWin* whimWinCreate(WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags);
WhimWin* whimWinCreateHooked(WhimHook *hook, WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags);
void whimWinDestroy(WhimWin *window);

WhimHook* whimHookSetup(enum WhimInitFlags);
void whimHookTerminate(WhimHook *hook);
void whimHookPollEvents(WhimHook *hook, WhimEvent *event);

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
#define WHIM_NOALIAS restrict

#ifdef __has_feature
#    if(__has_feature(address_sanitizer) || __has_feature(thread_sanitizer))
#    define WHIM_SANITIZERS_ON
#    endif
#elif !defined(WHIM_SANITIZERS_ON) && (defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__))
#define WHIM_SANITIZERS_ON
#endif

struct whim_xcb__iovec { void* base; int len; };
struct whim_xcb__req_t { size_t count; void *ext; whim_u8 opcode, isvoid; };

typedef union { whim_u8 as_8[4]; whim_u16 as_16[2]; whim_u32 as_32; } WhimPayload;
#define WHIM_ARRLEN(x) (sizeof(x) / sizeof(*x))

#define WHIM_SEND_REQUEST(hook, payload, payload_size) x11.sendRequest(hook.connection, 2, (struct whim_xcb__iovec[2]){{payload, payload_size}}, (struct whim_xcb__req_t[1]){1, 0, 0, 1});

struct WhimWin {
    whim_u32 window_id;
    WhimRect rect;

    WhimHook *hook;
};

struct WhimHook {
    void *connection;
    whim_u32 file_desc;
};

struct WhimX11Extensions {
    struct {
        WhimPayload *syms, *types;
        whim_u16 type_indices[256], syms_indices[248]; } xkb_keymap;
    struct { whim_u8 group, mods; } xkb_state;
    whim_u8 xkb, xkb_event;
};

static struct WhimX11State {
    void *lib;

    void* (*connect)(char*, int*);
    void (*disconnect)(void*);
    int (*getFileDesc)(void*);
    whim_u32 (*generateID)(void*);
    int (*sendRequest)(void*, int, struct whim_xcb__iovec*, const struct whim_xcb__req_t*);
    void* (*checkEventQueue)(void*);
    void* (*getReply)(void*, unsigned int, void**);
    void (*free)(void*);
    int (*flush)(void*);

    WhimHook hook;
    struct WhimX11Extensions extensions;

    struct WhimX11Atoms { whim_u32 wm_protocols, close, name, utf8_str; } atoms;

    whim_u32 screens, root_window;
} x11;

WHIM_UTIL whim_bool whimPollReceive(whim_u32 file_desc, WhimPayload *receiver, whim_u32 size)
{
    enum { POLL_TIMEOUT = -1 };
    struct pollfd fd_poll[1] = {file_desc, POLLIN};

    if(poll(fd_poll, 1, POLL_TIMEOUT) <= 0)
        return 0;

    if(read(file_desc, receiver, size) <= 0)
        return 0;

    return 1;
}

#define X11_INTERN_ATOM(con, str) x11String8Req(con, 16, str, sizeof(str) - 1)
#define X11_QUERY_EXT(con, str) x11String8Req(con, 98, str, sizeof(str) - 1)
WHIM_UTIL void x11String8Req(void* WHIM_NOALIAS con, whim_u8 req, char* WHIM_NOALIAS str, whim_u32 str_len)
{
    WhimPayload buffer[2] = {{req}}; buffer->as_16[1] = WHIM_ARRLEN(buffer) + (str_len + 3) / 4, buffer[1].as_16[0] = str_len;

    struct whim_xcb__iovec vecs[4] = {{&buffer, sizeof(buffer)}, {0}, {str, str_len}, {&buffer, -str_len & 3}};
    x11.sendRequest(con, 2, vecs, (struct whim_xcb__req_t[1]){4, 0, 0, 1});
}

WHIM_UTIL whim_u32* x11ScreenOfDisplay(void *con, int screen)
{
    struct xcb_iterator {whim_u32 *data; int rem; int index;} iter;
    void* (*const getSetup)(void*) = dlsym(x11.lib, "xcb_get_setup");
    void (*const screenNext)(struct xcb_iterator*) = dlsym(x11.lib, "xcb_screen_next");
    struct xcb_iterator (*rootsIterator)(const void*) = dlsym(x11.lib, "xcb_setup_roots_iterator");

    iter = rootsIterator(getSetup(con));
    for (; iter.rem; screen -= 1, screenNext(&iter))
        if(screen <= 0)
            return iter.data;

    return 0;
}

WHIM_UTIL void x11RoundtripAtoms(whim_u32 file_desc)
{
    WhimPayload receiver[8];
    whim_u32 *atoms[] = {&x11.atoms.wm_protocols, &x11.atoms.close, &x11.atoms.name, &x11.atoms.utf8_str};

    for(size_t i = 0; i < WHIM_ARRLEN(atoms); i++) {
        if(!whimPollReceive(file_desc, receiver, sizeof receiver))
            continue;

        *atoms[i] = receiver[2].as_32;
    }
}

WHIM_UTIL void x11RoundtripExtensions(void)
{
    enum {XKB_MAJOR = 1, XKB_MINOR = 0};
    WhimPayload xkb_receiver[8];

    if(whimPollReceive(x11.hook.file_desc, xkb_receiver, sizeof xkb_receiver)) do { // Initialize xkb
        if(!xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // xkb is not present

        whim_u8 opcode = xkb_receiver[2].as_8[1];
        whim_u8 first_event = xkb_receiver[2].as_8[2];

        WhimPayload buffer[2] = {{opcode, 0 /* XkbUseExtension */}}; buffer->as_16[1] = WHIM_ARRLEN(buffer);
        WhimPayload buffer2[7] = {{opcode, 21 /* XkbSetAutorepeat */}}; buffer2->as_16[1] = WHIM_ARRLEN(buffer2);

        buffer[1].as_16[0] = XKB_MAJOR, buffer[1].as_16[1] = XKB_MINOR;
        buffer2[1].as_16[0] = 256, buffer2[2].as_32 = 1, buffer2[3].as_32 = 1;

        WHIM_SEND_REQUEST(x11.hook, &buffer, sizeof(buffer));
        WHIM_SEND_REQUEST(x11.hook, &buffer2, sizeof(buffer2));

        x11.flush(x11.hook.connection);

        if(!whimPollReceive(x11.hook.file_desc, xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // Can't use xkb

        x11.extensions.xkb = opcode;
        x11.extensions.xkb_event = first_event;

        if(!whimPollReceive(x11.hook.file_desc, xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver[0].as_8[0] || !xkb_receiver[2].as_8[0])
            break; // Autorepeat unsupported
    } while(0);
}

WHIM_API(whim_bool whimInit, X11)(enum WhimInitFlags flags)
{
    if(!(x11.lib = dlopen("libxcb.so.1", RTLD_LAZY | RTLD_LOCAL)))
        goto LIBRARY_FAIL;

    x11.connect = dlsym(x11.lib, "xcb_connect");
    x11.disconnect = dlsym(x11.lib, "xcb_disconnect");

    int screen;
    if(!(x11.hook.connection = x11.connect(0, &screen)))
        goto CONNECT_FAIL;

    whim_u32* root_window_ptr = x11ScreenOfDisplay(x11.hook.connection, screen);
    if(!root_window_ptr)
        goto X11_FAIL;

    x11.sendRequest = dlsym(x11.lib, "xcb_send_request");
    x11.flush = dlsym(x11.lib, "xcb_flush");

    X11_INTERN_ATOM(x11.hook.connection, "WM_PROTOCOLS");
    X11_INTERN_ATOM(x11.hook.connection, "WM_DELETE_WINDOW");
    X11_INTERN_ATOM(x11.hook.connection, "_NET_WM_NAME");
    X11_INTERN_ATOM(x11.hook.connection, "UTF8_STRING");

    X11_QUERY_EXT(x11.hook.connection, "XKEYBOARD");
    //X11_QUERY_EXT(x11.hook.con, "RANDR");

    x11.flush(x11.hook.connection);

    x11.getFileDesc = dlsym(x11.lib, "xcb_get_file_descriptor");
    x11.checkEventQueue = dlsym(x11.lib, "xcb_poll_for_queued_event");
    x11.generateID = dlsym(x11.lib, "xcb_generate_id");
    x11.getReply = dlsym(x11.lib, "xcb_wait_for_reply");

#ifdef WHIM_SANITIZERS_ON
    x11.free = dlsym(0, "free");
#else
    x11.free = dlsym(x11.lib, "free");
#endif

    x11.hook.file_desc = x11.getFileDesc(x11.hook.connection);
    x11.root_window = *root_window_ptr;

    x11RoundtripAtoms(x11.hook.file_desc);
    x11RoundtripExtensions();

    return WHIM_TRUE;

X11_FAIL:
    x11.disconnect(x11.hook.connection);
CONNECT_FAIL:
    dlclose(x11.lib);
LIBRARY_FAIL:
    return WHIM_FALSE;
}

enum { X11_WIN_BACKGROUND = 2, X11_WIN_EVENTS = 2048,
       X11_EVENT_KEY_PRESS = 1, X11_EVENT_KEY_RELEASE = 2};
WHIM_UTIL void x11CreateWindow(WhimWin *win, whim_u32 parent)
{
    WhimPayload buffer[8] = {{1}};

    buffer->as_16[1] = 8;
    buffer[1].as_32 = win->window_id;

    buffer[2].as_32 = parent;
    buffer[4].as_16[1] = buffer[4].as_16[0] = 1;

    WHIM_SEND_REQUEST(win->hook[0], &buffer, sizeof(buffer));
}

WHIM_UTIL void x11MapWindow(WhimWin* win, whim_bool should_map)
{
    WhimPayload buffer[2] = {{should_map ? 8 : 10}}; buffer->as_16[1] = WHIM_ARRLEN(buffer); buffer[1].as_32 = win->window_id;
    WHIM_SEND_REQUEST(win->hook[0], &buffer, sizeof(buffer));
}

WHIM_UTIL void x11ReadjustWindow(WhimWin* win)
{
    WhimPayload buffer[7] = {{12}}; // ConfigureWindow
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[1].as_32 = win->window_id;

    buffer[2].as_32 = 1 | 2 | 4 | 8; buffer[3].as_32 = win->rect.x1; buffer[4].as_32 = win->rect.y1; buffer[5].as_32 = win->rect.x2; buffer[6].as_32 = win->rect.y2;

    WHIM_SEND_REQUEST(win->hook[0], &buffer, sizeof(buffer));
}

WHIM_UTIL void x11ChangeWindowAttr(WhimWin* win, whim_u32 mask, whim_u32 count, whim_u32 values[])
{
    WhimPayload buffer[3] = {{2}}; // ChangeWindowAttr
    buffer->as_16[1] = WHIM_ARRLEN(buffer) + count;
    buffer[1].as_32 = win->window_id;
    buffer[2].as_32 = mask;

    struct whim_xcb__iovec vecs[4] = {{&buffer, sizeof(buffer)}, {0}, {values, count * sizeof *values}, {0}};
    x11.sendRequest(win->hook[0].connection, 2, vecs, (struct whim_xcb__req_t[1]){4, 0, 0, 1});
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
    x11.sendRequest(win->hook[0].connection, 2, vecs, (struct whim_xcb__req_t[1]){4, 0, 0, 1});
}

WHIM_API(WhimWin* whimWinCreateHooked, X11)(WhimHook *hook, WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags)
{
    WhimWin* win = WHIM_MALLOC(sizeof(*win));
    if(!win) return 0;

    win->hook = hook;
    win->window_id = x11.generateID(win->hook[0].connection);
    win->rect = rect;

    (void)flags;

    whim_u32 values[] = {WHIM_COLOR_TO_HEX(clear_color), X11_EVENT_KEY_PRESS | X11_EVENT_KEY_RELEASE};

    x11CreateWindow(win, x11.root_window);
    x11ChangeWindowAttr(win, X11_WIN_BACKGROUND | X11_WIN_EVENTS, 2, values);
    x11MapWindow(win, WHIM_TRUE);
    x11ReadjustWindow(win);
    x11ChangeProperty(win, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11ChangeProperty(win, x11.atoms.wm_protocols, 4, 32, 1, &x11.atoms.close);
    x11.flush(win->hook[0].connection);

    return win;
}

WHIM_API(WhimWin* whimWinCreate, X11)(WhimRect rect, const char *title, WhimColor clear_color, enum WhimWinFlags flags)
{
    return whimWinCreateHooked(&x11.hook, rect, title, clear_color, flags);
}

WHIM_API(void whimWinSetTitle, X11)(WhimWin *win, const char *title) {
    x11ChangeProperty(win, x11.atoms.name, x11.atoms.utf8_str, 8, WHIM_STRLEN(title), title);
    x11.flush(win->hook[0].connection);
}

WHIM_API(void whimWinDestroy, X11)(WhimWin *win)
{
    WhimPayload buffer[2] = {{4}}; buffer->as_16[1] = 2; buffer[1].as_32 = win->window_id;
    WHIM_SEND_REQUEST(win->hook[0], &buffer, sizeof(buffer));
    x11.flush(win->hook[0].connection);
    WHIM_FREE(win);
}

WHIM_API(void whimDeinit, X11)(void)
{
    x11.disconnect(x11.hook.connection);
    dlclose(x11.lib);
}

WHIM_UTIL void x11ParseEvent(WhimPayload receiver[], WhimEvent *event)
{
    enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    switch(receiver[0].as_8[0] & (whim_u8)~0x80) {
        case KEY_PRESS:
            event->as_key.type = WHIM_EVENT_KEY;
            event->as_key.keycode = receiver[0].as_8[1] - 8;
            event->as_key.is_pressed = 1;
            return;
        case KEY_RELEASE:
            event->as_key.type = WHIM_EVENT_KEY;
            event->as_key.keycode = receiver[0].as_8[1] - 8;
            event->as_key.is_pressed = 0;
            return;
        case CLIENT_MESSAGE:
            if(receiver[3].as_32 != x11.atoms.close)
                return;

            event->as_close.type = WHIM_EVENT_CLOSE;
            event->as_close.window_id = receiver[1].as_32;
            return;
        default:
            event->type = WHIM_EVENT_NONE;
            break;
    }

    if(receiver[0].as_8[0] == x11.extensions.xkb_event) {
        enum { STATE_NOTIFY = 2 };
        switch(receiver[0].as_8[1]) {
            case STATE_NOTIFY:
                x11.extensions.xkb_state.mods = receiver[2].as_8[1];
                x11.extensions.xkb_state.group = receiver[3].as_8[1];
                event->type = WHIM_EVENT_NONE;
                event->none.internal = 1;
                break;
        }
    }
}

WHIM_API(void whimPollEvents, X11)(WhimEvent *event) { whimHookPollEvents(&x11.hook, event); }

WHIM_API(void whimHookPollEvents, X11)(WhimHook *hook, WhimEvent *event)
{
    WHIM_ASSERT(event, "Null pointer passed as event");

    /* NOTICE: WHIM assumes that xcb events have the same layout as the payload, technically this breaks strict aliasing */
    WhimPayload receiver[8];
    WhimPayload *queued_event = (WhimPayload*)x11.checkEventQueue(hook->connection);

    if(queued_event) {
        x11ParseEvent(queued_event, event);
        x11.free(queued_event);
        return;
    }

    int bytes_read = read(hook->file_desc, &receiver, sizeof(receiver));
    if(bytes_read <= 0) {
        WHIM_ASSERT(bytes_read != 0, "TODO: Handle connection close");
        WHIM_ASSERT(errno == EAGAIN, "TODO: Handle read errors");

        event->type = WHIM_EVENT_NONE;
        return;
    }

    //if(receiver->as_8[0] == 0) printf("Error happened: %d \n", receiver->as_8[1]);

    WHIM_ASSERT(receiver->as_8[0] != 0, "TODO: Create proper error handling");
    WHIM_ASSERT(receiver->as_8[0] != 1, "TODO: How do we even handle replies here?");

    x11ParseEvent(receiver, event);
}

WHIM_API(WhimHook* whimHookSetup, X11)(enum WhimInitFlags flags)
{
    WhimHook *hook = WHIM_MALLOC(sizeof *hook);
    if(!hook)
        return 0;

    hook->connection = x11.connect(0, 0);

    if(!hook->connection) {
        WHIM_FREE(hook);
        return 0;
    }

    hook->file_desc = x11.getFileDesc(hook->connection);
    return hook;
}

WHIM_API(void whimHookTerminate, X11)(WhimHook* hook)
{
    x11.disconnect(hook->connection);
    WHIM_FREE(hook);
}

// Xkb Specifics

static void xkbGetMap(int map) // 1 = KeyType map, 2 = KeySym map
{
/*
    Some useful info:
        sequence = receiver->as_16[1]
        length = receiver[1].as_32
        keycode-range = receiver[2].as_8[2] : receiver[2].as_8[3]
        present = receiver[6].as_16[0]
        keysym: first = receiver[4].as_8[1], total = receiver[4].as_16[1], nSyms = receiver[5].as_8[0]
*/
    struct WhimX11Extensions *xkb_ext = &x11.extensions;
    WhimPayload buffer[7] = {xkb_ext->xkb, 8}; // GetMap
    buffer->as_16[1] = WHIM_ARRLEN(buffer); // length
    buffer[1].as_16[0] = 256; // deviceSpec
    buffer[1].as_16[1] = map; // full

    int seq = WHIM_SEND_REQUEST(x11.hook, &buffer, sizeof(buffer));
    x11.flush(x11.hook.connection);

    WhimPayload *receiver = x11.getReply(x11.hook.connection, seq, 0);

    int offset = 0;
    int types_len = receiver[3].as_8[3];

    WhimPayload *cursor = xkb_ext->xkb_keymap.types = receiver + 10;
    if(map & 1) for(int i = 0, index = 0; i < types_len; ++i) {
        xkb_ext->xkb_keymap.type_indices[i] = index;
        index += 2 + cursor[1].as_8[1] * (2 + cursor[1].as_8[2]);
        cursor = xkb_ext->xkb_keymap.types + index;
    }

    xkb_ext->xkb_keymap.syms = cursor;

    int syms_len = receiver[5].as_8[0];
    if(map & 2) for(int i = 0, index = 0; i < syms_len; ++i) {
        WHIM_ASSERT(((whim_u16)cursor[1].as_8[0] * cursor[1].as_8[1]) == cursor[1].as_16[1], "Size mismatch, wrong indices");
        xkb_ext->xkb_keymap.syms_indices[i] = index;
        index += cursor[1].as_16[1] + 2;
        cursor = xkb_ext->xkb_keymap.syms + index;
    }
}

static void xkbSelectEvents(void)
{
    WHIM_ASSERT(x11.extensions.xkb, "Xkb is not supported");

    WhimPayload buffer[5] = {x11.extensions.xkb, 1}; // SelectEvents
    buffer->as_16[1] = WHIM_ARRLEN(buffer); // length
    buffer[1].as_16[0] = 256; // deviceSpec

    buffer[1].as_16[1] = 1 << 2; // affectWhich
    //buffer[2].as_16[0] = 0; // clear
    //buffer[2].as_16[1] = 0; // selectAll
    //buffer[3].as_16[0] = 0; // affectMap
    //buffer[3].as_16[1] = 0; // map

    buffer[4].as_16[0] = 1 | 16;
    buffer[4].as_16[1] = 1 | 16;

    WHIM_SEND_REQUEST(x11.hook, &buffer, sizeof buffer);
    x11.flush(x11.hook.connection);
}

WHIM_UTIL whim_bool xkbIsAlphabetic(whim_u32 k)
{
    static const whim_u64 latin[] = { 0x3FFFFFFULL, 0xC000000000000000ULL, 0x7FBFFFFFULL, 0, 0, 0x800000006F350000ULL, 0x2593CAB4ULL, 0, 0, 0xD210000ULL, 0x30900030ULL, 0, 0, 0x800000004E340002ULL, 0x31074840ULL};
    static const whim_u64 cyrgr[] = { 0x7FFFFFFF80007FFFULL, 0, 0, 0, 0x7FF0000ULL, 0x1FFFFFFULL};
    static const whim_u64 misc1[] = { 0x1ULL, 0x10000000000500ULL, 0x400002000000208ULL, 0, 0x100000000000ULL, 0x2000000100ULL};
    static const whim_u64 misc2[] = { 0x15150010511ULL, 0x10410040ULL, 0, 0xFFFFFFFFFC000ULL };
    static const whim_u64 misc3[] = { 0x4010000010000101ULL, 0x4000010040100000ULL, 0x5555555540000105ULL, 0x55555555555555ULL };

    enum { BIT_BOUND = sizeof(whim_u64) * 8, OFFSET_LATIN = 0x61, OFFSET_CYRGR = 0x6A1, OFFSET_MISC1 = 0x100012D, OFFSET_MISC2 = 0x1000493, OFFSET_MISC3 = 0x1001E03 };
    whim_u32 group = (k - OFFSET_LATIN < WHIM_ARRLEN(latin) * BIT_BOUND) << 0 |
                     (k - OFFSET_CYRGR < WHIM_ARRLEN(cyrgr) * BIT_BOUND) << 1 |
                     (k - OFFSET_MISC1 < WHIM_ARRLEN(misc1) * BIT_BOUND) << 2 |
                     (k - OFFSET_MISC2 < WHIM_ARRLEN(misc2) * BIT_BOUND) << 3 |
                     (k - OFFSET_MISC3 < WHIM_ARRLEN(misc3) * BIT_BOUND) << 4;

    const whim_u64* result;
    switch(group) {
        case 0x1:  k -= OFFSET_LATIN; result = latin; break;
        case 0x2:  k -= OFFSET_CYRGR; result = cyrgr; break;
        case 0x4:  k -= OFFSET_MISC1; result = misc1; break;
        case 0x8:  k -= OFFSET_MISC2; result = misc2; break;
        case 0x10: k -= OFFSET_MISC3; result = misc3; break;
        default: return 0;
    }

    return result[k / BIT_BOUND] >> k % BIT_BOUND & 1;
}

WHIM_UTIL whim_u8 xkbGetShiftLevel(whim_u32 kt_index, whim_u8 width, whim_u8 modifiers)
{
    whim_u32 level = 0;
    WhimPayload *cursor = x11.extensions.xkb_keymap.types + x11.extensions.xkb_keymap.type_indices[kt_index];

    whim_u8 mods_mask = cursor->as_8[0];
    whim_u8 map_entries_len = cursor[1].as_8[1];
    whim_u8 current_mods = modifiers & mods_mask;
    //whim_u8 num_levels = cursor[1].as_8[0];
    //whim_bool preserve = cursor[1].as_8[2]; // What is this for

    for(int i = 0; i < map_entries_len; ++i) {
        if(cursor[2 + i].as_8[0] && (cursor[2 + i].as_8[2] == current_mods)) {
            level = cursor[2 + i].as_8[1] % width;
            break;
        }
    }

    return level;
}

static whim_u32 xkbKeycodeToKesysym(whim_u32 keycode, whim_u8 group, whim_u8 modifiers)
{
    const whim_u16 offset = x11.extensions.xkb_keymap.syms_indices[keycode];
    const WhimPayload *keysyms = x11.extensions.xkb_keymap.syms;

    whim_u8 group_info = keysyms[offset + 1].as_8[0];
    whim_u8 width = keysyms[offset + 1].as_8[1];

    whim_u8 num_groups = group_info & 0xF;

    if(group >= num_groups)
        switch(group_info >> 6) {
            default: group %= num_groups; break; // Wrap
            case 1:  group = num_groups - 1; break; // Clamp
            case 2:  group = (group_info & 0x30) / 16; break; // Redirect
        }

    whim_u8 kt_index = keysyms[offset].as_8[group];
    const WhimPayload *current_keysym = &keysyms[offset + group * width + 2];
    whim_u32 level = xkbGetShiftLevel(kt_index, width, modifiers);

    WHIM_ASSERT(keysyms[offset + 1].as_16[1] >= group * width + level, "keycodeToKeysym: Out of bounds");

    if(level == 0 && !(modifiers & 1)) {
        level |= (modifiers & 2) && xkbIsAlphabetic(current_keysym[level].as_32);
        level |= (modifiers & 16) && ((current_keysym[1].as_32 - 0xFFB0) <= 9);
    }


    return current_keysym[level].as_32;
}

whim_u8 keysymToCodepoint(whim_u32 keysym)
{
    if(keysym <= 0x00ff || keysym & 0x1000000) // Latin-1 + Unicode
        return keysym & 0xFFFFFF;

    if(keysym - 0xFFB0 <= 9) {
        return (keysym & ~0xFFB0) + '0';
    }

    return 0;
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
