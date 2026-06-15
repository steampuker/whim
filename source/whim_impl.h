#include "whim_interface.h"

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
