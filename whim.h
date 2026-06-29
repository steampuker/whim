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
    WHIM_EVENT_CLOSE,
    WHIM_EVENT_KEY,
};

typedef struct WhimEventKey {
    whim_u32 type;
    whim_u32 keyval;
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

whim_u32 whimKeyvalToCodepoint(whim_u32 keyval);

#ifdef WHIM_IMPLEMENTATION
// NOTE: WHIM_API will be used in the future, for now it doesn't do anything
#define WHIM_API(func, postfix) func
#define WHIM_UTIL static inline
#define WHIM_NOALIAS restrict

#if !defined(NDEBUG) && !defined(WHIM_ASSERT)
    #include <assert.h>
    #define WHIM_ASSERT(cond, msg) assert(msg && (cond));
#elif !defined(WHIM_ASSERT)
    #define WHIM_ASSERT(cond, msg)
#endif

typedef struct WhimAllocator {
    void*    context;
    void*    (*alloc)(void* ctx, whim_size_t byte_size);
    void*    (*realloc)(void* ctx, void* ptr, whim_size_t old_size, whim_size_t new_size);
    void     (*free)(void* ctx, void* ptr, whim_size_t byte_size);
} WhimAllocator;

typedef struct WhimCore {
    WhimAllocator allocator;
    whim_size_t  (*strlen)(const char* str);
    void*         (*memcpy)(const void * WHIM_NOALIAS src, void * WHIM_NOALIAS dest, whim_size_t bytes);
} WhimCore;

#ifndef WHIM_CUSTOM_CORE
    #include <stdlib.h>
    #include <string.h>
    WHIM_UTIL void* whim__alloc(void* c, whim_size_t size) { (void)c; return malloc(size); }
    WHIM_UTIL void* whim__realloc(void* c, void* ptr, whim_size_t old_size, whim_size_t new_size) {(void)c; (void)old_size; return realloc(ptr, new_size); }
    WHIM_UTIL void whim__free(void* c, void* ptr, whim_size_t b) { (void)c; (void)b; return free(ptr); }
    WHIM_UTIL whim_size_t whim__strlen(const char* c) { return (whim_size_t)strlen(c); }
    WHIM_UTIL void* whim__memcpy(const void * WHIM_NOALIAS src, void * WHIM_NOALIAS dest, whim_size_t bytes) { return memcpy(dest, src, bytes); }

    static const WhimCore whim_core = {
        {0, whim__alloc, whim__realloc, whim__free},
        whim__strlen,
        whim__memcpy
    };
#else
    static WhimCore whim_core;
#endif

typedef whim_u32 whim_rb_index;
typedef whim_rb_index WhimRingbuf[2];
enum {WHIM_RINGBUF_READ = 0, WHIM_RINGBUF_WRITE = 1};

WHIM_UTIL whim_bool whimRingbufFull(WhimRingbuf *cursors, whim_size_t size) { return (((*cursors)[WHIM_RINGBUF_WRITE] + 1) & (size - 1)) == (*cursors)[WHIM_RINGBUF_READ]; }
WHIM_UTIL whim_bool whimRingbufEmpty(WhimRingbuf *cursors, whim_size_t size) { return (*cursors)[WHIM_RINGBUF_WRITE] == (*cursors)[WHIM_RINGBUF_READ]; }

/*
    Moves the write cursor, moves read cursor on overflow, returns index for write.
    Example use: arr[whimRingBufWrite(cursors, sizeof arr)] = a;
*/
WHIM_UTIL whim_rb_index whimRingbufWrite(WhimRingbuf *cursors, whim_size_t size) {
    WHIM_ASSERT(size && !(size & (size - 1)), "Size must be a power of 2");

    whim_rb_index old_cursor = (*cursors)[WHIM_RINGBUF_WRITE];
    whim_rb_index new_cursor = (old_cursor + 1) & (size - 1);

    if(whimRingbufFull(cursors, size))
        (*cursors)[WHIM_RINGBUF_READ] = ((*cursors)[WHIM_RINGBUF_READ] + 1) & (size - 1);

    (*cursors)[WHIM_RINGBUF_WRITE] = new_cursor;
    return old_cursor;
}

/*
    Moves the read cursor if the buffer is not empty, returns index for read. If empty, return -1.
    Example use:
        if(i = whimRingbufRead(cursors, sizeof arr), i != -1) arr[i]...
*/
WHIM_UTIL whim_rb_index whimRingbufRead(WhimRingbuf *cursors, whim_size_t size) {
    WHIM_ASSERT(size && !(size & (size - 1)), "Size must be a power of 2");
    if(whimRingbufEmpty(cursors, size))
        return -1;

    whim_rb_index old_cursor = (*cursors)[WHIM_RINGBUF_READ];
    whim_rb_index new_cursor = (old_cursor + 1) & (size - 1);

    (*cursors)[WHIM_RINGBUF_READ] = new_cursor;
    return old_cursor;
}

WHIM_UTIL whim_u32 whim__keysymToCodepoint(whim_u32 keysym) {
    static const whim_u32 codepoints[] = {0, 0xFF, 0xFEFFFFFF, 0x152, 0x153, 0x178};
    whim_u32 calculated = keysym & codepoints[(keysym <= 255) | (!!(keysym & 0x1000000) << 1)];
    if(calculated)
        return calculated;

    static const whim_u16 lookup[] = {
        0x104, 0x2D8, 0x141, 0, 0x13D, 0x15A, 0, 0, 0x160, 0x15E, 0x164, 0x179, 0, 0x17D, 0x17B, 0, 0x105, 0x2DB, 0x142, 0, 0x13E, 0x15B, 0x2C7, 0, 0x161, 0x15F, 0x165, 0x17A, 0x2DD, 0x17E, 0x17C, 0x154, 0, 0, 0x102, 0, 0x139, 0x106, 0, 0x10C, 0, 0x118, 0, 0x11A, 0, 0, 0x10E, 0x110, 0x143, 0x147, 0, 0, 0x150,
        0, 0, 0x158, 0x16E, 0, 0x170, 0, 0, 0x162, 0, 0x155, 0, 0, 0x103, 0, 0x13A, 0x107, 0, 0x10D, 0, 0x119, 0, 0x11B, 0, 0, 0x10F, 0x111, 0x144, 0x148, 0, 0, 0x151, 0, 0, 0x159, 0x16F, 0, 0x171, 0, 0, 0x163, 0x2D9, 0x126, 0, 0, 0, 0, 0x124, 0, 0, 0x130, 0, 0x11E,
        0x134, 0, 0, 0, 0, 0x127, 0, 0, 0, 0, 0x125, 0, 0, 0x131, 0, 0x11F, 0x135, 0, 0, 0, 0, 0, 0, 0, 0, 0x10A, 0x108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x120, 0, 0, 0x11C, 0, 0, 0, 0, 0x16C, 0x15C, 0, 0,
        0, 0, 0, 0, 0x10B, 0x109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x121, 0, 0, 0x11D, 0, 0, 0, 0, 0x16D, 0x15D, 0x138, 0x156, 0, 0x128, 0x13B, 0, 0, 0, 0x112, 0x122, 0x166, 0, 0, 0, 0, 0, 0, 0x157, 0, 0x129, 0x13C, 0, 0,
        0, 0x113, 0x123, 0x167, 0x14A, 0, 0x14B, 0x100, 0, 0, 0, 0, 0, 0, 0x12E, 0, 0, 0, 0, 0x116, 0, 0, 0x12A, 0, 0x145, 0x14C, 0x136, 0, 0, 0, 0, 0, 0x172, 0, 0, 0, 0x168, 0x16A, 0, 0x101, 0, 0, 0, 0, 0, 0, 0x12F, 0, 0, 0, 0, 0x117, 0,
        0, 0x12B, 0, 0x146, 0x14D, 0x137, 0, 0, 0, 0, 0, 0x173, 0, 0, 0, 0x169, 0x16B, 0x203E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x3002,
        0x300C, 0x300D, 0x3001, 0x30FB, 0x30F2, 0x30A1, 0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30C3, 0x30FC, 0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD, 0x30AF, 0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD, 0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC, 0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE, 0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8,
        0x30E9, 0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3, 0x309B, 0x309C, 0x60C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x61B, 0, 0, 0, 0x61F, 0, 0x621, 0x622, 0x623, 0x624, 0x625, 0x626, 0x627, 0x628, 0x629, 0x62A, 0x62B, 0x62C, 0x62D, 0x62E, 0x62F, 0x630, 0x631, 0x632, 0x633, 0x634, 0x635, 0x636, 0x637,
        0x638, 0x639, 0x63A, 0, 0, 0, 0, 0, 0x640, 0x641, 0x642, 0x643, 0x644, 0x645, 0x646, 0x647, 0x648, 0x649, 0x64A, 0x64B, 0x64C, 0x64D, 0x64E, 0x64F, 0x650, 0x651, 0x652, 0x452, 0x453, 0x451, 0x454, 0x455, 0x456, 0x457, 0x458, 0x459, 0x45A, 0x45B, 0x45C, 0x491, 0x45E, 0x45F, 0x2116, 0x402, 0x403, 0x401, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A,
        0x40B, 0x40C, 0x490, 0x40E, 0x40F, 0x44E, 0x430, 0x431, 0x446, 0x434, 0x435, 0x444, 0x433, 0x445, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F, 0x44F, 0x440, 0x441, 0x442, 0x443, 0x436, 0x432, 0x44C, 0x44B, 0x437, 0x448, 0x44D, 0x449, 0x447, 0x44A, 0x42E, 0x410, 0x411, 0x426, 0x414, 0x415, 0x424, 0x413, 0x425, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E,
        0x41F, 0x42F, 0x420, 0x421, 0x422, 0x423, 0x416, 0x412, 0x42C, 0x42B, 0x417, 0x428, 0x42D, 0x429, 0x427, 0x42A, 0x386, 0x388, 0x389, 0x38A, 0x3AA, 0, 0x38C, 0x38E, 0x3AB, 0, 0x38F, 0, 0, 0x385, 0x2015, 0, 0x3AC, 0x3AD, 0x3AE, 0x3AF, 0x3CA, 0x390, 0x3CC, 0x3CD, 0x3CB, 0x3B0, 0x3CE, 0, 0, 0, 0, 0, 0x391, 0x392, 0x393, 0x394, 0x395,
        0x396, 0x397, 0x398, 0x399, 0x39A, 0x39B, 0x39C, 0x39D, 0x39E, 0x39F, 0x3A0, 0x3A1, 0x3A3, 0, 0x3A4, 0x3A5, 0x3A6, 0x3A7, 0x3A8, 0x3A9, 0, 0, 0, 0, 0, 0, 0, 0x3B1, 0x3B2, 0x3B3, 0x3B4, 0x3B5, 0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BB, 0x3BC, 0x3BD, 0x3BE, 0x3BF, 0x3C0, 0x3C1, 0x3C3, 0x3C2, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x23B7,
        0x250C, 0x2500, 0x2320, 0x2321, 0x2502, 0x23A1, 0x23A3, 0x23A4, 0x23A6, 0x239B, 0x239D, 0x239E, 0x23A0, 0x23A8, 0x23AC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2264, 0x2260, 0x2265, 0x222B, 0x2234, 0x221D, 0x221E, 0, 0, 0x2207, 0, 0, 0x223C, 0x2243, 0, 0, 0, 0x21D4, 0x21D2, 0x2261, 0, 0, 0, 0, 0, 0, 0x221A,
        0, 0, 0, 0x2282, 0x2283, 0x2229, 0x222A, 0x2227, 0x2228, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2202, 0, 0, 0, 0, 0, 0, 0x192, 0, 0, 0, 0, 0x2190, 0x2191, 0x2192, 0x2193, 0x25C6, 0x2592, 0x2409, 0x240C, 0x240D, 0x240A, 0, 0, 0x2424, 0x240B, 0x2518, 0x2510, 0x250C,
        0x2514, 0x253C, 0x23BA, 0x23BB, 0x2500, 0x23BC, 0x23BD, 0x251C, 0x2524, 0x2534, 0x252C, 0x2502, 0x2003, 0x2002, 0x2004, 0x2005, 0x2007, 0x2008, 0x2009, 0x200A, 0x2014, 0x2013, 0, 0x2423, 0, 0x2026, 0x2025, 0x2153, 0x2154, 0x2155, 0x2156, 0x2157, 0x2158, 0x2159, 0x215A, 0x2105, 0, 0, 0x2012, 0x2329, 0x2E, 0x232A, 0, 0, 0, 0, 0x215B, 0x215C, 0x215D, 0x215E, 0, 0, 0x2122,
        0x2613, 0, 0x25C1, 0x25B7, 0x25CB, 0x25AF, 0x2018, 0x2019, 0x201C, 0x201D, 0x211E, 0x2030, 0x2032, 0x2033, 0, 0x271D, 0, 0x25AC, 0x25C0, 0x25B6, 0x25CF, 0x25AE, 0x25E6, 0x25AB, 0x25AD, 0x25B3, 0x25BD, 0x2606, 0x2022, 0x25AA, 0x25B2, 0x25BC, 0x261C, 0x261E, 0x2663, 0x2666, 0x2665, 0, 0x2720, 0x2020, 0x2021, 0x2713, 0x2717, 0x266F, 0x266D, 0x2642, 0x2640, 0x260E, 0x2315, 0x2117, 0x2038, 0x201A, 0x201E,
        0x3C, 0, 0, 0x3E, 0, 0x2228, 0x2227, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xAF, 0, 0x22A4, 0x2229, 0x230A, 0, 0x5F, 0, 0, 0, 0x2218, 0, 0x2395, 0, 0x22A5, 0x25CB, 0, 0, 0, 0x2308, 0, 0, 0x222A, 0,
        0x2283, 0, 0x2282, 0, 0x22A3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x22A2, 0x2017, 0x5D0, 0x5D1, 0x5D2, 0x5D3, 0x5D4, 0x5D5, 0x5D6, 0x5D7, 0x5D8, 0x5D9, 0x5DA, 0x5DB, 0x5DC, 0x5DD, 0x5DE,
        0x5DF, 0x5E0, 0x5E1, 0x5E2, 0x5E3, 0x5E4, 0x5E5, 0x5E6, 0x5E7, 0x5E8, 0x5E9, 0x5EA, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3138, 0x3139, 0x313A, 0x313B, 0x313C, 0x313D, 0x313E, 0x313F, 0x3140, 0x3141, 0x3142, 0x3143, 0x3144, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E, 0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157, 0x3158, 0x3159,
        0x315A, 0x315B, 0x315C, 0x315D, 0x315E, 0x315F, 0x3160, 0x3161, 0x3162, 0x3163, 0x11A8, 0x11A9, 0x11AA, 0x11AB, 0x11AC, 0x11AD, 0x11AE, 0x11AF, 0x11B0, 0x11B1, 0x11B2, 0x11B3, 0x11B4, 0x11B5, 0x11B6, 0x11B7, 0x11B8, 0x11B9, 0x11BA, 0x11BB, 0x11BC, 0x11BD, 0x11BE, 0x11BF, 0x11C0, 0x11C1, 0x11C2, 0x316D, 0x3171, 0x3178, 0x317F, 0x3181, 0x3184, 0x3186, 0x318D, 0x318E, 0x11EB, 0x11F0, 0x11F9, 0, 0, 0, 0, 0x20A9
    };

    static const whim_i16 pairs[16][2] = { 0, 0, ~0xa0, 0x5f, ~0x41, 0xbd, 0x1b, 0x11a, 0x9c, 0x17c, 0xd0, 0x1c3, 0x122, 0x222, 0x181, 0x27b, 0x1da, 0x2d9, 0x1f9, 0x2f2, 0x251, 0x350, 0x2ad, 0x3aa, 0x2cb, 0x3c6, 0, 0, 0x325, 0x425 };
    whim_u32 index = (keysym & ~0xFF) >> 8;

    calculated = keysym & 0xFF;
    switch(index) {
        case 0xD: return keysym + 96;
        case 0x13: keysym -= 0x13b9; return keysym <= 6 ? codepoints[keysym] : 0;
        case 0x20: return keysym;
        case 0xFF:
            if(calculated >= 0xAA && calculated <= 0xB9 || calculated == 0xBD)
                 return keysym - 0xFF80;
            switch(calculated) {
                case 8: case 9: case 10: case 11: case 13: case 0x1b: return calculated;
                case 0x80: return 0x20;
                case 0x89: return 9;
                case 0x8d: return 13;
                case 0xFF: return 0x7F;
            }
    }

    index &= 0xF;
    calculated += pairs[index][0];
    return (calculated < pairs[index][1]) ? lookup[calculated] : 0;
}

// X11 Backend
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
