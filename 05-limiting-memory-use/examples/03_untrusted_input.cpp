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
// Untrusted input: a record says how large its payload is, and a decoder
// trusts it.  A malicious record claims gigabytes.  With a footprint limit,
// the allocation fails and the decoder rejects the record, instead of the
// process running out of memory.
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include <cookmem.h>

typedef cookmem::SimpleMemContext<> Context;

// A record: a 4-byte length, then the payload.
static std::vector<std::uint8_t>
makeRecord (std::uint32_t claimedLength, const char* payload)
{
    std::vector<std::uint8_t> record (4);
    std::memcpy (record.data (), &claimedLength, 4);
    record.insert (record.end (), payload, payload + std::strlen (payload));
    return record;
}

// Decode a record into memory from memCtx.  Returns nullptr if the record
// is rejected.
static char*
decode (Context& memCtx, const std::vector<std::uint8_t>& record)
{
    std::uint32_t length;
    std::memcpy (&length, record.data (), 4);

    // The decoder allocates what the record claims, as many real decoders
    // do for speed.  The limit is what keeps a bad claim from hurting.
    char* payload = static_cast<char*> (memCtx.allocate (length + std::size_t (1)));
    if (payload == nullptr)
    {
        return nullptr;
    }
    std::size_t available = record.size () - 4;
    std::size_t n = available < length ? available : length;
    std::memcpy (payload, record.data () + 4, n);
    payload[n] = '\0';
    return payload;
}

int
main ()
{
    // One context per input, with a limit sized for legitimate input.
    Context memCtx;
    memCtx.setFootprintLimit (16 * 1024 * 1024);    // 16 MB

    std::vector<std::uint8_t> good = makeRecord (5, "hello");
    std::vector<std::uint8_t> bomb = makeRecord (0xF0000000u, "boom");   // claims 3.75 GB

    char* text = decode (memCtx, good);
    // Expect output Good record: hello
    std::cout << "Good record: " << (text ? text : "(rejected)") << "\n";

    char* bad = decode (memCtx, bomb);
    // Expect output Bad record: rejected
    std::cout << "Bad record: " << (bad ? "accepted" : "rejected") << "\n";

    memCtx.releaseAll ();
    return (text != nullptr && bad == nullptr) ? 0 : 1;
}
