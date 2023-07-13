#ifndef JAVAC_FSM_H
#define JAVAC_FSM_H

#define TOKEN_TYPE_SYMBOL       0x101
#define TOKEN_TYPE_NEWLINE      0x102
#define TOKEN_TYPE_EXPEND       0x103
#define TOKEN_TYPE_IDENTIFIER   0x104
#define TOKEN_TYPE_KEYWORD      0x105
#define TOKEN_TYPE_STRING       0x106

void fsm_signal_symbol_init();
int fsm_signal_symbol_next(int state, char c);

void fsm_identifier_init();
int fsm_identifier_next(int state, char c);

void fsm_string_init();
int fsm_string_next(int state, char c);
#endif