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
// A logger that numbers every allocation, so that a leak report can say
// which allocation was not freed, and a debugger can stop at it next time.
// It is turned on at startup; when it is off, each call costs one branch.
#ifndef TUTORIAL_NUMBERING_LOGGER_H
#define TUTORIAL_NUMBERING_LOGGER_H

#include <cstddef>
#include <unordered_map>

#include <cookmem.h>

// Called when the allocation numbered breakOn is made.  Put a breakpoint
// here to stop at the allocation that leaked.
inline void
onBreakAllocation (unsigned long number)
{
    (void) number;
}

// A CookMem logger.  Like the loggers in CookMem, it implements the
// MemLogger interface without inheriting from it, so the calls can be
// inlined.
class NumberingLogger
{
public:
    explicit NumberingLogger (bool enabled = true, unsigned long breakOn = 0)
    : m_enabled (enabled), m_breakOn (breakOn), m_next (0)
    {
    }

    // The number of an allocation that is still alive, or 0.
    unsigned long
    numberOf (void* ptr) const
    {
        std::unordered_map<void*, unsigned long>::const_iterator it = m_numbers.find (ptr);
        return it == m_numbers.end () ? 0 : it->second;
    }

    void
    logAllocation (void* ptr, std::size_t, cookmem::MemAllocType_et)
    {
        if (!m_enabled || ptr == nullptr)
        {
            return;
        }
        unsigned long number = ++m_next;
        m_numbers[ptr] = number;
        if (number == m_breakOn)
        {
            onBreakAllocation (number);
        }
    }

    void
    logDeallocation (void* ptr, std::size_t)
    {
        if (m_enabled)
        {
            m_numbers.erase (ptr);
        }
    }

    // A reallocation in place keeps its address, and so its number.
    void logReallocation (void*, std::size_t, std::size_t) { }
    void logGetSegment (void*, std::size_t) { }
    void logFreeSegment (void*, std::size_t) { }
    void logTrimEmptySegments () { }

    // Errors are reported by throwing, like the loggers of CookMem.
    void
    logError (void*, cookmem::MemError_et error, const char* message,
              const char* file = __FILE__, int line = __LINE__)
    {
        throw cookmem::Exception (error, message, file, line);
    }

private:
    bool                                        m_enabled;
    unsigned long                               m_breakOn;
    unsigned long                               m_next;
    std::unordered_map<void*, unsigned long>    m_numbers;
};

#endif // TUTORIAL_NUMBERING_LOGGER_H
