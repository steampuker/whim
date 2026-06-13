#include "whim_impl.h"

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
WHIM_UTIL void x11String8Req(void* WHIM_NOALIAS connection, whim_u8 req, const char* WHIM_NOALIAS str, whim_u32 str_len)
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

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) || !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;

        hook->xkb = opcode;

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) || !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;
    } while(0);
}

WHIM_API(whim_bool whimInit, X11)(enum WhimInitFlags flags)
{
    WHIM_ASSERT(whim_core.allocator.alloc, "Core functions are not defined, initialize whim_core");
    if(!(x11.lib = dlopen("libxcb.so.1", RTLD_LAZY | RTLD_LOCAL)))
        return WHIM_FALSE;

    x11.connect = dlsym(x11.lib, "xcb_connect");
    x11.disconnect = dlsym(x11.lib, "xcb_disconnect");

    int (*hasError)(void*) = dlsym(x11.lib, "xcb_connection_has_error");

    int screen;
    whim_u32* root_window_ptr;

    x11.hook.connection = x11.connect(0, &screen);
    if(hasError(x11.hook.connection) || !(root_window_ptr = x11ScreenOfDisplay(x11.hook.connection, screen)))
        return whimDeinit(), WHIM_FALSE;

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

#ifndef EXIT_SUCCESS
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
    WhimWin* win = whim_core.allocator.alloc(whim_core.allocator.context, sizeof *win);
    if(!win)
        return 0;

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
    if(x11.lib) {
        WhimUnit buffer[2] = {4};
        buffer->as_16[1] = WHIM_ARRLEN(buffer);
        buffer[1].as_32 = win->window_id;
        whimXcbSendRequest(x11.hook.connection, &buffer, sizeof(buffer));
    }

    x11.flush(x11.hook.connection);
    whim_core.allocator.free(whim_core.allocator.context, win, sizeof *win);
}

WHIM_API(void whimDeinit, X11)(void)
{
    if(!x11.lib)
        return;

    x11.disconnect(x11.hook.connection);
    dlclose(x11.lib);
    x11.lib = 0;
}

WHIM_UTIL void whimParseX11Event(WhimUnit receiver[], WhimEvent *event)
{
    switch(receiver->as_8[0] & (whim_u8)~0x80) { enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    case KEY_PRESS:
        event->type = WHIM_EVENT_KEY;
        event->as_key.keycode = receiver->as_8[1] - 8;
        event->as_key.is_pressed = WHIM_TRUE;
        return;
    case KEY_RELEASE:
        event->type = WHIM_EVENT_KEY;
        event->as_key.keycode = receiver->as_8[1] - 8;
        event->as_key.is_pressed = WHIM_FALSE;
        return;
    case CLIENT_MESSAGE:
        if(receiver[3].as_32 != x11.atoms.values.close)
            break;

        event->type = WHIM_EVENT_CLOSE;
        event->as_close.window_id = receiver[1].as_32;
        return;
    }

    event->type = WHIM_EVENT_NONE;
}

WHIM_API(void whimPollEvents, X11)(WhimEvent *event)
{
    WhimUnit *queued_event = (WhimUnit*)x11.checkEventQueue(x11.hook.connection); // Pray it works as intended

    if(queued_event)
        return whimParseX11Event(queued_event, event), x11Free(queued_event);

    WhimUnit receiver[8];
    int bytes_read = read(x11.hook.file_desc, &receiver, sizeof(receiver));

    if(bytes_read == 0)
        return event->type = WHIM_EVENT_CLOSE, event->as_close.window_id = 0, (void)0;
    else if(bytes_read < 0)
        return errno == EAGAIN ? (void)(event->type = WHIM_EVENT_NONE) : WHIM_ASSERT(0, "Event read error");

    // if(receiver->as_8[0] == 0) printf("Error happened: %d \n", receiver->as_8[1]);

    WHIM_ASSERT(receiver->as_8[0] != 0, "TODO: Create proper error handling");
    WHIM_ASSERT(receiver->as_8[0] != 1, "TODO: How do we even handle replies here?");

    whimParseX11Event(receiver, event);
}

/*
WHIM_UTIL char* x11GetAtomName(whim_u32 atom, whim_u32 *length) {
    WhimTypedUnit(whim_u8) buffer[2] = {17};
    buffer->as_16[1] = WHIM_ARRLEN(buffer);
    buffer[1].as_32 = atom;

    int seq = whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
    x11.flush(x11.hook.connection);

    WhimUnit *reply = x11.getReply(x11.hook.connection, seq, 0);
    if(!reply)
        return 0;

    *length = reply[2].as_16[0];

    WhimUnit* ret = whim_core.allocator.alloc(whim_core.allocator.context, *length);
    whim_core.memcpy(reply + 8, ret, *length);

    x11Free(reply);
    return (char*)ret;
}
*/
