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
// The nodes of an arithmetic expression tree, allocated in a memory context.
#ifndef TUTORIAL_AST_H
#define TUTORIAL_AST_H

#include <algorithm>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

#include "context.h"

// An expression node.  Its name and children come from the same context as
// the node, so the whole tree owns nothing but memory from that context.
struct Node
{
    enum Kind { NUMBER, VARIABLE, NEGATE, BINARY, CALL };

    Kind        kind;
    double      value;      // NUMBER
    char        op;         // BINARY: + - * /
    String      name;       // VARIABLE and CALL
    List<Node*> children;   // NEGATE: 1, BINARY: 2, CALL: the arguments

    Node (Context& memCtx, Kind k)
    : kind (k),
      value (0),
      op (0),
      name (cookmem::Allocator<char> (memCtx)),
      children (cookmem::Allocator<Node*> (memCtx))
    {
    }
};

// Print a tree in prefix form, such as (+ 1 (* 2 x)).
inline void
print (std::ostream& os, const Node* node)
{
    switch (node->kind)
    {
        case Node::NUMBER:
            os << node->value;
            break;
        case Node::VARIABLE:
            os << node->name;
            break;
        case Node::NEGATE:
            os << "(- ";
            print (os, node->children[0]);
            os << ")";
            break;
        case Node::BINARY:
            os << "(" << node->op << " ";
            print (os, node->children[0]);
            os << " ";
            print (os, node->children[1]);
            os << ")";
            break;
        case Node::CALL:
            os << "(" << node->name;
            for (const Node* arg : node->children)
            {
                os << " ";
                print (os, arg);
            }
            os << ")";
            break;
    }
}

typedef std::map<std::string, double> Variables;

// Evaluate a tree.  The functions min and max take any number of arguments.
inline double
evaluate (const Node* node, const Variables& vars)
{
    switch (node->kind)
    {
        case Node::NUMBER:
            return node->value;
        case Node::VARIABLE:
        {
            Variables::const_iterator it = vars.find (std::string (node->name.c_str ()));
            if (it == vars.end ())
            {
                throw std::runtime_error ("unknown variable " + std::string (node->name.c_str ()));
            }
            return it->second;
        }
        case Node::NEGATE:
            return -evaluate (node->children[0], vars);
        case Node::BINARY:
        {
            double left = evaluate (node->children[0], vars);
            double right = evaluate (node->children[1], vars);
            switch (node->op)
            {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                default: return left / right;
            }
        }
        case Node::CALL:
        {
            bool isMax = node->name == "max";
            if (!isMax && node->name != "min")
            {
                throw std::runtime_error ("unknown function " + std::string (node->name.c_str ()));
            }
            double result = evaluate (node->children[0], vars);
            for (std::size_t i = 1; i < node->children.size (); ++i)
            {
                double v = evaluate (node->children[i], vars);
                // (std::max) keeps the max and min macros of windows.h from expanding.
                result = isMax ? (std::max) (result, v) : (std::min) (result, v);
            }
            return result;
        }
    }
    return 0;
}

#endif // TUTORIAL_AST_H
