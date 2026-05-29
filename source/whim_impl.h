#include "whim_interface.h"

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
