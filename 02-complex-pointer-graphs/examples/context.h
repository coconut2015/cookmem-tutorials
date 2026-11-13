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
// The memory context of the examples, and containers whose memory comes from
// it.  Objects are created with memCtx.create<T> () and freed with
// memCtx.destroy (), which CookMem 1.6 provides.  Their constructors take the
// context as the first argument, so that an object can put its own strings
// and lists in the same context: memCtx.create<T> (memCtx, a, b) runs
// T (memCtx, a, b).
#ifndef TUTORIAL_CONTEXT_H
#define TUTORIAL_CONTEXT_H

#include <string>
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

#endif // TUTORIAL_CONTEXT_H
