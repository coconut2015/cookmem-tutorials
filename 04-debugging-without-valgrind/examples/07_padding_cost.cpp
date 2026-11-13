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
// What padding costs: the same allocation pattern with no padding and with
// 8 bytes of padding, timed, with the footprint each needs.  The numbers
// depend on the machine; this program only reports them.
#include <chrono>
#include <cstdio>
#include <vector>

#include <cookmem.h>

typedef cookmem::SimpleMemContext<> Context;

const int LIVE = 10000;             // allocations alive at any time
const int OPERATIONS = 2000000;     // frees and allocations

struct Result
{
    double milliseconds;
    std::size_t footprint;
};

// Keep LIVE allocations of 16 to 271 bytes, and replace a random one
// OPERATIONS times.
static Result
run (std::uint8_t paddingSize)
{
    Context memCtx (paddingSize);
    std::vector<void*> live (LIVE);
    unsigned int seed = 12345;
    for (int i = 0; i < LIVE; ++i)
    {
        seed = seed * 1103515245u + 12345u;
        live[i] = memCtx.allocate (16 + ((seed >> 16) & 255));
    }

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now ();
    for (int op = 0; op < OPERATIONS; ++op)
    {
        seed = seed * 1103515245u + 12345u;
        int i = static_cast<int> ((seed >> 8) % LIVE);
        memCtx.deallocate (live[i]);
        live[i] = memCtx.allocate (16 + ((seed >> 16) & 255));
    }
    std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now () - start;

    Result result = { elapsed.count (), memCtx.getFootprint () };
    for (void* p : live)
    {
        memCtx.deallocate (p);
    }
    return result;
}

int
main ()
{
    // Best of three runs of each.
    Result plain = run (0);
    Result padded = run (cookmem::MAX_PADDING_SIZE);
    for (int i = 0; i < 2; ++i)
    {
        Result r = run (0);
        if (r.milliseconds < plain.milliseconds) plain = r;
        r = run (cookmem::MAX_PADDING_SIZE);
        if (r.milliseconds < padded.milliseconds) padded = r;
    }

    std::printf ("padding   time         footprint\n");
    std::printf ("none      %7.1f ms   %8zu KB\n", plain.milliseconds, plain.footprint / 1024);
    std::printf ("8 bytes   %7.1f ms   %8zu KB\n", padded.milliseconds, padded.footprint / 1024);
    std::printf ("cost      %+6.1f %%     %+7.1f %%\n",
                 (padded.milliseconds / plain.milliseconds - 1) * 100,
                 (static_cast<double> (padded.footprint) / plain.footprint - 1) * 100);
    return 0;
}
