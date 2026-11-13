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
// You choose the scope of a memory context.  TLSContextProvider gives each
// thread its own context, and TLSContextScope gives a block of code its own
// context.  Either way, allocations never lock.
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <cookmem.h>

typedef cookmem::CachedMemContext<> Context;
struct WorkTag;     // keeps this group of allocators separate from others
typedef cookmem::TLSContextProvider<Context, WorkTag> Provider;

template<class T>
using Alloc = cookmem::Allocator<T, Provider>;

// Containers with a default-constructed Alloc use the active context of
// their thread: the thread's own context, or the innermost scope.
typedef std::vector<int, Alloc<int>> IntVector;
typedef std::map<int, int, std::less<int>, Alloc<std::pair<const int, int>>> IntMap;

static std::mutex s_printMutex;

static void
worker (int id)
{
    {
        // Thread scope: these containers allocate from this thread's own
        // context, without any locking.
        IntVector values;
        for (int i = 0; i < 1000; ++i)
        {
            values.push_back (i * id);
        }

        {
            // Block scope: a context for this block only.  Passing true
            // shares the thread context's cached segments.
            cookmem::TLSContextScope<Context, WorkTag> scope (true);
            IntMap squares;
            for (int i = 0; i < 1000; ++i)
            {
                squares[i] = i * i;
            }
            std::lock_guard<std::mutex> lock (s_printMutex);
            std::cout << "Thread " << id << ": block context "
                      << Provider::getActiveContext ().getFootprint ()
                      << " bytes, thread context "
                      << Provider::getThreadContext ().getFootprint ()
                      << " bytes\n";
        }   // the block's context is destroyed with its map

        std::lock_guard<std::mutex> lock (s_printMutex);
        std::cout << "Thread " << id << ": " << values.size ()
                  << " values in the thread context\n";
    }   // destroy the containers before releasing their memory

    // Give this thread's memory back to the system before it ends.
    Provider::getThreadContext ().releaseAllToSystem ();
}

int
main ()
{
    std::vector<std::thread> threads;
    for (int id = 1; id <= 4; ++id)
    {
        threads.emplace_back (worker, id);
    }
    for (std::thread& t : threads)
    {
        t.join ();
    }
    return 0;
}
