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
// One context per request.  Each request builds its trees in the context and
// releases them all when it is done.  CachedMemContext keeps the released
// segments, so the next request reuses them instead of asking the system.
#include <iostream>

#include "parser.h"

static double
handleRequest (Context& memCtx, const char* expression, const Variables& vars)
{
    Node* tree = Parser (memCtx, expression).parse ();
    return evaluate (tree, vars);
}

int
main ()
{
    Context memCtx;
    Variables vars;
    vars["x"] = 3;

    const char* requests[] = { "x * x", "max(x, 10) - 1", "-(x + 4) / 2" };
    void* firstSegment = nullptr;
    for (int round = 0; round < 1000; ++round)
    {
        for (const char* request : requests)
        {
            double result = handleRequest (memCtx, request, vars);
            if (round == 0)
            {
                std::cout << request << " = " << result << "\n";
            }
        }

        // The request is done: free everything it allocated at once.
        memCtx.releaseAll ();

        // The memory goes back to the cache, not to the system, so the
        // next request gets the same memory again.
        void* probe = memCtx.allocate (1);
        if (round == 0)
        {
            firstSegment = probe;
        }
        else if (probe != firstSegment)
        {
            std::cout << "Memory was not reused\n";
            return 1;
        }
        memCtx.deallocate (probe);
    }
    std::cout << "1000 rounds of requests reused the same memory\n";
    return 0;
}
