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
// Padding after each allocation catches a write past its end when the
// allocation is freed.  The exception says what was found and where.
#include <cstring>
#include <iostream>

#include <cookmem.h>

static const char*
errorName (cookmem::MemError_et error)
{
    switch (error)
    {
        case cookmem::MEM_ERROR_PADDING: return "MEM_ERROR_PADDING";
        case cookmem::MEM_ERROR_DOUBLE_FREE: return "MEM_ERROR_DOUBLE_FREE";
        case cookmem::MEM_ERROR_ASSERT: return "MEM_ERROR_ASSERT";
        default: return "MEM_ERROR_GENERAL";
    }
}

int
main ()
{
    // 8 bytes of padding after each allocation, filled with 0xcd.  Padding
    // also makes the context store the exact size of each allocation.
    cookmem::SimpleMemContext<> memCtx (cookmem::MAX_PADDING_SIZE);

    char* name = static_cast<char*> (memCtx.allocate (5));
    // Expect output 5 bytes
    std::cout << memCtx.getSize (name) << " bytes\n";

    // A classic bug: "hello" needs 6 bytes, with the '\0'.
    std::memcpy (name, "hello", sizeof ("hello"));

    try
    {
        memCtx.deallocate (name);
    }
    catch (const cookmem::Exception& ex)
    {
        // Expect output MEM_ERROR_PADDING: padding byte got modified.
        std::cout << errorName (ex.getError ()) << ": " << ex.getMessage () << "\n";
        std::cout << "  detected at " << ex.getFile () << ":" << ex.getLine () << "\n";
        // The context must not be used after an exception: log the error,
        // then stop or restart this unit of work.
        return 0;
    }
    std::cout << "The write past the end was not detected\n";
    return 1;
}
