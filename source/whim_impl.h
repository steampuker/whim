#include "whim_interface.h"

#if !defined(NDEBUG) && !defined(WHIM_ASSERT)
    #include <assert.h>
    #define WHIM_ASSERT(cond, msg) assert(msg && (cond));
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
