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

static void parse_expression_node_pop(struct vector *ops, struct vector *values, node *n)
{
    if (n->value.ttype == TOKEN_TYPE_SYMBOL && n->exp.symbol == ')') {
        n = vector_back(ops);
        parse_expression_node_pop(ops, values, n);
        return;
    } else if (n->value.ttype == TOKEN_TYPE_SYMBOL && n->exp.symbol == '(') {
        vector_pop(ops);
        n = vector_back(ops);
        parse_expression_node_pop(ops, values, n);
        return;
    }
    
    n->exp.opp = operator_get_precedence(n->exp.op);
    // 操作数弹栈
    node *n1 = vector_back(values), *n2 = NULL;
    vector_pop(values);
    if (n->exp.opp->ec > 1) {
        n2 = vector_back(values);
        vector_pop(values);
    }
    // 运算符弹栈
    node *np = vector_back(ops);    
    np->exp.opp = operator_get_precedence(np->exp.op);
    vector_pop(ops);
    // 构造结果入栈
    node *nn = node_create_expression_tree(n1, n2, np);
    vector_push(values, nn);
}

static void parse_expression_node(struct vector *ops, struct vector *values, node *n)
{
    // 操作数直接入栈
    if (n->value.ttype != TOKEN_TYPE_OPERATOR && n->value.ttype != TOKEN_TYPE_SYMBOL) {
        vector_push(values, n);
        return;
    }

    // 栈为空或者遇到左括号则直接入栈
    if (vector_empty(ops) || (n->exp.symbol == '(' && n->value.ttype == TOKEN_TYPE_SYMBOL)) {
        vector_push(ops, n);
        return;
    }

    // 遇到右括号直接出栈直到遇到左括号
    if (n->exp.symbol == ')' && n->value.ttype == TOKEN_TYPE_SYMBOL) {
        node *nt = NULL;
        while(true) {
            nt = vector_back(ops);
            if (nt->value.ttype == TOKEN_TYPE_SYMBOL && nt->exp.symbol == '(') {
                vector_pop(ops);
                return;
            }
            // 弹栈组装树结构
            parse_expression_node_pop(ops, values, nt);
        }
    }

    // 比较优先级再决定是否入栈
    node *np = vector_back(ops);
    if (np->value.ttype == TOKEN_TYPE_SYMBOL && np->exp.symbol == '(') {
        vector_push(ops, n);
        return;
    }

    if (operator_is_prioritized(n->exp.op, np->exp.op)) {
        vector_push(ops, n);
    } else {
        // 弹栈组装树结构
        parse_expression_node_pop(ops, values, n);
        // 再次尝试运算符入栈
        parse_expression_node(ops, values, n);
    }
}

int parse_expression(struct vector *expression)
{
    struct vector *ops = vector_create(sizeof(node));
    struct vector *values = vector_create(sizeof(node));
    int count = vector_count(expression);
    node *n = NULL;
    for (int i = 0; i < count; i++) {
        n = vector_peek_at(expression, i);
        if (n && n->type != NODE_TYPE_DELIMITER)
            parse_expression_node(ops, values, n);
    }
    // 清空运算符栈
    n = vector_back(ops);
    while (vector_count(ops) > 0) {
        parse_expression_node_pop(ops, values, n);
    }
    n = vector_peek(values);
    vector_pop(values);
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
        
        if (exp) {
            node_create_expression(process, elem);
        }
        elem ++;
    };

    // parse expression
    if (parse_expression(process->compiler->ast.expressions) != JAVAC_PARSE_OK)
        return JAVAC_PARSE_ERROR;

    return JAVAC_PARSE_OK;
}