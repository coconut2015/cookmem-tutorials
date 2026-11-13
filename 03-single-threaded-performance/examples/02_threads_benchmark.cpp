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
// The same allocation-heavy work on 1, 2, 4 and 8 threads, first with the
// shared heap (std::allocator, which uses new and delete), then with a
// memory context for each thread (no locking).  The times depend on the
// machine and on the system allocator; this program only reports them.
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <functional>
#include <map>
#include <thread>
#include <vector>

#include <cookmem.h>

typedef cookmem::CachedMemContext<> Context;
struct BenchTag;
typedef cookmem::TLSContextProvider<Context, BenchTag> Provider;

typedef std::pair<const int, int> Entry;
typedef std::map<int, int, std::less<int>, std::allocator<Entry>> HeapMap;
typedef std::map<int, int, std::less<int>, cookmem::Allocator<Entry, Provider>> ContextMap;

const int ROUNDS = 200;
const int ENTRIES = 2000;

// Build and destroy a map many times: about 400,000 allocations and as
// many deallocations per thread.
template<class Map>
static long
work ()
{
    long sum = 0;
    for (int round = 0; round < ROUNDS; ++round)
    {
        Map map;
        for (int i = 0; i < ENTRIES; ++i)
        {
            map[(i * 7919) % ENTRIES] = i;
        }
        sum += static_cast<long> (map.size ());
    }
    return sum;
}

// Run work<Map> on numThreads threads and return the elapsed milliseconds.
template<class Map>
static double
run (int numThreads)
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now ();
    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back ([] ()
        {
            volatile long sink = work<Map> ();
            (void) sink;
        });
    }
    for (std::thread& t : threads)
    {
        t.join ();
    }
    std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now () - start;
    return elapsed.count ();
}

int
main ()
{
    std::printf ("threads   shared heap   context per thread   ratio\n");
    for (int numThreads : { 1, 2, 4, 8 })
    {
        // Take the best of three runs of each, to reduce noise.
        double heap = 1e30;
        double context = 1e30;
        for (int i = 0; i < 3; ++i)
        {
            heap = (std::min) (heap, run<HeapMap> (numThreads));
            context = (std::min) (context, run<ContextMap> (numThreads));
        }
        std::printf ("%7d   %8.1f ms   %15.1f ms   %5.2fx\n",
                     numThreads, heap, context, heap / context);
    }
    return 0;
}
