#include "parser.h"

#define OPERATOR_SIZE   34

static operator_precedence op_precedence[] = {
    // 1
    {.operator = "++", .ec = 1, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 1},
    {.operator = "--", .ec = 1, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 1},
    {.operator = "!", .ec = 1, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 1},
    {.operator = "~", .ec = 1, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 1},
    // 2
    {.operator = "*", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 2},
    {.operator = "/", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 2},
    {.operator = "%", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 2},
    // 3
    {.operator = "+", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 3},
    {.operator = "-", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 3},
    // 4
    {.operator = "<<", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 4},
    {.operator = ">>", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 4},
    {.operator = ">>>", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 4},
    // 5
    {.operator = "<", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 5},
    {.operator = "<=", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 5},
    {.operator = ">", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 5},
    {.operator = ">=", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 5},
    {.operator = "instanceof", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 5},
    // 6
    {.operator = "==", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 6},
    {.operator = "!=", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 6},
    // 7
    {.operator = "&", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 7},
    // 8
    {.operator = "^", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 8},
    // 9
    {.operator = "|", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 9},
    // 10
    {.operator = "&&", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 10},
    // 11
    {.operator = "||", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 11},
    // 12
    {.operator = "?", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 12},
    {.operator = ":", .ec = 2, .order = OPERATOR_PRECEDENCE_LEFT_TO_RIGHT, .precedence = 12},
    // 13
    {.operator = "=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "+=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "-=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "*=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "/=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "%=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "<<=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = ">>=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = ">>>=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "&=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "^=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13},
    {.operator = "|=", .ec = 2, .order = OPERATOR_PRECEDENCE_RIGHT_TO_LEFT, .precedence = 13}
};

static bool operator_is_prioritized(const char *op1, const char *op2)
{
    operator_precedence *opp1 = NULL, *opp2 = NULL, *entry = NULL;
    for (int i = 0; i < OPERATOR_SIZE; i ++) {
        if (opp1 && opp2)
            break;
        entry = &op_precedence[i];
        if (SEQ(entry->operator, op1)) {
            opp1 = &op_precedence[i];
            continue;
        } else if (SEQ(entry->operator, op2)) {
            opp2 = &op_precedence[i];
            continue;
        }
    }

    if (!opp1 || !opp2)
        return false;
    
    return opp1->precedence < opp2->precedence;
}

static operator_precedence *operator_get_precedence(const char *op)
{
    operator_precedence *opp = NULL;
    for (int i = 0; i < OPERATOR_SIZE; i ++) {
        if (SEQ(op_precedence[i].operator, op)) {
            opp = &op_precedence[i];
            break;
        }
    }
    return opp;
}

int parse_expression(struct vector *expression)
{
    struct vector *ops = vector_create(sizeof(node*));
    struct vector *values = vector_create(sizeof(node*));
    node *n = vector_peek(expression);
    while (n) {
        vector_push(n->value.ttype == TOKEN_TYPE_OPERATOR ? ops : values, &n);
        n++;
    }
    return JAVAC_PARSE_OK;
}

int parse(lexer_process *process)
{
    node_set_vector(process->compiler->ast.expressions, process->compiler->ast.statements);
    // 解析toke列表
    bool exp = false;
    token *elem = (token*)vector_peek(process->tokens);
    // 区分Expression和Statement
    while(elem && elem->type) {
        if (elem->type == TOKEN_TYPE_OPERATOR && SEQ(elem->sval, "=")) {
            exp = true;
            elem ++;
            continue;
        } else if (elem->type == TOKEN_TYPE_SYMBOL && elem->cval == ';' && exp) {
            node_push_delimiter(NODE_TYPE_EXPRESSION);
            exp = false;
            elem ++;
            continue;
        }
        
        if (exp) 
            node_create_expression(process, elem);
        elem ++;
    };

    // parse expression
    if (parse_expression(process->compiler->ast.expressions) != JAVAC_PARSE_OK)
        return JAVAC_PARSE_ERROR;

    return JAVAC_PARSE_OK;
}