#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/* =========================================================
 *  SECTION 1 : Constructeurs de l'AST
 * ========================================================= */

static ASTNode *ast_alloc(void) { return calloc(1, sizeof(ASTNode)); }

ASTNode *ast_make_number(double value, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_NUMBER; n->value = value; n->position = pos;
    return n;
}

ASTNode *ast_make_variable(const char *name, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_VARIABLE; n->position = pos;
    strncpy(n->name, name, sizeof(n->name) - 1);
    return n;
}

ASTNode *ast_make_binop(char op, ASTNode *left, ASTNode *right, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_BINOP;
    n->name[0] = op; n->name[1] = '\0';
    n->left = left; n->right = right; n->position = pos;
    return n;
}

ASTNode *ast_make_unaryop(char op, ASTNode *operand, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_UNARYOP;
    n->name[0] = op; n->name[1] = '\0';
    n->left = operand; n->position = pos;
    return n;
}

ASTNode *ast_make_function(const char *name, ASTNode **args, int argc, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_FUNCTION; n->argc = argc; n->position = pos;
    strncpy(n->name, name, sizeof(n->name) - 1);
    if (argc > 0) {
        n->args = malloc(argc * sizeof(ASTNode *));
        if (!n->args) { free(n); return NULL; }
        memcpy(n->args, args, argc * sizeof(ASTNode *));
    }
    return n;
}

ASTNode *ast_make_assign(const char *var, ASTNode *expr, int pos) {
    ASTNode *n = ast_alloc(); if (!n) return NULL;
    n->type = NODE_ASSIGN; n->position = pos;
    strncpy(n->name, var, sizeof(n->name) - 1);
    n->left = expr;
    return n;
}

void ast_free(ASTNode *node) {
    int i;
    if (!node) return;
    ast_free(node->left);
    ast_free(node->right);
    if (node->args) {
        for (i = 0; i < node->argc; i++) ast_free(node->args[i]);
        free(node->args);
    }
    free(node);
}

/* =========================================================
 *  SECTION 2 : Utilitaires internes du parser
 * ========================================================= */

static Token *current(Parser *p) {
    return &p->tokens[p->pos];
}

static Token *advance(Parser *p) {
    Token *t = &p->tokens[p->pos];
    if (p->pos < p->count - 1) p->pos++;
    return t;
}

static int check(Parser *p, TokenType type) {
    return current(p)->type == type;
}

static void set_error(Parser *p, CalcError err) {
    if (p->error == ERR_NONE) {
        p->error     = err;
        p->error_pos = current(p)->position;
    }
}

static int expect(Parser *p, TokenType type) {
    if (check(p, type)) { advance(p); return 1; }
    set_error(p, ERR_SYNTAX);
    return 0;
}

/* =========================================================
 *  SECTION 3 : Regles de grammaire (descente recursive)
 * ========================================================= */

static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_logical(Parser *p);
static ASTNode *parse_term(Parser *p);
static ASTNode *parse_power(Parser *p);
static ASTNode *parse_unary(Parser *p);
static ASTNode *parse_primary(Parser *p);

/*
 * expression -> logical (('+' | '-') logical)*
 * Addition et soustraction
 */
static ASTNode *parse_expression(Parser *p) {
    ASTNode *node = parse_logical(p);
    if (!node || p->error != ERR_NONE) return node;

    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        Token *op  = advance(p);
        char   opc = (op->type == TOK_PLUS) ? '+' : '-';
        int    pos = op->position;
        ASTNode *right = parse_logical(p);
        if (!right || p->error != ERR_NONE) { ast_free(node); return NULL; }
        node = ast_make_binop(opc, node, right, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
    }
    return node;
}

/*
 * logical -> term (('and' | 'or' | 'xor' | 'xnor') term)*
 * Operateurs logiques bit-a-bit (F-CO-02) -- priorite inferieure a +/-
 */
static ASTNode *parse_logical(Parser *p) {
    ASTNode *node = parse_term(p);
    if (!node || p->error != ERR_NONE) return node;

    while (check(p, TOK_AND)  || check(p, TOK_OR) ||
           check(p, TOK_XOR)  || check(p, TOK_XNOR)) {
        Token      *op  = advance(p);
        const char *opname;
        char        opc;
        switch (op->type) {
            case TOK_AND:  opc = '&'; opname = "and";  break;
            case TOK_OR:   opc = '|'; opname = "or";   break;
            case TOK_XOR:  opc = 'x'; opname = "xor";  break;
            default:       opc = 'n'; opname = "xnor"; break;
        }
        int pos = op->position;
        ASTNode *right = parse_term(p);
        if (!right || p->error != ERR_NONE) { ast_free(node); return NULL; }
        ASTNode *binop = ast_make_binop(opc, node, right, pos);
        if (!binop) { set_error(p, ERR_MEMORY); return NULL; }
        strncpy(binop->name, opname, sizeof(binop->name) - 1);
        node = binop;
    }
    return node;
}

/*
 * term -> power (('*' | '/' | '%') power)*
 * Multiplication, division, modulo
 */
static ASTNode *parse_term(Parser *p) {
    ASTNode *node = parse_power(p);
    if (!node || p->error != ERR_NONE) return node;

    while (check(p, TOK_MUL) || check(p, TOK_DIV) || check(p, TOK_MOD)) {
        Token *op  = advance(p);
        char   opc = (op->type == TOK_MUL) ? '*'
                   : (op->type == TOK_DIV) ? '/' : '%';
        int    pos = op->position;
        ASTNode *right = parse_power(p);
        if (!right || p->error != ERR_NONE) { ast_free(node); return NULL; }
        node = ast_make_binop(opc, node, right, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
    }
    return node;
}

/*
 * power -> unary ('^' power)*
 * Puissance -- associative a droite : 2^3^2 = 2^(3^2)
 */
static ASTNode *parse_power(Parser *p) {
    ASTNode *base = parse_unary(p);
    if (!base || p->error != ERR_NONE) return base;

    if (check(p, TOK_POW)) {
        Token *op  = advance(p);
        int    pos = op->position;
        ASTNode *exp = parse_power(p);  /* recursion droite */
        if (!exp || p->error != ERR_NONE) { ast_free(base); return NULL; }
        ASTNode *node = ast_make_binop('^', base, exp, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
        return node;
    }
    return base;
}

/*
 * unary -> '-' unary | primary
 * Signe negatif unaire
 */
static ASTNode *parse_unary(Parser *p) {
    if (check(p, TOK_MINUS)) {
        Token *op  = advance(p);
        int    pos = op->position;
        ASTNode *operand = parse_unary(p);
        if (!operand || p->error != ERR_NONE) return NULL;
        ASTNode *node = ast_make_unaryop('-', operand, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
        return node;
    }
    return parse_primary(p);
}

/*
 * primary -> NUMBER | VARIABLE ['=' expression]
 *          | FUNCTION '(' args ')'
 *          | '(' expression ')'
 */
static ASTNode *parse_primary(Parser *p) {
    Token *tok = current(p);
    int    i;

    /* Nombre literal */
    if (tok->type == TOK_NUMBER) {
        advance(p);
        ASTNode *n = ast_make_number(tok->value, tok->position);
        if (!n) { set_error(p, ERR_MEMORY); return NULL; }
        return n;
    }

    /* Variable -- avec affectation possible (F-ME-02) */
    if (tok->type == TOK_VARIABLE) {
        char vname[32];
        int  vpos = tok->position;
        strncpy(vname, tok->name, sizeof(vname) - 1);
        advance(p);

        if (check(p, TOK_ASSIGN)) {
            advance(p);
            ASTNode *expr = parse_expression(p);
            if (!expr || p->error != ERR_NONE) return NULL;
            ASTNode *n = ast_make_assign(vname, expr, vpos);
            if (!n) { set_error(p, ERR_MEMORY); return NULL; }
            return n;
        }
        ASTNode *n = ast_make_variable(vname, vpos);
        if (!n) { set_error(p, ERR_MEMORY); return NULL; }
        return n;
    }

    /* Appel de fonction */
    if (tok->type == TOK_FUNCTION) {
        char fname[32];
        int  fpos = tok->position;
        strncpy(fname, tok->name, sizeof(fname) - 1);
        advance(p);

        if (!expect(p, TOK_LPAREN)) return NULL;

        ASTNode *args[16];
        int      argc = 0;

        /* Lecture des arguments */
        if (!check(p, TOK_RPAREN) && !check(p, TOK_END)) {
            do {
                if (argc >= 16) { set_error(p, ERR_ARGUMENT); return NULL; }
                args[argc] = parse_expression(p);
                if (!args[argc] || p->error != ERR_NONE) {
                    for (i = 0; i < argc; i++) ast_free(args[i]);
                    return NULL;
                }
                argc++;
            } while (check(p, TOK_COMMA) && advance(p));
        }

        /* Parenthese fermante optionnelle en fin d'expression (F-CO-04) */
        if (check(p, TOK_RPAREN)) {
            advance(p);
        } else if (!check(p, TOK_END)) {
            set_error(p, ERR_SYNTAX);
            for (i = 0; i < argc; i++) ast_free(args[i]);
            return NULL;
        }

        ASTNode *node = ast_make_function(fname, args, argc, fpos);
        if (!node) {
            set_error(p, ERR_MEMORY);
            for (i = 0; i < argc; i++) ast_free(args[i]);
            return NULL;
        }
        return node;
    }

    /* Expression entre parentheses */
    if (tok->type == TOK_LPAREN) {
        advance(p);
        ASTNode *node = parse_expression(p);
        if (!node || p->error != ERR_NONE) return NULL;

        /* Parenthese fermante optionnelle (F-CO-04) */
        if (check(p, TOK_RPAREN)) {
            advance(p);
        } else if (!check(p, TOK_END)) {
            set_error(p, ERR_SYNTAX);
            ast_free(node);
            return NULL;
        }
        return node;
    }

    set_error(p, ERR_SYNTAX);
    return NULL;
}

/* =========================================================
 *  SECTION 4 : Interface publique
 * ========================================================= */

void parser_init(Parser *p, Token *tokens, int count, const char *expr) {
    p->tokens    = tokens;
    p->count     = count;
    p->pos       = 0;
    p->error     = ERR_NONE;
    p->error_pos = -1;
    p->expr      = expr;
}

ASTNode *parser_parse(Parser *p) {
    ASTNode *tree = parse_expression(p);
    if (tree && p->error == ERR_NONE && !check(p, TOK_END)) {
        set_error(p, ERR_SYNTAX);
        ast_free(tree);
        return NULL;
    }
    return (p->error == ERR_NONE) ? tree : NULL;
}

void parser_print_error(const Parser *p) {
    error_print(p->error, p->expr, p->error_pos);
}
