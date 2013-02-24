//
//  Memory.h
//  Mist
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTMemory_h
#define MISTMemory_h

#include "mist/Platform.h"

#ifndef MIST_OS_OSX
#include <memory.h>
#endif

#include <cstdlib>

namespace mist {

    // memory allocator for sora core
    struct MemoryAllocFunc {
        typedef void* (*MallocFunc)(size_t);
        typedef void  (*FreeFunc)(void*);
        typedef void* (*ReallocFunc)(void*, size_t);

        MallocFunc  ukn_malloc;
        FreeFunc    ukn_free;
        ReallocFunc ukn_realloc;

        MemoryAllocFunc(MallocFunc ma,
                        FreeFunc fr,
                        ReallocFunc re):
        ukn_malloc(ma),
        ukn_free(fr),
        ukn_realloc(re) {}
    };

    static MemoryAllocFunc g_memory_allocator(::malloc,
                                              ::free,
                                              ::realloc);

    static void set_malloc_func(MemoryAllocFunc::MallocFunc ma) {
        g_memory_allocator.ukn_malloc = ma;
    }

    static void set_free_func(MemoryAllocFunc::FreeFunc fr) {
        g_memory_allocator.ukn_free = fr;
    }

    static void set_realloc_func(MemoryAllocFunc::ReallocFunc re) {
        g_memory_allocator.ukn_realloc = re;
    }

    inline void* mist_malloc(size_t size) {
        return g_memory_allocator.ukn_malloc(size);
    }

    inline void mist_free(void* ptr) {
        return g_memory_allocator.ukn_free(ptr);
    }

    inline void* mist_realloc(void* ptr, size_t size) {
        return g_memory_allocator.ukn_realloc(ptr, size);
    }

#define mist_malloc_t(t, size) \
    (t*)mist::mist_malloc(sizeof(t) * size);

} // namespace mist

#endif
