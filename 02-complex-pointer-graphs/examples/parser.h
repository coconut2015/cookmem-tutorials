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
// A recursive-descent parser for arithmetic expressions, which creates the
// nodes of the tree in a memory context.
//
// Grammar:
//
//   expression := term (('+' | '-') term)*
//   term       := factor (('*' | '/') factor)*
//   factor     := number | name | name '(' arguments ')' | '(' expression ')'
//               | '-' factor
//   arguments  := expression (',' expression)*
#ifndef TUTORIAL_PARSER_H
#define TUTORIAL_PARSER_H

#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "ast.h"

// A recursive-descent parser.  Every node it creates comes from memCtx; it
// never frees a node, not even when it fails part way through.
class Parser
{
public:
    Parser (Context& memCtx, const char* text)
    : m_memCtx (memCtx), m_pos (text)
    {
    }

    Node*
    parse ()
    {
        Node* node = parseExpression ();
        skipSpaces ();
        if (*m_pos != '\0')
        {
            fail ("unexpected text");
        }
        return node;
    }

private:
    Node*
    parseExpression ()
    {
        Node* left = parseTerm ();
        while (accept ('+') || accept ('-'))
        {
            // Read the operator before parsing the right side changes it.
            char op = m_last;
            Node* right = parseTerm ();
            left = binary (op, left, right);
        }
        return left;
    }

    Node*
    parseTerm ()
    {
        Node* left = parseFactor ();
        while (accept ('*') || accept ('/'))
        {
            char op = m_last;
            Node* right = parseFactor ();
            left = binary (op, left, right);
        }
        return left;
    }

    Node*
    parseFactor ()
    {
        skipSpaces ();
        if (accept ('-'))
        {
            Node* node = make<Node> (m_memCtx, Node::NEGATE);
            node->children.push_back (parseFactor ());
            return node;
        }
        if (accept ('('))
        {
            Node* node = parseExpression ();
            expect (')');
            return node;
        }
        if (std::isdigit (static_cast<unsigned char> (*m_pos)) || *m_pos == '.')
        {
            char* end;
            Node* node = make<Node> (m_memCtx, Node::NUMBER);
            node->value = std::strtod (m_pos, &end);
            m_pos = end;
            return node;
        }
        if (std::isalpha (static_cast<unsigned char> (*m_pos)))
        {
            const char* start = m_pos;
            while (std::isalnum (static_cast<unsigned char> (*m_pos)))
            {
                ++m_pos;
            }
            if (accept ('('))
            {
                Node* node = make<Node> (m_memCtx, Node::CALL);
                node->name.assign (start, nameLength (start));
                do
                {
                    node->children.push_back (parseExpression ());
                }
                while (accept (','));
                expect (')');
                return node;
            }
            Node* node = make<Node> (m_memCtx, Node::VARIABLE);
            node->name.assign (start, nameLength (start));
            return node;
        }
        fail ("expected a number, a name or '('");
        return nullptr;
    }

    Node*
    binary (char op, Node* left, Node* right)
    {
        Node* node = make<Node> (m_memCtx, Node::BINARY);
        node->op = op;
        node->children.push_back (left);
        node->children.push_back (right);
        return node;
    }

    static std::size_t
    nameLength (const char* start)
    {
        std::size_t n = 0;
        while (std::isalnum (static_cast<unsigned char> (start[n])))
        {
            ++n;
        }
        return n;
    }

    void
    skipSpaces ()
    {
        while (std::isspace (static_cast<unsigned char> (*m_pos)))
        {
            ++m_pos;
        }
    }

    bool
    accept (char c)
    {
        skipSpaces ();
        if (*m_pos == c)
        {
            m_last = c;
            ++m_pos;
            return true;
        }
        return false;
    }

    void
    expect (char c)
    {
        if (!accept (c))
        {
            fail (std::string ("expected '") + c + "'");
        }
    }

    void
    fail (const std::string& message)
    {
        throw std::runtime_error (message + " at \"" + m_pos + "\"");
    }

    Context&    m_memCtx;
    const char* m_pos;
    char        m_last = 0;
};

#endif // TUTORIAL_PARSER_H
