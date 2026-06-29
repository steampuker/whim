#include "whim_impl.h"

#include <unistd.h>
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

    whim_u8 xkb, xkb_event;
    struct WhimXkbState {
        WhimUnit *map_ptr, *syms, *types;
        whim_u16 type_indices[256], syms_indices[248];
    } xkb_keymap;
};

static struct WhimX11State {
    void *lib;

    void* (*connect)(char*, int*);
    void (*disconnect)(void*);
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

WHIM_UTIL whim_bool whimPollReceive(whim_u32 file_desc, WhimUnit *receiver, whim_u32 size)
{
    enum { POLL_TIMEOUT = 5000 }; struct pollfd fd_poll[1] = {file_desc, POLLIN};
    return poll(fd_poll, 1, POLL_TIMEOUT) > 0 && read(file_desc, receiver, size) > 0;
}

#define X11_INTERN_ATOM(str) x11String8Req(x11.hook.connection, 16, str, sizeof(str) - 1)
#define X11_QUERY_EXT(str) x11String8Req(x11.hook.connection, 98, str, sizeof(str) - 1)
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
    for(size_t i = 0; i < WHIM_ARRLEN(x11.atoms.elements); i++)
        whimPollReceive(file_desc, receiver, sizeof receiver) ? (x11.atoms.elements[i] = receiver[2].as_32) : 0;
}

WHIM_UTIL void x11RoundtripExtensions(struct WhimXcbHook *hook)
{
    enum {XKB_MAJOR = 1, XKB_MINOR = 0};
    WhimUnit xkb_receiver[8];

    if(whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver)) do {
        if(!xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;

        whim_u8 opcode = xkb_receiver[2].as_8[1];
        whim_u8 first_event = xkb_receiver[2].as_8[2];

        WhimTypedUnit(whim_u16) buffer[2] = {0, WHIM_ARRLEN(buffer), XKB_MAJOR, XKB_MINOR};
        buffer->as_8[0] = opcode, buffer->as_8[1] = 0;
        whimXcbSendRequest(hook->connection, buffer, sizeof buffer);

        WhimTypedUnit(whim_u32) buffer2[7] = {0, 0, 1, 1};
        buffer2->as_8[0] = opcode, buffer2->as_8[1] = 21, buffer2->as_16[1] = WHIM_ARRLEN(buffer2), buffer2[1].as_16[0] = 256;
        whimXcbSendRequest(hook->connection, buffer2, sizeof buffer2);

        x11.flush(hook->connection);

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) || !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;

        hook->xkb = opcode;
        hook->xkb_event = first_event;

        if(!whimPollReceive(hook->file_desc, xkb_receiver, sizeof xkb_receiver) || !xkb_receiver->as_8[0] || !xkb_receiver[2].as_8[0])
            break;
    } while(0);
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

WHIM_UTIL void xkbSelectEvents(void)
{
    WhimUnit buffer[5] = {x11.hook.xkb, 1}; // SelectEvents
    buffer->as_16[1] = WHIM_ARRLEN(buffer); // length
    buffer[1].as_16[0] = 256; // deviceSpec

    buffer[1].as_16[1] = 2; // MapNotify
    buffer[3].as_16[0] = buffer[3].as_16[1] = 2; // KeyType + KeySym

    whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
}

WHIM_UTIL void xkbInitMap(void)
{
    WhimTypedUnit(whim_u16) buffer[7] = {0, WHIM_ARRLEN(buffer), 256, 1 | 2};
    buffer->as_8[0] = x11.hook.xkb, buffer->as_8[1] = 8;

    int seq = whimXcbSendRequest(x11.hook.connection, buffer, sizeof buffer);
    x11.flush(x11.hook.connection);

    x11Free(x11.hook.xkb_keymap.map_ptr);

    WhimUnit *receiver = x11.getReply(x11.hook.connection, seq, 0);
    x11.hook.xkb_keymap.map_ptr = receiver;

    whim_u32 types_len = receiver[3].as_8[3];

    WhimUnit *cursor = x11.hook.xkb_keymap.types = receiver + 10;
    for(whim_u32 i = 0, index = 0; i < types_len; ++i) {
        x11.hook.xkb_keymap.type_indices[i] = index;
        cursor = x11.hook.xkb_keymap.types + (index += 2 + cursor[1].as_8[1] * (2 + cursor[1].as_8[2]));
    }

    x11.hook.xkb_keymap.syms = cursor;

    whim_u32 syms_len = receiver[5].as_8[0];
    for(whim_u32 i = 0, index = 0; i < syms_len; ++i) {
        WHIM_ASSERT(((whim_u16)cursor[1].as_8[0] * cursor[1].as_8[1]) == cursor[1].as_16[1], "Size mismatch, wrong indices");
        x11.hook.xkb_keymap.syms_indices[i] = index;
        cursor = x11.hook.xkb_keymap.syms + (index += cursor[1].as_16[1] + 2);
    }
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
    WhimUnit *cursor = x11.hook.xkb_keymap.types + x11.hook.xkb_keymap.type_indices[kt_index];

    whim_u8 current_mods = modifiers & cursor->as_8[0];

    for(int i = 0; i < cursor[1].as_8[1]; ++i)
        if(cursor[2 + i].as_8[0] && (cursor[2 + i].as_8[2] == current_mods)) {
            level = cursor[2 + i].as_8[1] % width;
            break;
        }

    return level;
}

WHIM_UTIL whim_u32 xkbKeycodeToKeysym(whim_u32 keycode, whim_u16 state)
{
    whim_u8 group = state >> 0xD;
    whim_u8 modifiers = state & 0xFF;
    WhimUnit *keysyms = x11.hook.xkb_keymap.syms + x11.hook.xkb_keymap.syms_indices[keycode];

    whim_u8 group_info = keysyms[1].as_8[0];
    whim_u8 width = keysyms[1].as_8[1];

    whim_u8 num_groups = group_info & 0xF;

    switch((group >= num_groups) << (group_info >> 6)) {
        case 0: break;
        case 2:  group = num_groups - 1; break;
        case 4:  group = (group_info & 0x30) / 16; break;
        default: group %= num_groups; break;
    }

    whim_u8 kt_index = keysyms->as_8[group];
    const WhimUnit *current_keysym = &keysyms[group * width + 2];
    whim_u32 level = xkbGetShiftLevel(kt_index, width, modifiers);

    WHIM_ASSERT(keysyms[1].as_16[1] >= group * width + level, "keycodeToKeysym: Out of bounds");

    if(level == 0 && !(modifiers & 1))
        level |= ((modifiers & 0x2) && xkbIsAlphabetic(current_keysym[level].as_32)) | ((modifiers & 0x10) && ((current_keysym->as_32 - 0xFF95) <= 0xA));

    return current_keysym[level].as_32;
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
    X11_INTERN_ATOM("WM_PROTOCOLS"), X11_INTERN_ATOM("WM_DELETE_WINDOW"), X11_INTERN_ATOM("_NET_WM_NAME"), X11_INTERN_ATOM("UTF8_STRING");
    X11_QUERY_EXT("XKEYBOARD"); // "RANDR"

    x11.flush(x11.hook.connection);

    int (*getFileDesc)(void*) = dlsym(x11.lib, "xcb_get_file_descriptor");
    x11.checkEventQueue = dlsym(x11.lib, "xcb_poll_for_event");
    x11.generateID = dlsym(x11.lib, "xcb_generate_id");
    x11.getReply = dlsym(x11.lib, "xcb_wait_for_reply");

#ifndef EXIT_SUCCESS
    x11Free = dlsym(x11.lib, "free");
#endif

    x11.hook.file_desc = getFileDesc(x11.hook.connection);
    x11.root_window = *root_window_ptr;

    x11RoundtripAtoms(x11.hook.file_desc), x11RoundtripExtensions(&x11.hook);

    if(x11.hook.xkb)
        xkbSelectEvents(), xkbInitMap();

    return WHIM_TRUE;
}

WHIM_API(void whimDeinit, X11)(void)
{
    return x11.lib ? (x11.disconnect(x11.hook.connection), dlclose(x11.lib), x11.lib = 0) : (void)0;
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

WHIM_UTIL void whimParseX11Event(WhimUnit receiver[], WhimEvent *event)
{
    whim_u32 event_type = receiver->as_8[0] & (whim_u8)~0x80;
    switch(event_type) { enum {KEY_PRESS = 2, KEY_RELEASE = 3, CLIENT_MESSAGE = 33};
    case KEY_PRESS:
    case KEY_RELEASE:
        event->type = WHIM_EVENT_KEY;
        event->as_key.keycode = receiver->as_8[1] - 8;
        event->as_key.keyval = xkbKeycodeToKeysym(event->as_key.keycode, receiver[7].as_16[0]);
        event->as_key.is_pressed = (event_type == KEY_PRESS);
        return;
    case CLIENT_MESSAGE:
        if(receiver[3].as_32 != x11.atoms.values.close)
            break;

        event->type = WHIM_EVENT_CLOSE;
        event->as_close.window_id = receiver[1].as_32;
        return;
    }

    if(event_type == x11.hook.xkb_event) switch(receiver->as_8[1])
        case 1: { /* Doesn't work for now */
            // xkbInitMap();
        }

    event->type = WHIM_EVENT_NONE;
}

WHIM_API(void whimPollEvents, X11)(WhimEvent *event)
{
    WhimUnit *queued_event = (WhimUnit*)x11.checkEventQueue(x11.hook.connection); // Pray it works as intended
    return queued_event ? (whimParseX11Event(queued_event, event), x11Free(queued_event)) : (event->type = WHIM_EVENT_NONE);
}

WHIM_API(whim_u32 whimKeyvalToCodepoint, X11)(whim_u32 keyval) { return whim__keysymToCodepoint(keyval); }
