#ifndef JAVAC_FSM_H
#define JAVAC_FSM_H

void fsm_signal_symbol_init();
int fsm_signal_symbol_next(int state, char c);

void fsm_identifier_init();
int fsm_identifier_next(int state, char c);

void fsm_string_init();
int fsm_string_next(int state, char c);

void fsm_number_init();
int fsm_number_next(int state, char c);

void fsm_operator_init();
int fsm_operator_next(int state, char c);

void fsm_quote_init();
int fsm_quote_next(int state, unsigned char c);

void fsm_comment_init();
int fsm_comment_next(int state, char c);
#endif