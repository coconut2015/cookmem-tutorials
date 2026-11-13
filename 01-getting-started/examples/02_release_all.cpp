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
// Free everything in a memory context at once, instead of one by one.
#include <iostream>

#include <cookmem.h>

int
main ()
{
    cookmem::SimpleMemContext<> memCtx;

    // Many small allocations, for example the nodes of a parse tree that
    // all live as long as one request.
    for (int i = 0; i < 100000; ++i)
    {
        memCtx.allocate (48);
    }
    std::cout << "Footprint: " << memCtx.getFootprint () << " bytes" << std::endl;

    // No need to free each allocation: release all the segments at once.
    // Every pointer from memCtx is invalid after this.
    //
    // releaseAll () frees the memory but calls no destructors, so RAII does
    // not happen.  Use it only for objects that own nothing but memory from
    // memCtx, and destroy objects that hold files, locks or memory from the
    // global heap first.
    memCtx.releaseAll ();

    // Expect output Footprint: 0 bytes
    std::cout << "Footprint: " << memCtx.getFootprint () << " bytes" << std::endl;

    // Destroying a context also releases all of its memory.
    return memCtx.getFootprint () == 0 ? 0 : 1;
}
