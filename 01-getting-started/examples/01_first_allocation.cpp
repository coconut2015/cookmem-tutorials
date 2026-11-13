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
// Allocate, resize and free memory from a memory context, like malloc,
// realloc and free.
#include <cstring>
#include <iostream>

#include <cookmem.h>

int
main ()
{
    // A memory context that gets large segments from the system (mmap on
    // Unix, VirtualAlloc on Windows) and hands out small pieces of them.
    cookmem::SimpleMemContext<> memCtx;

    // Allocate, like malloc.
    const char hello[] = "Hello, CookMem";
    char* text = static_cast<char*> (memCtx.allocate (32));
    std::memcpy (text, hello, sizeof (hello));
    std::cout << text << "\n";

    // Grow it, like realloc.  The contents are kept.
    text = static_cast<char*> (memCtx.reallocate (text, 64));
    // Expect output 64 bytes
    std::cout << memCtx.getSize (text) << " bytes\n";

    // Free it, like free.
    memCtx.deallocate (text);
    return 0;
}
