#include "parser.h"
#include <stddef.h>

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

static bool is_expression_begin(token *tk)
{
    return tk->type == TOKEN_TYPE_OPERATOR && (
        SEQ(tk->sval, "=") || SEQ(tk->sval, "+=") || SEQ(tk->sval, "-=") || SEQ(tk->sval, "*=") ||
        SEQ(tk->sval, "/=") || SEQ(tk->sval, "%=") || SEQ(tk->sval, ">>=") || SEQ(tk->sval, "<<=") || 
        SEQ(tk->sval, ">>>=") || SEQ(tk->sval, "|=") || SEQ(tk->sval, "&=") || SEQ(tk->sval, "^=")
    );
}

static bool is_declare_begin(token *tk)
{
    return tk->type == TOKEN_TYPE_KEYWORD && (
        SEQ(tk->sval, "public") || SEQ(tk->sval, "private") || SEQ(tk->sval, "protected") || 
        SEQ(tk->sval, "static") || SEQ(tk->sval, "final") ||SEQ(tk->sval, "transient") || SEQ(tk->sval, "volatile") || 
        SEQ(tk->sval, "abstract") || SEQ(tk->sval, "void") || 
        SEQ(tk->sval, "byte") || SEQ(tk->sval, "char") || SEQ(tk->sval, "short") || SEQ(tk->sval, "int") || SEQ(tk->sval, "long") || SEQ(tk->sval, "boolean") || 
        SEQ(tk->sval, "double") || SEQ(tk->sval, "float"));
}

static void attach_expression_to_declare(struct vector *nodes)
{
    // expression value
    node *exp = (node*)vector_back(nodes);
    vector_pop(nodes);
    // delimiter node
    vector_pop(nodes);
    // declare node
    node *dec = (node*)vector_back(nodes);
    vector_pop(nodes);
    dec->var.value = exp;
    vector_push(nodes, dec);
    vector_set_peek_pointer_end(nodes);
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
    char vop[2] = { n->exp.symbol, 0 };
    n->exp.opp = operator_get_precedence(n->value.ttype == TOKEN_TYPE_SYMBOL ? vop : n->exp.op);
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

static void parse_expression_node(struct vector *ops, struct vector *values, node *n);

static void parse_node_push(struct vector *ops, struct vector *values, node *n)
{
    // 比较优先级再决定是否入栈
    node *np = vector_back(ops);
    if (np->value.ttype == TOKEN_TYPE_SYMBOL && np->exp.symbol == '(') {
        vector_push(ops, n);
        return;
    }

    if (n->exp.op && np->exp.op && operator_is_prioritized(n->exp.op, np->exp.op)) {
        vector_push(ops, n);
        return;
    } else if (n->value.ttype == TOKEN_TYPE_SYMBOL && np->value.ttype != TOKEN_TYPE_SYMBOL) {
        char vn[2] = { n->exp.symbol, 0};
        if (operator_is_prioritized(vn, np->exp.op)) {
            vector_push(ops, n);
            return;
        }
    } else if (n->value.ttype != TOKEN_TYPE_SYMBOL && np->value.ttype == TOKEN_TYPE_SYMBOL) {
        char vnp[2] = { np->exp.symbol, 0};
        if (operator_is_prioritized(n->exp.op, vnp)) {
            vector_push(ops, n);
            return;
        }
    } else if (n->value.ttype == TOKEN_TYPE_SYMBOL && np->value.ttype == TOKEN_TYPE_SYMBOL) {
        char vn[2] = { n->exp.symbol, 0};
        char vnp[2] = { np->exp.symbol, 0};
        if (operator_is_prioritized(vn, vnp)) {
            vector_push(ops, n);
            return;
        }
    }

    // 弹栈组装树结构
    parse_expression_node_pop(ops, values, n);
    // 再次尝试运算符入栈
    parse_expression_node(ops, values, n);
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
    parse_node_push(ops, values, n);
}

int parse_expression(struct vector *expression)
{
    struct vector *ops = vector_create(sizeof(node));
    struct vector *values = vector_create(sizeof(node));
    node *n = vector_peek_no_increment(expression);
    while (n && n->type) {
        if (n->type == NODE_TYPE_DELIMITER) {
            vector_peek_pop(expression);
            break;
        }
        parse_expression_node(ops, values, n);
        vector_peek_pop(expression);
        n = vector_peek_no_increment(expression);
    }
    // 清空运算符栈
    if (!vector_empty(ops)) {
        n = vector_back(ops);
        while (vector_count(ops) > 0) {
            parse_expression_node_pop(ops, values, n);
        }
    }
    vector_push(expression, vector_peek(values));
    vector_free(ops);
    vector_free(values);
    return JAVAC_PARSE_OK;
}

int parse_daclare(struct vector *statement) 
{
    datatype *dtype = datatype_create();
    node *n = vector_peek_no_increment(statement);
    char *var_name = NULL;
    while (n && n->type) {
        if (n->type == NODE_TYPE_DELIMITER) {
            break;
        }
        switch (n->value.ttype) {
            case TOKEN_TYPE_KEYWORD:
                datatype_parse_flags(n, dtype) || datatype_parse_type(n, dtype);
                break;
            case TOKEN_TYPE_IDENTIFIER: {
                size_t len_name = strlen(n->value.val);
                var_name = malloc(len_name + 1);
                strncpy(var_name, n->value.val, len_name);
                break;
            }
            default:
                datatype_parse_array(n, dtype);
                break;
        }
        vector_peek_pop(statement);
        n = vector_peek_no_increment(statement);
    }
    node_create(&(node){ 
        .type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_DECLARE, 
        .var.type = dtype, .var.name = var_name });
    return JAVAC_PARSE_OK;
}

int parse(lexer_process *process)
{
    node_set_vector(process->compiler->nodes);
    // 解析toke列表
    bool exp = false, declare = false;
    token *elem = (token*)vector_peek(process->tokens);
    // 区分Expression和Statement
    while(elem && elem->type) {
        if (is_expression_begin(elem)) {
            exp = true;
            declare = false;
            // parse declare statement
            if (parse_daclare(process->compiler->nodes) != JAVAC_PARSE_OK)
                return JAVAC_PARSE_ERROR;
            node_push_delimiter();
            elem ++;
            continue;
        } else if (elem->type == TOKEN_TYPE_SYMBOL && elem->cval == ';' && exp) {
            exp = false;
            // parse expression
            if (parse_expression(process->compiler->nodes) != JAVAC_PARSE_OK)
                return JAVAC_PARSE_ERROR;
            // attach the expression node to the value attribute of declare node
            attach_expression_to_declare(process->compiler->nodes);
            node_push_delimiter();
            elem ++;
            continue;
        } else if (is_declare_begin(elem)) {
            declare = true;
        }
        
        if (exp) {
            node_create_expression(process, elem);
        } else if (declare) {
            node_create_declare(process, elem);
        }
        elem ++;
    }

    return JAVAC_PARSE_OK;
}