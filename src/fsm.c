#include "fsm.h"

static short signal_symbol[1][128];

static short identifier[2][128];

static short string[4][128];

void fsm_signal_symbol_init()
{
    for (int r = 0; r < 1; r++)
        for (int c = 0; c < 128; c++)
        signal_symbol[r][c] = -1;

    signal_symbol[0][' '] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['\t'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['\n'] = TOKEN_TYPE_NEWLINE;
    signal_symbol[0]['('] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['{'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['['] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0][']'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['}'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0][')'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0][','] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['.'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0][':'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['|'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['&'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['='] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['>'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['<'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['?'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['*'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['/'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['+'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['-'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['%'] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0][';'] = TOKEN_TYPE_EXPEND;
    signal_symbol[0]['\''] = TOKEN_TYPE_SYMBOL;
    signal_symbol[0]['"'] = TOKEN_TYPE_SYMBOL;
}

int fsm_signal_symbol_next(int state, char c)
{
    return signal_symbol[state][c];
}

void fsm_identifier_init()
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 128; c++)
            identifier[r][c] = -1;
    
    for (char c = 'A'; c <= 'Z'; c++) {
        identifier[0][c] = 1;
        identifier[1][c] = 1;
    }
    for (char c = 'a'; c <= 'z'; c++) {
        identifier[0][c] = 1;
        identifier[1][c] = 1;
    }
    for (char c = '0'; c <= '9'; c++) {
        identifier[1][c] = 1;
    }
    identifier[0]['_'] = 1;
    identifier[0]['$'] = 1;
    identifier[1][' '] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['\t'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['\n'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['['] = TOKEN_TYPE_IDENTIFIER;
    identifier[1][']'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['{'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['}'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['('] = TOKEN_TYPE_IDENTIFIER;
    identifier[1][')'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1][','] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['.'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1][':'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['?'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['+'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['-'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['*'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['/'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['='] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['>'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['<'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['%'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['|'] = TOKEN_TYPE_IDENTIFIER;
    identifier[1]['&'] = TOKEN_TYPE_IDENTIFIER;
}

int fsm_identifier_next(int state, char c)
{
    return identifier[state][c];
}

void fsm_string_init()
{
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 128; c++)
            string[r][c] = -1;
    
    string[0]['"'] = 1;

    for (int c = 0; c < 128; c++)
        string[1][c] = 1;
    string[1]['"'] = 3;
    string[1]['\\'] = 2;
    for (int c = 0; c < 128; c++)
        string[2][c] = 1;
    string[3][' '] = TOKEN_TYPE_STRING;
    string[3]['\t'] = TOKEN_TYPE_STRING;
    string[3]['\n'] = TOKEN_TYPE_STRING;
    string[3][';'] = TOKEN_TYPE_STRING;
    string[3]['+'] = TOKEN_TYPE_STRING;
    string[3][')'] = TOKEN_TYPE_STRING;
    string[3]['.'] = TOKEN_TYPE_STRING;
}

int fsm_string_next(int state, char c)
{
    return string[state][c];
}