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

static compile_process *current_process;

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

static bool is_new_begin(token *tk)
{
    return tk->type == TOKEN_TYPE_KEYWORD && SEQ(tk->sval, "new");
}

static void attach_expression_to_declare(struct vector *nodes, bool comma)
{
    // expression value
    node *exp = (node*)vector_back(nodes);
    vector_pop(nodes);
    // delimiter node
    vector_pop(nodes);
    // declare node
    node *dec = (node*)vector_back(nodes);
    if (comma) {
        if (!(dec->type & NODE_TYPE_STATEMENT_DECLARE_LIST)) {
            compile_error(current_process, "错误的声明\n");
        }
        ((node*)vector_back(dec->varlist.list))->var.value = exp;
    } else {
        if (!(dec->type & NODE_TYPE_STATEMENT_DECLARE)) {
            compile_error(current_process, "错误的声明\n");
        }
        dec->var.value = exp;
    }
    vector_set_peek_pointer_end(nodes);
}

static void attach_new_to_declare(struct vector *nodes)
{
    // pop delimiter node
    vector_pop(nodes);
    // pop new node
    node *nn = vector_back(nodes);
    struct vector *array = nn->new.array.values;
    vector_pop(nodes);
    // pop delimiter node
    vector_pop(nodes);
    // set declare node 's value
    node *nd = vector_back(nodes);
    nd->var.value = nn;
}

static void attach_expression_to_array(struct vector *nodes)
{
    int dim = 0;
    // value node
    node *nv = vector_back(nodes);
    vector_pop(nodes);
    // delimiter node
    vector_pop(nodes);
    node *nb = vector_back(nodes);
    if (SEQ(nb->value.val, "[")) {
        dim ++;
        vector_pop(nodes);
    }
    node *ntype = vector_back(nodes);
    vector_pop(nodes);
    node *nn = vector_back(nodes);
    nn->new.array.dim += dim;
    if (!nn->new.array.values) {
        nn->new.array.values = vector_create(sizeof(node));
        nn->new.array.type = ntype;
    }
    vector_push(nn->new.array.values, nv);
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
    node *nv = vector_peek(values);
    vector_push(expression, nv);
    vector_free(ops);
    vector_free(values);
    return JAVAC_PARSE_OK;
}

#define DECLARE_ERROR   compile_error(current_process, "错误的声明格式\n")

int parse_declare_list(struct vector *statement)
{
    // pop new declare identifier
    node *nn = vector_back(statement);
    vector_pop(statement);
    // pop delimiter node
    vector_pop(statement);
    // pop original declare node
    node *nvar = vector_back(statement);
    node *nlist = NULL;
    // create new var list node or get var list node
    if (!(nvar->type & NODE_TYPE_STATEMENT_DECLARE_LIST)) {
        vector_pop(statement);
        nlist = node_create(&(node){
            .type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_DECLARE_LIST,
            .varlist = vector_create(sizeof(node))
        });
        vector_push(nlist->varlist.list, nvar);
    } else {
        nlist = vector_back(statement);
    }
    node nnn;
    nnn.type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_DECLARE;
    nnn.var.name = nn->value.val;
    nnn.var.type = ((node*)vector_peek_no_increment(nlist->varlist.list))->var.type;
    vector_push(nlist->varlist.list, &nnn);
    return JAVAC_PARSE_OK;
}

int parse_daclare(struct vector *statement, bool comma) 
{
    if (comma) {
        return parse_declare_list(statement);
    }

    datatype *dtype = datatype_create();
    node *n = vector_peek_no_increment(statement);
    char *var_name = NULL;
    bool typed = false, identified = false;
    while (n && n->type) {
        if (n->type == NODE_TYPE_DELIMITER) {
            break;
        }
        switch (n->value.ttype) {
            case TOKEN_TYPE_KEYWORD: {
                bool flaged = false;
                if (!typed && !identified) 
                    flaged = datatype_parse_flags(n, dtype);
                if (!flaged) 
                    typed = datatype_parse_type(n, dtype);
                break;
            }
            case TOKEN_TYPE_IDENTIFIER: {
                if (typed) {
                    identified = true;
                    size_t len_name = strlen(n->value.val);
                    var_name = malloc(len_name + 1);
                    strncpy(var_name, n->value.val, len_name);
                } else DECLARE_ERROR;
                break;
            }
            default: {
                if ((typed || identified) && (SEQ(n->value.val, "[") && n->value.ttype == TOKEN_TYPE_SYMBOL)) {
                    vector_peek_pop(statement);
                    node *nn = vector_peek_no_increment(statement);
                    if (SEQ(nn->value.val, "]") && nn->value.ttype == TOKEN_TYPE_SYMBOL) {
                        dtype->array.dim ++;
                    }
                } else DECLARE_ERROR;
                break;
            }
        }
        vector_peek_pop(statement);
        n = vector_peek_no_increment(statement);
    }
    node_create(&(node){ 
        .type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_DECLARE, 
        .var.type = dtype, .var.name = var_name });
    return JAVAC_PARSE_OK;
}

int parse_new(struct vector *vec)
{
    // TODO: parse new node
    return JAVAC_PARSE_OK;
}

int parse(lexer_process *process)
{
    current_process = process->compiler;
    node_set_vector(process->compiler->nodes);
    // 解析toke列表
    bool exp = false, declare = false, new = false, array = false, comma = false;
    token *elem = (token*)vector_peek(process->tokens);
    // 区分Expression和Statement
    while(elem && elem->type) {
        if (is_expression_begin(elem)) {
            elem = vector_peek_no_increment(process->tokens);
            if (is_new_begin(elem)) {
                new = true;
            } else {
                exp = true;
            }
            // parse declare statement
            if (parse_daclare(process->compiler->nodes, comma) != JAVAC_PARSE_OK)
                return JAVAC_PARSE_ERROR;
            node_push_delimiter();
            elem = (token*)vector_peek(process->tokens);
            continue;
        } else if (elem->type == TOKEN_TYPE_SYMBOL && ((elem->cval == ']' && array && new) || elem->cval == ';' || elem->cval == ',')) {
            if (exp) {
                exp = false;
                // parse expression
                if (parse_expression(process->compiler->nodes) != JAVAC_PARSE_OK)
                    return JAVAC_PARSE_ERROR;
                if (declare) {
                    // attach the expression node to the value attribute of declare node
                    attach_expression_to_declare(process->compiler->nodes, comma);
                } else if (new && array) {
                    // attach the expression node to the values attribute of array node
                    attach_expression_to_array(process->compiler->nodes);
                }
            } else if (new) {
                new = false;
                // parse declare statement
                if (parse_new(process->compiler->nodes) != JAVAC_PARSE_OK)
                    return JAVAC_PARSE_ERROR;
                // attach the new node to the value attribute of declare node
                attach_new_to_declare(process->compiler->nodes);
            }
            if (elem->cval == ',') {
                new = array = exp = false;
                declare = true;
                comma = true;
            } else if (elem->cval == ';') {
                comma = false;
            }
            node_push_delimiter();
            elem = (token*)vector_peek(process->tokens);
            continue;
        } else if (is_declare_begin(elem) && !new) {
            declare = true;
        }
        
        if (exp) {
            node_create_expression(process, elem);
        } else if (declare) {
            node_create_declare(process, elem);
        } else if (new) {
            node_create_new(process, elem);
            if (elem->cval == '[' && elem->type == TOKEN_TYPE_SYMBOL) {
                exp = true;
                array = true;
                node_push_delimiter();
            }
        }
        elem = (token*)vector_peek(process->tokens);
    }

    return JAVAC_PARSE_OK;
}