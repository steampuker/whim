// See the end of file for license information
#ifndef WHIM_H
#define WHIM_H
#if !defined(WHIM_NO_STDINT)
    #include <stdint.h>
    #include <stddef.h>
    typedef _Bool whim_bool;

    typedef int8_t   whim_i8;
    typedef uint8_t  whim_u8;
    typedef int16_t  whim_i16;
    typedef uint16_t whim_u16;
    typedef int32_t  whim_i32;
    typedef uint32_t whim_u32;
    typedef int64_t  whim_i64;
    typedef uint64_t whim_u64;

    typedef size_t whim_size_t;
#else
    typedef _Bool whim_bool;
    typedef   signed char  whim_i8;
    typedef unsigned char  whim_u8;
    typedef   signed short whim_i16;
    typedef unsigned short whim_u16;
    typedef   signed int   whim_i32;
    typedef unsigned int   whim_u32;

    typedef unsigned long whim_size_t;

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

#define WHIM_COLOR_TO_HEX(x) (const whim_u32){((x)[2] | ((x)[1] << 8) | ((x)[0] << 16)) & 0xFFFFFFFF}
#define WHIM_HEX_TO_COLOR(x) (&(WhimColor){(x & 0xFF) >> 16, (x & 0xFF) >> 8, (x & 0xFF)})

typedef whim_u32 WhimRect[4];
typedef whim_u32 WhimVec2[2];
typedef whim_u8 WhimColor[3];

enum WhimInitFlags {
    WHIM_INIT_NONE = 0,
};

enum WhimWinFlags {
    WHIM_WINDOW_CENTERED      = 1U << 0,
    WHIM_WINDOW_NOT_RESIZABLE = 1U << 1,
    WHIM_WINDOW_UNDECORATED   = 1U << 2,
    WHIM_WINDOW_TOPMOST       = 1U << 3,
    WHIM_WINDOW_MAXIMIZED     = 1U << 4,
    WHIM_WINDOW_TRANSPARENT   = 1U << 5
};

typedef struct WhimWin WhimWin;

enum {
    WHIM_EVENT_NONE,
    WHIM_EVENT_INTERNAL,
    WHIM_EVENT_KEY,
    WHIM_EVENT_CLOSE
};

typedef struct WhimEventKey {
    whim_u32 type;
    whim_u32 keysym;
    whim_u8 keycode;
    whim_bool is_pressed;
} WhimEventKey;

typedef struct WhimEventClose {
    whim_u32 type;
    whim_u32 window_id;
} WhimEventClose;

// struct WhimMouseMotionEvent {};
// struct WhimMouseButtonEvent {};

typedef union WhimEvent {
    whim_u32 type;
    WhimEventKey as_key;
    WhimEventClose as_close;
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

WhimWin* whimWinCreate(WhimRect *rect, const char *title, WhimColor *color, enum WhimWinFlags flags);
void whimWinDestroy(WhimWin *window);

void whimWinSetTitle(WhimWin *window, const char *title);
void whimWinSetPosition(WhimWin *window, WhimVec2 *position);
void whimWinSetSize(WhimWin *window, WhimVec2 *size);
void whimWinSetSizeLimits(WhimWin *window, WhimVec2 *min_size, WhimVec2 *max_size);
void whimWinSetClearColor(WhimWin *window, WhimColor clear_color);
whim_bool whimWinShouldClose(WhimWin *window);

#ifdef WHIM_IMPLEMENTATION
#if !defined(NDEBUG) && !defined(WHIM_ASSERT)
    #include <assert.h>
    #define WHIM_ASSERT(cond, msg) assert(msg && (cond));
#elif !defined(WHIM_ASSERT)
    #define WHIM_ASSERT(cond, msg)
#endif

typedef struct {
    void*        context;
    void*        (*alloc)(whim_size_t bytes, void* ctx);
    void         (*free)(void* ptr, whim_size_t bytes, void* ctx);
    whim_size_t  (*strlen)(const char* str);
} WhimCore;

#ifndef WHIM_CUSTOM_CORE
    #include <stdlib.h>
    #include <string.h>
    static inline void* whim__alloc(whim_size_t size, void* c) { (void)c; return malloc(size); }
    static inline void whim__free(void* ptr, whim_size_t b, void* c) { (void)c; (void)b; return free(ptr); }
    static inline whim_size_t whim__strlen(const char* c) { return (whim_size_t)strlen(c); }

    static const WhimCore whim_core = {
        0,
        whim__alloc,
        whim__free,
        whim__strlen
    };
#else
    static WhimCore whim_core;
#endif

// NOTE: WHIM_API will be used in the future, for now it doesn't do anything
#define WHIM_API(func, postfix) func
#define WHIM_UTIL static inline
#define WHIM_NOALIAS restrict

typedef whim_u32 whim_rb_index;
typedef whim_rb_index WhimRingbuf[2];
enum {WHIM_RINGBUF_READ = 0, WHIM_RINGBUF_WRITE = 1};

WHIM_UTIL whim_bool whimRingbufFull(WhimRingbuf *cursors, whim_size_t size) { return (((*cursors)[WHIM_RINGBUF_WRITE] + 1) & (size - 1)) == (*cursors)[WHIM_RINGBUF_READ]; }
WHIM_UTIL whim_bool whimRingbufEmpty(WhimRingbuf *cursors, whim_size_t size) { return (*cursors)[WHIM_RINGBUF_WRITE] == (*cursors)[WHIM_RINGBUF_READ]; }

/*
    Moves the write cursor, moves read cursor on overflow, returns index for write.
    Example use: arr[whimRingBufWrite(cursors, sizeof arr)] = a;
*/
WHIM_UTIL whim_u32 whimRingbufWrite(WhimRingbuf *cursors, whim_size_t size) {
    WHIM_ASSERT(size && !(size & (size - 1)), "Size must be a power of 2");

    whim_rb_index old_cursor = (*cursors)[WHIM_RINGBUF_WRITE];
    whim_rb_index new_cursor = (old_cursor + 1) & (size - 1);

    if(whimRingbufFull(cursors, size))
        (*cursors)[WHIM_RINGBUF_READ] = ((*cursors)[WHIM_RINGBUF_READ] + 1) & (size - 1);

    (*cursors)[WHIM_RINGBUF_WRITE] = new_cursor;
    return old_cursor;
}

/*
    Moves the read cursor if the buffer is not empty, returns index for read.
    Example use:
        if(i = whimRingbufRead(cursors, sizeof arr), i != -1) arr[i]...
*/
WHIM_UTIL whim_u32 whimRingbufRead(WhimRingbuf *cursors, whim_size_t size) {
    WHIM_ASSERT(size && !(size & (size - 1)), "Size must be a power of 2");
    if(whimRingbufEmpty(cursors, size))
        return -1;

    whim_rb_index old_cursor = (*cursors)[WHIM_RINGBUF_READ];
    whim_rb_index new_cursor = (old_cursor + 1) & (size - 1);

    (*cursors)[WHIM_RINGBUF_READ] = new_cursor;
    return old_cursor;
}

// X11 Backend
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <poll.h>
#include <sys/uio.h>

#if defined(EXIT_SUCCESS) || defined(WHIM_SANITIZE)
    #include <stdlib.h>
    #define x11Free free
#else
    static void (*x11Free)(void*);
#endif

typedef union { whim_u8 as_8[4]; whim_u16 as_16[2]; whim_u32 as_32; } WhimUnit;
#define WhimTypedUnit(init_type) union { init_type init[sizeof(whim_u32[1]) / sizeof(init_type)]; whim_u8 as_8[4]; whim_u16 as_16[2]; whim_u32 as_32; WhimUnit as_unit; }

#define WHIM_ARRLEN(x) (sizeof(x) / sizeof *(x))

struct WhimWin {
    const whim_u32 window_id;
};

struct xcb__req { size_t count; void *ext; whim_u8 opcode, isvoid; };

struct WhimXcbHook {
    void *connection;
    whim_u32 file_desc;

    whim_u8 xkb;
};

static struct WhimX11State {
    void *lib;

    void* (*connect)(char*, int*);
    void (*disconnect)(void*);
    int (*getFileDesc)(void*);
    whim_u32 (*generateID)(void*);
    int (*sendRequest)(void*, int, struct iovec*, struct xcb__req*);
    void* (*checkEventQueue)(void*);
    void* (*getReply)(void*, unsigned int, void**);
    int (*flush)(void*);

    struct WhimXcbHook hook;

    union {
        struct { whim_u32 wm_protocols, close, name, utf8_str; } values;
        whim_u32 elements[4];
    } atoms;

    whim_u32 screens, root_window;
} x11;

WHIM_UTIL int whimXcbSendRequest(void *WHIM_NOALIAS connection, void* WHIM_NOALIAS payload, size_t payload_size)
{
    struct xcb__req req[1] = {1, 0, 0, 1};
    struct iovec iovec[2] = {payload, payload_size};
    int result = x11.sendRequest(connection, 2, iovec, req);
    WHIM_ASSERT(result, "Invalid data handling");
    return result;
}

WHIM_UTIL int whimXcbSendRequestMix(void *WHIM_NOALIAS connection, whim_u32 count, void* WHIM_NOALIAS buffers[], whim_u32 sizes[])
{
    enum { IOVEC_MAX = 4 };
    WHIM_ASSERT(count * 2 <= IOVEC_MAX, "iovec limit exceeded");

    struct xcb__req req[1] = {count * 2, 0, 0, 1};
    struct iovec iovec[IOVEC_MAX];
    for(int i = 0; i < count; ++i) {
        iovec[i * 2].iov_base = buffers[i];
        iovec[i * 2].iov_len = sizes[i];
        iovec[i * 2 + 1].iov_base = buffers[i];
        iovec[i * 2 + 1].iov_len = -sizes[i] & 3;
    }

    int result = x11.sendRequest(connection, 2, iovec, req);
    WHIM_ASSERT(result, "Invalid data handling");
    return result;
}

WHIM_UTIL whim_bool whimPollReceive(whim_u32 file_desc, WhimUnit *receiver, whim_u32 size) {
    enum { POLL_TIMEOUT = 5000 };
    struct pollfd fd_poll[1] = {file_desc, POLLIN};

    return poll(fd_poll, 1, POLL_TIMEOUT) > 0 && read(file_desc, receiver, size) > 0;
}

#define X11_INTERN_ATOM(hook, str) x11String8Req(hook, 16, str, sizeof(str) - 1)
#define X11_QUERY_EXT(hook, str) x11String8Req(hook, 98, str, sizeof(str) - 1)
WHIM_UTIL void x11String8Req(void *WHIM_NOALIAS connection, whim_u8 req, const char* WHIM_NOALIAS str, whim_u32 str_len)
{
    WhimTypedUnit(whim_u16) buffer[2] = { 0, WHIM_ARRLEN(buffer) + (str_len + 3) / 4, str_len };
    buffer->as_8[0] = req;

    void *ptr[] = {buffer, (void*)str};
    whim_u32 lengths[] = {sizeof buffer, str_len};
    whimXcbSendRequestMix(connection, 2, ptr, lengths);
}

WHIM_UTIL whim_u32* x11ScreenOfDisplay(void *connection, int screen)
{
    typedef struct {whim_u32 *data; int rem; int index;} xcb_iterator;
    void* (*getSetup)(void*) = dlsym(x11.lib, "xcb_get_setup");
    void (*screenNext)(xcb_iterator*) = dlsym(x11.lib, "xcb_screen_next");
    xcb_iterator (*rootsIterator)(void*) = dlsym(x11.lib, "xcb_setup_roots_iterator");

    xcb_iterator iter = rootsIterator(getSetup(connection));
    for (; iter.rem; screen -= 1, screenNext(&iter))
        if(screen <= 0)
            return iter.data;

    return 0;
}

WHIM_UTIL void x11RoundtripAtoms(whim_u32 file_desc)
{
    WhimUnit receiver[8];

    for(size_t i = 0; i < WHIM_ARRLEN(x11.atoms.elements); i++) {
        if(!whimPollReceive(file_desc, receiver, sizeof receiver))
            continue;

        x11.atoms.elements[i] = receiver[2].as_32;
    }
}

WHIM_UTIL void x11RoundtripExtensions(struct WhimXcbHook *hook)
{
    enum {XKB_MAJOR = 1, XKB_MINOR = 0};
    WhimUnit xkb_receiver[8];

    if(whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver)) do {
        if(!xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;

        whim_u8 opcode = xkb_receiver[2].as_8[1];

        WhimTypedUnit(whim_u16) buffer[2] = {0, WHIM_ARRLEN(buffer), XKB_MAJOR, XKB_MINOR};
        buffer->as_8[0] = opcode;
        buffer->as_8[1] = 0;
        whimXcbSendRequest(hook->connection, buffer, sizeof buffer);

        WhimTypedUnit(whim_u32) buffer2[7] = {0, 0, 1, 1};
        buffer2->as_8[0] = opcode;
        buffer2->as_8[1] = 21;
        buffer2->as_16[1] = WHIM_ARRLEN(buffer2);
        buffer2[1].as_16[0] = 256;
        whimXcbSendRequest(hook->connection, buffer2, sizeof buffer2);

        x11.flush(hook->connection);

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;

        hook->xkb = opcode;

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) ||
           !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;
    } while(0);
}

WHIM_API(whim_bool whimInit, X11)(enum WhimInitFlags flags)
{
    WHIM_ASSERT(whim_core.alloc && whim_core.free && whim_core.strlen, "Core functions are not defined, initialize whim_core");
    if(!(x11.lib = dlopen("libxcb.so.1", RTLD_LAZY | RTLD_LOCAL)))
        return WHIM_FALSE;

    x11.connect = dlsym(x11.lib, "xcb_connect");
    x11.disconnect = dlsym(x11.lib, "xcb_disconnect");

    int (*hasError)(void*) = dlsym(x11.lib, "xcb_connection_has_error");

    int screen;
    whim_u32* root_window_ptr;

    x11.hook.connection = x11.connect(0, &screen);
    if(hasError(x11.hook.connection) || !(root_window_ptr = x11ScreenOfDisplay(x11.hook.connection, screen)))
        return x11.disconnect(x11.hook.connection), dlclose(x11.lib), WHIM_FALSE;

    x11.sendRequest = dlsym(x11.lib, "xcb_send_request");
    x11.flush = dlsym(x11.lib, "xcb_flush");

    // Ensure this is in the same order as x11.atoms
    X11_INTERN_ATOM(x11.hook.connection, "WM_PROTOCOLS");
    X11_INTERN_ATOM(x11.hook.connection, "WM_DELETE_WINDOW");
    X11_INTERN_ATOM(x11.hook.connection, "_NET_WM_NAME");
    X11_INTERN_ATOM(x11.hook.connection, "UTF8_STRING");

    X11_QUERY_EXT(x11.hook.connection, "XKEYBOARD"); // "RANDR"

    x11.flush(x11.hook.connection);

    x11.getFileDesc = dlsym(x11.lib, "xcb_get_file_descriptor");
    x11.checkEventQueue = dlsym(x11.lib, "xcb_poll_for_queued_event");
    x11.generateID = dlsym(x11.lib, "xcb_generate_id");
    x11.getReply = dlsym(x11.lib, "xcb_wait_for_reply");

#if !defined(EXIT_SUCCESS)
    x11Free = dlsym(x11.lib, "free");
#endif

    x11.hook.file_desc = x11.getFileDesc(x11.hook.connection);
    x11.root_window = *root_window_ptr;

    x11RoundtripAtoms(x11.hook.file_desc);
    x11RoundtripExtensions(&x11.hook);

    return WHIM_TRUE;
}

enum { X11_WIN_BACKGROUND = 2, X11_WIN_EVENTS = 2048, X11_EVENT_KEY_PRESS = 1, X11_EVENT_KEY_RELEASE = 2};
WHIM_UTIL void x11CreateWindow(WhimWin *win, whim_u32 parent)
{
    WhimTypedUnit(whim_u32) buffer[8] = {0, win->window_id, parent};

    buffer->as_8[0] = 1;
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[4].as_16[1] = buffer[4].as_16[0] = 1;

    whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
}

WHIM_UTIL void x11MapWindow(WhimWin *win, whim_bool should_map)
{
    WhimUnit buffer[2] = {should_map ? 8 : 10};
    buffer->as_16[1] = WHIM_ARRLEN(buffer);

    buffer[1].as_32 = win->window_id;
    whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
}

WHIM_UTIL void x11ReadjustWindow(WhimWin *win, WhimRect *rect)
{
    WhimTypedUnit(whim_u32) buffer[7] = {0, win->window_id, 1 | 2 | 4 | 8, rect[0][0], rect[0][1], rect[0][2], rect[0][3]};

    buffer->as_8[0] = 12;
    buffer->as_16[1] = WHIM_ARRLEN(buffer);

    whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
}

WHIM_UTIL void x11ChangeWindowAttr(WhimWin *WHIM_NOALIAS win, whim_u32 mask, whim_u32 count, whim_u32 values[])
{
    WhimTypedUnit(whim_u32) buffer[3] = {0, win->window_id, mask};

    buffer->as_8[0] = 2;
    buffer->as_16[1] = WHIM_ARRLEN(buffer) + count;

    void *payloads[] = {buffer, values};
    whim_u32 lengths[] = {sizeof buffer, count * sizeof *values};
    whimXcbSendRequestMix(x11.hook.connection, WHIM_ARRLEN(payloads), payloads, lengths);
}

WHIM_UTIL void x11ChangeProperty(WhimWin *WHIM_NOALIAS win, whim_u32 property, whim_u32 type, whim_u8 format, whim_u32 data_length, const void *WHIM_NOALIAS data)
{
    WhimTypedUnit(whim_u32) buffer[6] = {0, win->window_id, property, type, 0, data_length};
    whim_u32 data_bytes = data_length * format / 8;

    buffer->as_8[0] = 18;
    buffer->as_16[1] = WHIM_ARRLEN(buffer) + (data_bytes + 3) / 4;
    buffer[4].as_8[0] = format;

    void *payloads[] = {buffer, (void*)data};
    whim_u32 lengths[] = {sizeof buffer, data_bytes};
    whimXcbSendRequestMix(x11.hook.connection, WHIM_ARRLEN(payloads), payloads, lengths);
}

WHIM_API(WhimWin* whimWinCreate, X11)(WhimRect *rect, const char *title, WhimColor *clear_color, enum WhimWinFlags flags)
{
    WhimWin* win = whim_core.alloc(sizeof *win, whim_core.context);
    if(!win) return 0;

    *(whim_u32*)&win->window_id = x11.generateID(x11.hook.connection);

    (void)flags;

    whim_u32 values[] = {WHIM_COLOR_TO_HEX(*clear_color), X11_EVENT_KEY_PRESS | X11_EVENT_KEY_RELEASE};

    x11CreateWindow(win, x11.root_window);
    x11ChangeWindowAttr(win, X11_WIN_BACKGROUND | X11_WIN_EVENTS, 2, values);
    x11MapWindow(win, WHIM_TRUE);
    x11ReadjustWindow(win, rect);
    x11ChangeProperty(win, x11.atoms.values.name, x11.atoms.values.utf8_str, 8, whim_core.strlen(title), title);
    x11ChangeProperty(win, x11.atoms.values.wm_protocols, 4, 32, 1, &x11.atoms.values.close);
    x11.flush(x11.hook.connection);

    return win;
}

WHIM_API(void whimWinSetTitle, X11)(WhimWin *win, const char *title)
{
    x11ChangeProperty(win, x11.atoms.values.name, x11.atoms.values.utf8_str, 8, whim_core.strlen(title), title);
    x11.flush(x11.hook.connection);
}

WHIM_API(void whimWinDestroy, X11)(WhimWin *win)
{
    WhimUnit buffer[2] = {4};
    buffer->as_16[1] = WHIM_ARRLEN(buffer);

    buffer[1].as_32 = win->window_id;
    whimXcbSendRequest(x11.hook.connection, &buffer, sizeof(buffer));

    x11.flush(x11.hook.connection);
    whim_core.free(win, sizeof *win, whim_core.context);
}

WHIM_API(void whimDeinit, X11)(void)
{
    x11.disconnect(x11.hook.connection);
    dlclose(x11.lib);
}

WHIM_UTIL void x11ParseEvent(WhimUnit receiver[], WhimEvent *event)
{
    enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    switch(receiver->as_8[0] & (whim_u8)~0x80) {
    case KEY_PRESS:
        event->type = WHIM_EVENT_KEY;
        event->as_key.keycode = receiver->as_8[1] - 8;
        event->as_key.is_pressed = WHIM_TRUE;
        break;
    case KEY_RELEASE:
        event->type = WHIM_EVENT_KEY;
        event->as_key.keycode = receiver->as_8[1] - 8;
        event->as_key.is_pressed = WHIM_FALSE;
        break;
    case CLIENT_MESSAGE:
        if(receiver[3].as_32 != x11.atoms.values.close)
            break;

        event->type = WHIM_EVENT_CLOSE;
        event->as_close.window_id = receiver[1].as_32;
        break;
    default:
        event->type = WHIM_EVENT_NONE;
    }
}

WHIM_API(void whimPollEvents, X11)(WhimEvent *event)
{
    /* NOTICE: WHIM assumes that xcb events have the same layout as the payload, technically this breaks strict aliasing */
    WhimUnit *queued_event = (WhimUnit*)x11.checkEventQueue(x11.hook.connection);

    if(queued_event) {
        x11ParseEvent(queued_event, event);
        x11Free(queued_event);
        return;
    }

    WhimUnit receiver[8];
    int bytes_read = read(x11.hook.file_desc, &receiver, sizeof(receiver));
    if(bytes_read <= 0) {
        WHIM_ASSERT(bytes_read != 0, "TODO: Handle connection close");
        WHIM_ASSERT(errno == EAGAIN, "TODO: Handle read errors");

        event->type = WHIM_EVENT_NONE;
        return;
    }

    // if(receiver->as_8[0] == 0) printf("Error happened: %d \n", receiver->as_8[1]);

    WHIM_ASSERT(receiver->as_8[0] != 0, "TODO: Create proper error handling");
    WHIM_ASSERT(receiver->as_8[0] != 1, "TODO: How do we even handle replies here?");

    x11ParseEvent(receiver, event);
}

/*
WHIM_UTIL WhimUnit* x11GetAtomName(whim_u32 atom, whim_u16 *length) {
    WhimTypedUnit(whim_u8) buffer[2] = {17};
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[1].as_32 = atom;

    int seq = whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
    x11.flush(x11.hook.connection);

    WhimUnit *reply = x11.getReply(x11.hook.connection, seq, 0);
    if(!reply)
        return 0;

    *length = reply[2].as_16[0];

    return reply;
}
*/
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
