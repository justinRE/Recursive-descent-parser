// Assignment4CS335.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <queue>

using namespace std;
bool debug = false;
// TOKEN TYPES
typedef enum {
    t_EOF,
    t_read,
    t_write,
    t_id,
    t_sum,
    t_add,
    t_sub,
    t_mul,
    t_div,
    t_left,
    t_right,
    t_literal
} token;

inline const char* ToString(token v) {
    switch (v) {
    case t_EOF:
        return "$$";
    case t_read:
        return "read";
    case t_write:
        return "write";
    case t_id:
        return "id";
    case t_sum:
        return ":=";
    case t_add:
        return "+";
    case t_sub:
        return "-";
    case t_mul:
        return "*";
    case t_div:
        return "/";
    case t_left:
        return "(";
    case t_right:
        return ")";
    case t_literal:
        return "number";
    default:
        return "[Unknown token]";
    }
}

// TOKEN STACK
queue<token> tokens;
// called functions
vector<string> functions;

// util functions
bool is_number(string&);

// Building stack
void scan();

// program functions
void program();
void stmt_list();
void stmt();
void match(token);
void expr();
void term();
void term_tail();
void factor_tail();
void factor();
void add_op();
void mult_op();

void parser_error(string);

int main() {
    if (debug)
        cout << "== INPUT ==" << endl;
    scan();

    cout << "== PROGRAM ==" << endl;
    program();

    return 0;
}

void scan() {

    // open file
    ifstream file;
    file.open("input.txt");

    if (file.is_open()) {
        // for line in file
        // if line == "read"
        // tokens.push(t_read);
        // etc.
        string line;

        while (getline(file, line)) {

            if (line == "$$") {
                if (debug)
                    cout << "read EOF token" << endl;
                tokens.push(t_EOF);
            }

            else if (!is_number(line)) {
                if (line == "read") {
                    if (debug)
                        cout << "read token" << endl;
                    tokens.push(t_read);
                }
                else if (line == "write") {
                    if (debug)
                        cout << "read write" << endl;
                    tokens.push(t_write);
                }
                else if (line == ":=") {
                    if (debug)
                        cout << "read := (t_sum)" << endl;
                    tokens.push(t_sum);
                }
                else if (line == "+") {
                    if (debug)
                        cout << "read + (t_add)" << endl;
                    tokens.push(t_add);
                }
                else if (line == "-") {
                    if (debug)
                        cout << "read - (t_sub)" << endl;
                    tokens.push(t_sub);
                }
                else if (line == "*") {
                    if (debug)
                        cout << "read * token" << endl;
                    tokens.push(t_mul);
                }
                else if (line == "/") {
                    if (debug)
                        cout << "read / token" << endl;
                    tokens.push(t_div);
                }
                else if (line == "(") {
                    if (debug)
                        cout << "read ( token" << endl;
                    tokens.push(t_left);
                }
                else if (line == ")") {
                    if (debug)
                        cout << "read ) token" << endl;
                    tokens.push(t_right);
                }

                else {
                    if (debug)
                        cout << "read id (" << line << ")" << endl;
                    tokens.push(t_id);
                }
            }
            else {
                if (debug)
                    cout << "read literal num: " << line << endl;
                tokens.push(t_literal);
            }
        }
    }
    file.close();
}

/** Program functions **/
void program() {
    functions.push_back("program()");
    switch (tokens.front()) {
    case t_id:
    case t_read:
    case t_write:
    case t_EOF:
        stmt_list();
        match(t_EOF);
        break;
    default:
        parser_error("Program");
        break;
    }
}

void match(token expected) {
    if (tokens.front() == expected) {
        // consume input token

        if (functions.size()) {
            cout << "Executed: ";
            for (string i : functions) {
                cout << i << " ";
            }
            cout << endl;
        }

        functions.clear();

        cout << "Token: " << ToString(tokens.front()) << " consumed." << endl;
        tokens.pop();
    }
    else {
        cout << "error: expected: " << ToString(expected)
            << " got: " << ToString(tokens.front()) << endl;
        parser_error("match");
    }
}

void stmt_list() {
    functions.push_back("stmt_list()");
    switch (tokens.front()) {
    case t_id:
    case t_read:
    case t_write:
        stmt();
        stmt_list();
        break;
    case t_EOF:
        functions.push_back("stmt_list()");
        break;
    default:
        parser_error("stmt_list");
        break;
    }
}

void stmt() {
    functions.push_back("stmt()");
    switch (tokens.front()) {
    case t_id:
        match(t_id);
        match(t_sum);
        expr();
        break;
    case t_read:
        match(t_read);
        match(t_id);
        break;
    case t_write:
        match(t_write);
        expr();
        break;
    default:
        parser_error("stmt");
        break;
    }
}

void expr() {
    functions.push_back("expr()");
    switch (tokens.front()) {
    case t_id:
    case t_literal:
    case t_left:
        term();
        term_tail();
        break;
    default:
        parser_error("expr");
        break;
    }
}

void term() {
    functions.push_back("term()");
    switch (tokens.front()) {
    case t_id:
    case t_literal:
    case t_left:
        factor();
        factor_tail();
        break;
    default:
        break;
    }
}
void term_tail() {
    functions.push_back("term_tail()");
    switch (tokens.front()) {
    case t_add:
    case t_sub:
        add_op();
        term();
        term_tail();
        break;
    case t_right:
    case t_id:
    case t_read:
    case t_write:
    case t_EOF:
        break;
    default:
        parser_error("term_tail()");
    }
}
void factor_tail() {
    functions.push_back("factor_tail()");
    switch (tokens.front()) {
    case t_mul:
    case t_div:
        mult_op();
        factor();
        factor_tail();
        break;
    case t_add:
    case t_sub:
    case t_right:
    case t_id:
    case t_read:
    case t_write:
    case t_EOF:
        break;
    default:
        parser_error("factor_tail()");
    }
}
void factor() {
    functions.push_back("factor()");
    switch (tokens.front()) {
    case t_id:
        match(t_id);
        break;
    case t_literal:
        match(t_literal);
        break;
    case t_left:
        match(t_left);
        expr();
        match(t_right);
        break;
    default:
        parser_error("factor()");
    }
}
void add_op() {
    functions.push_back("add_op()");
    switch (tokens.front()) {
    case t_add:
        match(t_add);
        break;
    case t_sub:
        match(t_sub);
        break;
    default:
        parser_error("add_op()");
    }
}
void mult_op() {
    functions.push_back("mult_op()");
    switch (tokens.front()) {
    case t_mul:
        match(t_mul);
        break;
    case t_div:
        match(t_div);
        break;
    default:
        parser_error("mult_op()");
    }
}

void parser_error(string location) {
    cout << "Error parsing Location: " << location
        << "- last token: " << ToString(tokens.front()) << endl;
    exit(0);
}

/** UTIL FUNCTIONS **/
bool is_number(string& in) {
    try {

        stoi(in);
        return true;
    }
    catch (invalid_argument) {
        return false;
    }
}
