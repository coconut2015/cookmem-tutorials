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
// Do the work in a short-lived context, and copy only the results that must
// survive into a longer-lived context before the short-lived one goes away.
#include <iostream>

#include "parser.h"

// A result that outlives the work: the name and value of each definition.
// Its memory comes from the long-lived context.
struct Definition
{
    String name;
    double value;

    Definition (Context& memCtx, const String& defName, double defValue)
    : name (defName.c_str (), cookmem::Allocator<char> (memCtx)),
      value (defValue)
    {
    }
};

int
main ()
{
    // The long-lived context, for example one per program or per session.
    Context session;
    List<Definition*> definitions ((cookmem::Allocator<Definition*> (session)));

    const char* source[][2] = {
        { "width", "4 * 20" },
        { "height", "max(30, 5 * 7)" },
        { "area", "4 * 20 * max(30, 5 * 7)" },
    };

    {
        // The short-lived context shares the session's cached segments, so
        // creating and releasing it is cheap.
        Context work (session, true);
        Variables vars;
        for (const auto& def : source)
        {
            Node* tree = Parser (work, def[1]).parse ();
            double value = evaluate (tree, vars);

            // Copy the result into the session before work is released.
            String name (def[0], cookmem::Allocator<char> (work));
            definitions.push_back (session.create<Definition> (session, name, value));
        }
        std::cout << "Work context footprint: " << work.getFootprint () << "\n";

        // contains () tells which context a pointer belongs to.
        // Expect output Result in session: 1, in work: 0
        std::cout << "Result in session: " << session.contains (definitions[0])
                  << ", in work: " << work.contains (definitions[0]) << "\n";
    }   // work is destroyed: every tree is freed at once

    for (const Definition* def : definitions)
    {
        std::cout << def->name << " = " << def->value << "\n";
    }
    return 0;
}
