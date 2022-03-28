#ifndef AST_HPP
#define AST_HPP

/* ----------------------      all headers in AST          ---------------------- */
#include "ast5/ast5_context.hpp"
#include "ast5/ast5_declaration.hpp"
#include "ast5/ast5_expression.hpp"
#include "ast5/ast5_begin.hpp"
#include "ast5/ast5_statement.hpp"

/* ----------------------          all libraries            ---------------------- */
#include "stdio.h"
#include <iostream>
#include <fstream> 
#include <string>
#include <cstdlib>
#include <typeinfo>

extern const Node* parse();
extern FILE* yyin;		 

#endif