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
#endif
