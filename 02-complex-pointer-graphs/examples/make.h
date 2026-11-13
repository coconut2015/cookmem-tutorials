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
// Helpers to create objects in a memory context, and containers whose
// memory comes from a memory context.
#ifndef TUTORIAL_MAKE_H
#define TUTORIAL_MAKE_H

#include <new>
#include <string>
#include <utility>
#include <vector>

#include <cookmem.h>

// The memory context used by the examples.  CachedMemContext keeps released
// segments for reuse, and is the context type cookmem::Allocator uses by
// default.
typedef cookmem::CachedMemContext<> Context;

// A string and a list whose memory comes from a memory context.  Objects
// that hold only these, plain values and pointers into the same context own
// nothing but memory from that context, so they can be released all at once
// without running their destructors.
typedef std::basic_string<char, std::char_traits<char>, cookmem::Allocator<char>> String;

template<class T>
using List = std::vector<T, cookmem::Allocator<T>>;

// Create an object in a memory context, like new.  The constructor gets the
// context as its first argument, so that the object can put its own strings
// and lists in the same context: make<T> (memCtx, a, b) runs
// T (memCtx, a, b).
template<class T, class... Args>
T*
make (Context& memCtx, Args&&... args)
{
    void* ptr = memCtx.allocateAligned (sizeof (T), alignof (T));
    if (ptr == nullptr)
    {
        throw std::bad_alloc ();
    }
    return new (ptr) T (memCtx, std::forward<Args> (args)...);
}

// Destroy an object created by make (), like delete.  Use it to free one
// object while the context lives on; releasing the context frees everything
// without calling destructors.
template<class T>
void
destroy (Context& memCtx, T* ptr)
{
    ptr->~T ();
    memCtx.deallocate (ptr);
}

#endif // TUTORIAL_MAKE_H
