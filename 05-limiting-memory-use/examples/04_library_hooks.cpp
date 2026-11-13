/*
 * Copyright (c) 2026 Heng Yuan
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Limiting a third-party library.  Many C libraries let you supply the
// allocation function.  This example uses a stand-in library whose hook has
// the same signature as Lua's lua_Alloc, and gives each plugin its own
// context and limit.  A runaway plugin hits its own limit; the others, and
// the program, are not affected.
#include <cstddef>
#include <cstring>
#include <iostream>

#include <cookmem.h>

// ---- A stand-in for a third-party C library ---------------------------
//
// Like Lua, it calls one function for all its memory:
//   ptr == nullptr, nsize > 0    allocate nsize bytes
//   ptr != nullptr, nsize > 0    resize to nsize bytes
//   nsize == 0                   free ptr
// and it handles a nullptr result as "out of memory".
extern "C"
{
typedef void* (*plugin_alloc_fn) (void* ud, void* ptr, std::size_t osize, std::size_t nsize);

struct plugin
{
    plugin_alloc_fn alloc;
    void*           ud;
    char*           data;
    std::size_t     size;
};

// Append n bytes to the plugin's data; returns 0 when out of memory.
static int
plugin_append (plugin* p, const char* bytes, std::size_t n)
{
    char* grown = static_cast<char*> (p->alloc (p->ud, p->data, p->size, p->size + n));
    if (grown == nullptr)
    {
        return 0;
    }
    std::memcpy (grown + p->size, bytes, n);
    p->data = grown;
    p->size += n;
    return 1;
}

static void
plugin_close (plugin* p)
{
    p->alloc (p->ud, p->data, p->size, 0);
    p->data = nullptr;
    p->size = 0;
}
}
// ---- End of the stand-in library --------------------------------------

typedef cookmem::SimpleMemContext<> Context;

// The adapter: a lua_Alloc-style function that allocates from the context
// passed as ud.  The same function works for Lua itself.
static void*
contextAlloc (void* ud, void* ptr, std::size_t osize, std::size_t nsize)
{
    (void) osize;
    Context* memCtx = static_cast<Context*> (ud);
    if (nsize == 0)
    {
        memCtx->deallocate (ptr);
        return nullptr;
    }
    return memCtx->reallocate (ptr, nsize);     // nullptr when over the limit
}

int
main ()
{
    const char* names[] = { "spell checker", "runaway script", "formatter" };
    const std::size_t limits[] = { 1024 * 1024, 1024 * 1024, 1024 * 1024 };

    Context contexts[3];
    plugin plugins[3];
    for (int i = 0; i < 3; ++i)
    {
        contexts[i].setFootprintLimit (limits[i]);
        plugins[i] = { contextAlloc, &contexts[i], nullptr, 0 };
    }

    // Each plugin does its work; the runaway one never stops growing.
    char chunk[4096];
    std::memset (chunk, 'x', sizeof (chunk));
    for (int i = 0; i < 3; ++i)
    {
        int rounds = (i == 1) ? 1000000 : 20;
        int done = 0;
        while (done < rounds && plugin_append (&plugins[i], chunk, sizeof (chunk)))
        {
            ++done;
        }
        std::cout << names[i] << ": " << plugins[i].size / 1024 << " KB, "
                  << (done < rounds ? "stopped at its limit" : "finished") << "\n";
    }

    for (int i = 0; i < 3; ++i)
    {
        plugin_close (&plugins[i]);
    }
    return 0;
}
