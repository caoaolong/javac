#include "fsm.h"
#include "token.h"

#define STATE_SIZE              128

#define SIGNAL_SYMBOL_STATES    1
static short signal_symbol[SIGNAL_SYMBOL_STATES][STATE_SIZE];

#define IDENTIFIER_STATES       2
static short identifier[IDENTIFIER_STATES][STATE_SIZE];

#define STRING_STATES           4
static short string[STRING_STATES][STATE_SIZE];

#define NUMBER_STATES           16
static short number[NUMBER_STATES][STATE_SIZE];

#define OPERATOR_STATES         17
static short operator[OPERATOR_STATES][STATE_SIZE];

void fsm_signal_symbol_init()
{
    for (int r = 0; r < SIGNAL_SYMBOL_STATES; r++)
        for (int c = 0; c < STATE_SIZE; c++)
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
    for (int r = 0; r < IDENTIFIER_STATES; r++)
        for (int c = 0; c < STATE_SIZE; c++)
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
    identifier[1][';'] = TOKEN_TYPE_IDENTIFIER;
}

int fsm_identifier_next(int state, char c)
{
    return identifier[state][c];
}

void fsm_string_init()
{
    for (int r = 0; r < STRING_STATES; r++)
        for (int c = 0; c < STATE_SIZE; c++)
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

void fsm_number_init()
{
    for (int r = 0; r < NUMBER_STATES; r++)
        for (int c = 0; c < STATE_SIZE; c++)
            number[r][c] = -1;
    
    number[0]['-'] = number[0]['+'] = 1;
    number[0]['0'] = 3;
    for (int i = '1'; i <= '9'; i++) {
        number[0][i] = 2;
        number[15][i] = 15;
    }
    for (int i = '0'; i <= '9'; i++) {
        number[1][i] = number[2][i] = number[3][i] = 2;
        number[10][i] = number[12][i] = 12;
        number[11][i] = number[13][i] = 13;
    }
    number[2]['.'] = 10;
    number[2]['e'] = number[2]['E'] = 11;
    number[12]['e'] = number[12]['E'] = 15;
    number[13]['l'] = number[13]['L'] = 14;
    number[12]['f'] = number[12]['F'] = number[15]['f'] = number[15]['F'] = 14;
    number[3]['b'] = 4;
    number[4]['0'] = number[4]['1'] = 7;
    number[7]['0'] = number[7]['1'] = 7;
    number[3]['o'] = 5;
    for (int i = '0'; i <= '7'; i++)
        number[5][i] = number[8][i] = 8;
    number[3]['x'] = 6;
    for (int i = '0'; i <= '9'; i++)
        number[6][i] = number[9][i] = 9;
    for (int i = 'a'; i <= 'f'; i++)
        number[6][i] = number[9][i] = 9;
    for (int i = 'A'; i <= 'F'; i++)
        number[6][i] = number[9][i] = 9;
    
    // SYMBOL: + - * / % | & ? ] ) \b \n \t
    number[13]['+'] = number[2]['+'] = number[15]['+'] = number[14]['+'] = number[7]['+'] = number[8]['+'] = number[9]['+'] = TOKEN_TYPE_NUMBER;
    number[13]['-'] = number[2]['-'] = number[15]['-'] = number[14]['-'] = number[7]['-'] = number[8]['-'] = number[9]['-'] = TOKEN_TYPE_NUMBER;
    number[13]['*'] = number[2]['*'] = number[15]['*'] = number[14]['*'] = number[7]['*'] = number[8]['*'] = number[9]['*'] = TOKEN_TYPE_NUMBER;
    number[13]['/'] = number[2]['/'] = number[15]['/'] = number[14]['/'] = number[7]['/'] = number[8]['/'] = number[9]['/'] = TOKEN_TYPE_NUMBER;
    number[13]['&'] = number[2]['&'] = number[15]['&'] = number[14]['&'] = number[7]['&'] = number[8]['&'] = number[9]['&'] = TOKEN_TYPE_NUMBER;
    number[13]['|'] = number[2]['|'] = number[15]['|'] = number[14]['|'] = number[7]['|'] = number[8]['|'] = number[9]['|'] = TOKEN_TYPE_NUMBER;
    number[13]['%'] = number[2]['%'] = number[15]['%'] = number[14]['%'] = number[7]['%'] = number[8]['%'] = number[9]['%'] = TOKEN_TYPE_NUMBER;
    number[13][']'] = number[2][']'] = number[15][']'] = number[14][']'] = number[7][']'] = number[8][']'] = number[9][']'] = TOKEN_TYPE_NUMBER;
    number[13][')'] = number[2][')'] = number[15][')'] = number[14][')'] = number[7][')'] = number[8][')'] = number[9][')'] = TOKEN_TYPE_NUMBER;
    number[13][';'] = number[2][';'] = number[15][';'] = number[14][';'] = number[7][';'] = number[8][';'] = number[9][';'] = TOKEN_TYPE_NUMBER;
    number[13][' '] = number[2][' '] = number[15][' '] = number[14][' '] = number[7][' '] = number[8][' '] = number[9][' '] = TOKEN_TYPE_NUMBER;
    number[13]['?'] = number[2]['?'] = number[15]['?'] = number[14]['?'] = number[7]['?'] = number[8]['?'] = number[9]['?'] = TOKEN_TYPE_NUMBER;
    number[13]['\t'] = number[2]['\t'] = number[15]['\t'] = number[14]['\t'] = number[7]['\t'] = number[8]['\t'] = number[9]['\t'] = TOKEN_TYPE_NUMBER;
    number[13]['\n'] = number[2]['\n'] = number[15]['\n'] = number[14]['\n'] = number[7]['\n'] = number[8]['\n'] = number[9]['\n'] = TOKEN_TYPE_NUMBER;
}

int fsm_number_next(int state, char c)
{
    return number[state][c];
}

void fsm_operator_init()
{
    for (int r = 0; r < OPERATOR_STATES; r++)
        for (int c = 0; c < STATE_SIZE; c++)
            operator[r][c] = -1;

    operator[0]['*'] = operator[0]['/'] = operator[0]['%'] = operator[0]['!'] = operator[0]['^'] = 1;
    operator[1]['='] = 14;
    operator[0]['+'] = 2;
    operator[2]['+'] = 15;
    operator[2]['='] = 14;
    operator[0]['-'] = 3;
    operator[3]['-'] = 16;
    operator[3]['='] = 14;
    operator[0]['|'] = 4;
    operator[4]['|'] = operator[4]['='] = 14;
    operator[0]['&'] = 5;
    operator[5]['&'] = operator[5]['='] = 14;
    
    operator[0]['>'] = 6;
    operator[6]['>'] = 8;
    operator[6]['='] = 10;
    operator[8]['='] = 10;

    operator[0]['<'] = 7;
    operator[7]['<'] = 9;
    operator[7]['='] = 11;
    operator[9]['='] = 11;

    operator[0]['='] = 12;
    operator[12]['='] = 13;

    for (int i = '0'; i <= '9'; i++) {
        for (int s = 1; s < OPERATOR_STATES; s++) 
            operator[s][i] = TOKEN_TYPE_OPERATOR;
    }
    for (int i = 'a'; i <= 'z'; i++) {
        operator[1][i] = TOKEN_TYPE_OPERATOR;
        operator[14][i] = TOKEN_TYPE_OPERATOR;
        operator[15][i] = operator[16][i] = TOKEN_TYPE_OPERATOR;
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        operator[1][i] = TOKEN_TYPE_OPERATOR;
        operator[14][i] = TOKEN_TYPE_OPERATOR;
        operator[15][i] = operator[16][i] = TOKEN_TYPE_OPERATOR;
    }
    for (int i = 1; i < OPERATOR_STATES; i++)
        operator[i]['_'] = operator[i][' '] = operator[i]['\t'] = 
        operator[i]['\n'] = operator[i][';'] = operator[i]['('] = TOKEN_TYPE_OPERATOR;
    operator[15][']'] = operator[16][']'] = 
    operator[15][';'] = operator[16][';'] = 
    operator[15][' '] = operator[16]['\t'] = 
    operator[15]['\n'] = operator[16]['+'] = 
    operator[15]['-'] = operator[16]['*'] = operator[15]['|'] = operator[16]['&'] = 
    operator[15]['/'] = operator[16]['%'] = TOKEN_TYPE_OPERATOR;
}

int fsm_operator_next(int state, char c)
{
    return operator[state][c];
}