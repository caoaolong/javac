#ifndef JAVAC_PARSER_H
#define JAVAC_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "helpers/vector.h"

int parse(lexer_process *process);

#endif