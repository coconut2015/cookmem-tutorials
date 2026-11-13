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
// A long-lived tree that keeps changing.  Replaced nodes are freed one at a
// time with destroy (), and the context reuses their memory, so the footprint
// stays flat.  An arena that can only free everything at once would keep
// growing instead.
#include <iostream>

#include "ast.h"

static Node*
number (Context& memCtx, double value)
{
    Node* node = memCtx.create<Node> (memCtx, Node::NUMBER);
    node->value = value;
    return node;
}

// Fold a BINARY node of two numbers into one number, and free the three
// nodes it replaces.
static Node*
fold (Context& memCtx, Node* node)
{
    Node* left = node->children[0];
    Node* right = node->children[1];
    Node* result = number (memCtx, evaluate (node, Variables ()));
    memCtx.destroy (left);
    memCtx.destroy (right);
    memCtx.destroy (node);     // also frees its list of children
    return result;
}

int
main ()
{
    Context memCtx;

    // A call with 100 arguments: min(0, 1, 2, ..., 99).
    Node* call = memCtx.create<Node> (memCtx, Node::CALL);
    call->name = "min";
    for (int i = 0; i < 100; ++i)
    {
        call->children.push_back (number (memCtx, i));
    }

    std::size_t firstFootprint = 0;
    for (int pass = 1; pass <= 100000; ++pass)
    {
        // An edit replaces one argument with (i + 1)...
        std::size_t i = pass % call->children.size ();
        Node* sum = memCtx.create<Node> (memCtx, Node::BINARY);
        sum->op = '+';
        sum->children.push_back (call->children[i]);
        sum->children.push_back (number (memCtx, 1));
        call->children[i] = sum;

        // ...and the optimizer folds it back into a number.
        call->children[i] = fold (memCtx, sum);

        if (pass == 1)
        {
            firstFootprint = memCtx.getFootprint ();
        }
        if (pass == 1 || pass == 1000 || pass == 100000)
        {
            std::cout << "Pass " << pass << ": footprint " << memCtx.getFootprint () << "\n";
        }
    }

    // Expect output = 1000
    std::cout << "= " << evaluate (call, Variables ()) << "\n";
    return memCtx.getFootprint () == firstFootprint ? 0 : 1;
}
