#include "core.h"
#include "../stat/stat.h"

/* =========================================================
 * errors.c -- Definitions des fonctions de gestion d'erreurs
 * ========================================================= */

const char *error_message(CalcError err) {
    switch (err) {
        case ERR_NONE:      return "OK";
        case ERR_SYNTAX:    return "Syntax ERROR";
        case ERR_MATH:      return "Math ERROR";
        case ERR_STACK:     return "Stack ERROR";
        case ERR_ARGUMENT:  return "Argument ERROR";
        case ERR_MEMORY:    return "Memory ERROR";
        case ERR_DIV_ZERO:  return "Math ERROR: division par zero";
        case ERR_DOMAIN:    return "Math ERROR: domaine invalide";
        case ERR_OVERFLOW:  return "Math ERROR: depassement de plage";
        default:            return "Erreur inconnue";
    }
}

void error_print(CalcError err, const char *expr, int position) {
    fprintf(stderr, "\n");
    if (expr) fprintf(stderr, "  %s\n", expr);
    if (position >= 0 && expr) {
        int i;
        fprintf(stderr, "  ");
        for (i = 0; i < position; i++) fprintf(stderr, " ");
        fprintf(stderr, "^\n");
    }
    fprintf(stderr, "  [%s]\n\n", error_message(err));
}

/* =========================================================
 * tokenizer.c -- Decoupage de l'expression en tokens
 * ========================================================= */

#define PI 3.14159265358979323846
#define E  2.71828182845904523536

static const char *KNOWN_FUNCTIONS[] = {
    "sin", "cos", "tan", "cot", "sec", "csc",
    "asin", "acos", "atan", "acot", "asec", "acsc",
    "sinh", "cosh", "tanh", "coth", "sech", "csch",
    "asinh", "acosh", "atanh", "acoth", "asech", "acsch",
    "log", "ln", "exp", "sqrt", "cbrt", "abs",
    "floor", "ceil", "round", "fact", "sqr", "cub",
    "pol_to_rec", "rec_to_pol",
    "arg", "conj", "re", "im", "mod",  /* fonctions complexes */
    "nPr", "nCr", "gcd", "lcm", "modulo",
    "rand", "randi", "seed", "flip", "perm", "comb",
    "bin", "oct", "dec", "hex",
    "sigma", "prod", "integ", "deriv",
    "switch", "reset", "clr", "deg", "rad",
    "norm", "P", "Q", "R", /* fonctions statistiques continues */
    NULL
};

static const char *KNOWN_VARIABLES[] = {
    "Ans", "A", "B", "C", "D", "E", "F", "M", "X", "Y",
    "mean", "std", "samp_std", "var", "n", "sum", "min", "max",
    "regA", "regB", "regC",
    NULL
};

static int is_function(const char *name) {
    int i;
    for (i = 0; KNOWN_FUNCTIONS[i]; i++)
        if (strcmp(name, KNOWN_FUNCTIONS[i]) == 0) return 1;
    return 0;
}

static int is_variable(const char *name) {
    int i;
    for (i = 0; KNOWN_VARIABLES[i]; i++)
        if (strcmp(name, KNOWN_VARIABLES[i]) == 0) return 1;
    return 0;
}

static int insert_implicit_mul(Token *out, int *n, int pos) {
    if (*n >= MAX_TOKENS - 1) return 0;
    memset(&out[*n], 0, sizeof(Token));
    out[*n].type     = TOK_MUL;
    out[*n].position = pos;
    (*n)++;
    return 1;
}

static int needs_implicit_mul(Token *out, int n) {
    if (n == 0) return 0;
    TokenType prev = out[n - 1].type;
    /* FIX 7: TOK_IMAG added so e.g. "2i(3)" gets implicit mul */
    return (prev == TOK_NUMBER   ||
            prev == TOK_RPAREN   ||
            prev == TOK_VARIABLE ||
            prev == TOK_IMAG);
}

CalcError tokenize(const char *expr, Token *out, int *count, int *err_pos) {
    int i = 0, n = 0;
    int len = (int)strlen(expr);
    *err_pos = -1;
    *count   = 0;

    if (len > 99) { *err_pos = 99; return ERR_SYNTAX; }

    while (i < len) {
        if (n >= MAX_TOKENS - 1) { *err_pos = i; return ERR_SYNTAX; }

        Token t;
        memset(&t, 0, sizeof(t));
        t.position = i;

        if (isspace((unsigned char)expr[i])) { i++; continue; }

        /* Gestion des caractères Unicode pour ² et ³ (UTF-8) */
        if ((unsigned char)expr[i] == 0xC2 && i + 1 < len) {
            if ((unsigned char)expr[i+1] == 0xB2) {
                t.type = TOK_SQUARE;
                t.position = i;
                out[n++] = t;
                i += 2;
                continue;
            }
            if ((unsigned char)expr[i+1] == 0xB3) {
                t.type = TOK_CUBE;
                t.position = i;
                out[n++] = t;
                i += 2;
                continue;
            }
        }

        if (expr[i] == 'p' && i + 1 < len && expr[i+1] == 'i') {
            if (needs_implicit_mul(out, n))
                if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            t.type  = TOK_NUMBER;
            t.value = PI;
            i += 2;
            out[n++] = t;
            continue;
        }

        /* FIX 4: 'i' seul = unite imaginaire TOK_IMAG, pas TOK_NUMBER=1.0 */
        if (expr[i] == 'i' && (i + 1 >= len || !isalnum((unsigned char)expr[i+1]))) {
            if (needs_implicit_mul(out, n))
                if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            t.type = TOK_IMAG;   /* unite imaginaire pure */
            i++;
            out[n++] = t;
            continue;
        }

        if (isdigit((unsigned char)expr[i]) || expr[i] == '.') {
            char *end;
            t.type  = TOK_NUMBER;
            t.value = strtod(&expr[i], &end);
            i = (int)(end - expr);
            out[n++] = t;
            if (i < len) {
                int is_alpha = isalpha((unsigned char)expr[i]);
                int is_lpar  = expr[i] == '(';
                if ((is_alpha || is_lpar) && n < MAX_TOKENS - 1)
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            }
            continue;
        }

        if (isalpha((unsigned char)expr[i])) {
            int j = 0;
            while (i < len && (isalnum((unsigned char)expr[i])))
                t.name[j++] = expr[i++];
            t.name[j] = '\0';

            if (is_function(t.name)) {
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, t.position)) { *err_pos = t.position; return ERR_SYNTAX; }
                t.type = TOK_FUNCTION;
            } else if (is_variable(t.name)) {
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, t.position)) { *err_pos = t.position; return ERR_SYNTAX; }
                t.type = TOK_VARIABLE;
                if (i < len && expr[i] == '(')
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            } else if (strcmp(t.name, "and")  == 0) { t.type = TOK_AND;
            } else if (strcmp(t.name, "or")   == 0) { t.type = TOK_OR;
            } else if (strcmp(t.name, "xor")  == 0) { t.type = TOK_XOR;
            } else if (strcmp(t.name, "xnor") == 0) { t.type = TOK_XNOR;
            } else if (strcmp(t.name, "nPr")  == 0) { t.type = TOK_NPR;
            } else if (strcmp(t.name, "nCr")  == 0) { t.type = TOK_NCR;
            } else {
                *err_pos = t.position;
                return ERR_SYNTAX;
            }
            out[n++] = t;
            continue;
        }

        switch (expr[i]) {
            case '+': t.type = TOK_PLUS;   break;
            case '*': t.type = TOK_MUL;    break;
            case '/': t.type = TOK_DIV;    break;
            case '^': t.type = TOK_POW;    break;
            case '%': t.type = TOK_MOD;    break;
            case ',': t.type = TOK_COMMA;  break;
            case '=': t.type = TOK_ASSIGN; break;
            case '-':
                if (n == 0                     ||
                    out[n-1].type == TOK_PLUS   ||
                    out[n-1].type == TOK_MINUS  ||
                    out[n-1].type == TOK_MUL    ||
                    out[n-1].type == TOK_DIV    ||
                    out[n-1].type == TOK_POW    ||
                    out[n-1].type == TOK_MOD    ||
                    out[n-1].type == TOK_LPAREN) {
                    Token zero;
                    memset(&zero, 0, sizeof(zero));
                    zero.type = TOK_NUMBER; zero.value = 0.0; zero.position = i;
                    out[n++] = zero;
                }
                t.type = TOK_MINUS;
                break;
            case '(':
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
                t.type = TOK_LPAREN;
                break;
            case ')': t.type = TOK_RPAREN; break;
            case '!': t.type = TOK_FACT; break;
            default:
                *err_pos = i;
                return ERR_SYNTAX;
        }

        i++;
        out[n++] = t;
    }

    memset(&out[n], 0, sizeof(Token));
    out[n].type     = TOK_END;
    out[n].position = len;
    n++;
    *count = n;
    return ERR_NONE;
}

/* =========================================================
 * parser.c -- Analyseur syntaxique + constructeurs AST
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

static Token *current(Parser *p)  { return &p->tokens[p->pos]; }

static Token *advance(Parser *p) {
    Token *t = &p->tokens[p->pos];
    if (p->pos < p->count - 1) p->pos++;
    return t;
}

static int check(Parser *p, TokenType type) { return current(p)->type == type; }

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

static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_logical   (Parser *p);
static ASTNode *parse_term      (Parser *p);
static ASTNode *parse_power     (Parser *p);
static ASTNode *parse_unary     (Parser *p);
static ASTNode *parse_primary   (Parser *p);

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

static ASTNode *parse_npr_ncr(Parser *p);

static ASTNode *parse_term(Parser *p) {
    ASTNode *node = parse_npr_ncr(p);
    if (!node || p->error != ERR_NONE) return node;
    while (check(p, TOK_MUL) || check(p, TOK_DIV) || check(p, TOK_MOD)) {
        Token *op  = advance(p);
        char   opc = (op->type == TOK_MUL) ? '*'
                   : (op->type == TOK_DIV) ? '/' : '%';
        int    pos = op->position;
        ASTNode *right = parse_npr_ncr(p);
        if (!right || p->error != ERR_NONE) { ast_free(node); return NULL; }
        node = ast_make_binop(opc, node, right, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
    }
    return node;
}

static ASTNode *parse_npr_ncr(Parser *p) {
    ASTNode *node = parse_power(p);
    if (!node || p->error != ERR_NONE) return node;
    while (check(p, TOK_NPR) || check(p, TOK_NCR)) {
        Token *op = advance(p);
        int    pos = op->position;
        const char *opname = (op->type == TOK_NPR) ? "nPr" : "nCr";
        ASTNode *right = parse_power(p);
        if (!right || p->error != ERR_NONE) { ast_free(node); return NULL; }
        ASTNode *binop = ast_make_binop('n', node, right, pos);
        if (!binop) { set_error(p, ERR_MEMORY); return NULL; }
        strncpy(binop->name, opname, sizeof(binop->name) - 1);
        node = binop;
    }
    return node;
}

static ASTNode *parse_postfix(Parser *p);

static ASTNode *parse_power(Parser *p) {
    ASTNode *base = parse_postfix(p);
    if (!base || p->error != ERR_NONE) return base;
    if (check(p, TOK_POW)) {
        Token *op  = advance(p);
        int    pos = op->position;
        ASTNode *exp = parse_power(p);
        if (!exp || p->error != ERR_NONE) { ast_free(base); return NULL; }
        ASTNode *node = ast_make_binop('^', base, exp, pos);
        if (!node) { set_error(p, ERR_MEMORY); return NULL; }
        return node;
    }
    return base;
}

static ASTNode *parse_postfix(Parser *p) {
    ASTNode *node = parse_unary(p);
    if (!node || p->error != ERR_NONE) return node;

    while (check(p, TOK_SQUARE) || check(p, TOK_CUBE) || check(p, TOK_FACT)) {
        Token *op = advance(p);
        int    pos = op->position;
        char   postfix_op[8];

        switch (op->type) {
            case TOK_SQUARE: strncpy(postfix_op, "sqr",  sizeof(postfix_op)); break;
            case TOK_CUBE:   strncpy(postfix_op, "cub",  sizeof(postfix_op)); break;
            case TOK_FACT:   strncpy(postfix_op, "fact", sizeof(postfix_op)); break;
            default:         strncpy(postfix_op, "",     sizeof(postfix_op)); break;
        }

        ASTNode *arg = node;
        ASTNode **args = malloc(sizeof(ASTNode *));
        if (!args) { set_error(p, ERR_MEMORY); ast_free(node); return NULL; }
        args[0] = arg;

        ASTNode *new_node = ast_make_function(postfix_op, args, 1, pos);
        if (!new_node) {
            set_error(p, ERR_MEMORY);
            free(args);
            ast_free(node);
            return NULL;
        }
        node = new_node;
    }
    return node;
}

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

static ASTNode *parse_primary(Parser *p) {
    Token *tok = current(p);
    int    i;

    if (tok->type == TOK_NUMBER) {
        advance(p);
        ASTNode *n = ast_make_number(tok->value, tok->position);
        if (!n) { set_error(p, ERR_MEMORY); return NULL; }
        return n;
    }

    /* FIX 4: TOK_IMAG → noeud NODE_IMAG (imaginaire pur = 0+1i) */
    if (tok->type == TOK_IMAG) {
        advance(p);
        ASTNode *n = ast_alloc();
        if (!n) { set_error(p, ERR_MEMORY); return NULL; }
        n->type     = NODE_IMAG;
        n->value    = 1.0;
        n->position = tok->position;
        return n;
    }

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

    if (tok->type == TOK_FUNCTION) {
        char fname[32];
        int  fpos = tok->position;
        strncpy(fname, tok->name, sizeof(fname) - 1);
        advance(p);
        if (!expect(p, TOK_LPAREN)) return NULL;
        ASTNode *args[16];
        int      argc = 0;
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

    if (tok->type == TOK_LPAREN) {
        advance(p);
        ASTNode *node = parse_expression(p);
        if (!node || p->error != ERR_NONE) return NULL;
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

/* =========================================================
 * evaluator.c -- Evaluation de l'AST
 * ========================================================= */

#undef PI
#define PI  3.14159265358979323846
#define E_  2.71828182845904523536

#define CASIO_MAX       9.999999999e99
#define SINH_MAX        230.2585092
#define EXP_MAX         230.2585092
#define LOG10_MAX_EXP   99.99999999

#define SIN_COS_DEG_MAX 9e9
#define SIN_COS_RAD_MAX 157079632.7

static double to_rad  (double x, int deg) { return deg ? x * PI / 180.0 : x; }
static double from_rad(double x, int deg) { return deg ? x * 180.0 / PI : x; }
static int    is_invalid(double x)        { return isinf(x) || isnan(x); }

#define MAX_EVAL_DEPTH 200

static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             ComplexValue *result, int depth);

void eval_memory_init(CalcMemory *mem) {
    memset(mem->vars, 0, sizeof(mem->vars));
    mem->mem_M        = cx_make(0.0, 0.0);
    mem->ans          = cx_make(0.0, 0.0);
    mem->angle_deg    = 1;
    mem->complex_mode = 0;
    mem->current_mode = MODE_COMP;
}

/* FIX 5: eval_function prend double* args et retourne ComplexValue*
 * Toutes les fonctions utilisent cx_make() pour construire le resultat.
 * FIX 1: dead code apres return supprime dans cos.
 * FIX 2: toutes les assignations *result = double remplacees par cx_make(). */
static CalcError eval_function(const char *name, double *args, int argc,
                               CalcMemory *mem, ComplexValue *result) {
    if (strcmp(name, "sin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        double limit = mem->angle_deg ? SIN_COS_DEG_MAX : SIN_COS_RAD_MAX;
        if (fabs(args[0]) >= limit) return ERR_MATH;
        *result = cx_make(sin(to_rad(args[0], mem->angle_deg)), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "cos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        double limit = mem->angle_deg ? SIN_COS_DEG_MAX : SIN_COS_RAD_MAX;
        if (fabs(args[0]) >= limit) return ERR_MATH;
        /* FIX 1: dead code supprime — une seule ligne de retour */
        *result = cx_make(cos(to_rad(args[0], mem->angle_deg)), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "tan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (mem->angle_deg) {
            double mod = fmod(fabs(args[0]), 180.0);
            if (fabs(mod - 90.0) < 1e-10) return ERR_DOMAIN;
        }
        /* FIX 2: cx_make() au lieu d'assigner double directement */
        *result = cx_make(tan(to_rad(args[0], mem->angle_deg)), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "asin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = cx_make(from_rad(asin(args[0]), mem->angle_deg), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "acos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = cx_make(from_rad(acos(args[0]), mem->angle_deg), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "atan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(from_rad(atan(args[0]), mem->angle_deg), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "sinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = cx_make(sinh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "cosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = cx_make(cosh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "tanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(tanh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "asinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(asinh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "acosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 1.0) return ERR_DOMAIN;
        *result = cx_make(acosh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "atanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) >= 1.0) return ERR_DOMAIN;
        *result = cx_make(atanh(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "exp") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] > EXP_MAX) return ERR_OVERFLOW;
        *result = cx_make(exp(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "ln") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] <= 0.0) return ERR_DOMAIN;
        *result = cx_make(log(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "log") == 0) {
        if (argc == 1) {
            if (args[0] <= 0.0) return ERR_DOMAIN;
            *result = cx_make(log10(args[0]), 0.0);
            return ERR_NONE;
        }
        if (argc == 2) {
            if (args[0] <= 0.0 || args[0] == 1.0) return ERR_ARGUMENT;
            if (args[1] <= 0.0) return ERR_DOMAIN;
            *result = cx_make(log(args[1]) / log(args[0]), 0.0);
            return ERR_NONE;
        }
        return ERR_ARGUMENT;
    }
    if (strcmp(name, "sqrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 0.0) return ERR_DOMAIN;
        *result = cx_make(sqrt(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "cbrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(cbrt(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "abs") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(fabs(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "floor") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(floor(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "ceil") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(ceil(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "round") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(round(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "Pol") == 0) {
        double x, y, r, theta;
        if (argc != 2) return ERR_ARGUMENT;
        x = args[0]; y = args[1];
        r     = sqrt(x * x + y * y);
        theta = from_rad(atan2(y, x), mem->angle_deg);
        printf("  r = %g, theta = %g\n", r, theta);
        *result = cx_make(r, 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "Rec") == 0) {
        double r, theta_rad, xv, yv;
        if (argc != 2) return ERR_ARGUMENT;
        r         = args[0];
        theta_rad = to_rad(args[1], mem->angle_deg);
        xv = r * cos(theta_rad);
        yv = r * sin(theta_rad);
        printf("  x = %g, y = %g\n", xv, yv);
        *result = cx_make(xv, 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "nPr") == 0) {
        if (argc != 2) return ERR_ARGUMENT;
        double n = args[0], r = args[1];
        if (n < 0 || r < 0 || r > n || n >= 1e10) return ERR_ARGUMENT;
        if (n != floor(n) || r != floor(r)) return ERR_ARGUMENT;
        int ni = (int)n, ri = (int)r;
        double result_val = 1.0;
        for (int k = 0; k < ri; k++) result_val *= (ni - k);
        *result = cx_make(result_val, 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "nCr") == 0) {
        if (argc != 2) return ERR_ARGUMENT;
        double n = args[0], r = args[1];
        if (n < 0 || r < 0 || r > n || n >= 1e10) return ERR_ARGUMENT;
        if (n != floor(n) || r != floor(r)) return ERR_ARGUMENT;
        int ni = (int)n, ri = (int)r;
        if (ri > ni - ri) ri = ni - ri;
        double result_val = 1.0;
        for (int k = 0; k < ri; k++) result_val = result_val * (ni - k) / (k + 1);
        *result = cx_make(result_val, 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "sqr") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) >= 1e50) return ERR_OVERFLOW;
        *result = cx_make(args[0] * args[0], 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "cub") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(args[0] * args[0] * args[0], 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "fact") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 0.0 || args[0] != floor(args[0])) return ERR_DOMAIN;
        if (args[0] > 69.0) return ERR_MATH;
        int n = (int)args[0];
        double fact = 1.0;
        for (int k = 2; k <= n; k++) fact *= (double)k;
        *result = cx_make(fact, 0.0);
        return ERR_NONE;
    }
    /* Fonctions spécifiques aux nombres complexes */
    if (strcmp(name, "arg") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(cx_arg(cx_make(args[0], 0.0)), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "conj") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_conj(cx_make(args[0], 0.0));
        return ERR_NONE;
    }
    if (strcmp(name, "re") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(args[0], 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "im") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(0.0, 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "mod") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(cx_mod(cx_make(args[0], 0.0)), 0.0);
        return ERR_NONE;
    }
    /* Fonctions statistiques - utilisent les donnees stockees via stat_push */
    if (strcmp(name, "mean") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_mean(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "std") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_stddev_pop(), 0.0);  /* ecart-type population par defaut */
        return ERR_NONE;
    }
    if (strcmp(name, "var") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_var(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "n") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make((double)stat_count(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "sum") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_sum(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "min") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_min(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "max") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_max(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "samp_std") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_stddev_samp(), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "norm") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(normalize(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "P") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(normal_P(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "Q") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(normal_Q(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "R") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cx_make(normal_R(args[0]), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "regA") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_reg_A(stat_get_regression_type()), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "regB") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_reg_B(stat_get_regression_type()), 0.0);
        return ERR_NONE;
    }
    if (strcmp(name, "regC") == 0) {
        if (argc != 0) return ERR_ARGUMENT;
        *result = cx_make(stat_reg_C(stat_get_regression_type()), 0.0);
        return ERR_NONE;
    }
    return ERR_ARGUMENT;
}

CalcError eval(const ASTNode *node, CalcMemory *mem, ComplexValue *result) {
    return eval_depth(node, mem, result, 0);
}

static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             ComplexValue *result, int depth) {
    CalcError    err;
    ComplexValue lv, rv;
    int          i;

    if (depth > MAX_EVAL_DEPTH) return ERR_STACK;
    if (!node) return ERR_SYNTAX;

    switch (node->type) {

        /* FIX 3: NODE_NUMBER assigne via cx_make, pas directement double */
        case NODE_NUMBER:
            *result = cx_make(node->value, 0.0);
            return ERR_NONE;

        /* FIX 4: NODE_IMAG = 0 + 1i */
        case NODE_IMAG:
            *result = cx_make(0.0, node->value);
            return ERR_NONE;

        case NODE_VARIABLE:
            if (strcmp(node->name, "Ans") == 0) {
                *result = mem->ans;
            } else if (strcmp(node->name, "M") == 0) {
                *result = mem->mem_M;
            } else if (strcmp(node->name, "mean") == 0) {
                *result = cx_make(stat_mean(), 0);
            } else if (strcmp(node->name, "std") == 0) {
                *result = cx_make(stat_stddev_pop(), 0);
            } else if (strcmp(node->name, "samp_std") == 0) {
                *result = cx_make(stat_stddev_samp(), 0);
            } else if (strcmp(node->name, "var") == 0) {
                *result = cx_make(stat_var(), 0);
            } else if (strcmp(node->name, "n") == 0) {
                *result = cx_make(stat_count(), 0);
            } else if (strcmp(node->name, "sum") == 0) {
                *result = cx_make(stat_sum(), 0);
            } else if (strcmp(node->name, "min") == 0) {
                *result = cx_make(stat_min(), 0);
            } else if (strcmp(node->name, "max") == 0) {
                *result = cx_make(stat_max(), 0);
            } else if (strcmp(node->name, "regA") == 0) {
                *result = cx_make(stat_reg_A(stat_get_regression_type()), 0);
            } else if (strcmp(node->name, "regB") == 0) {
                *result = cx_make(stat_reg_B(stat_get_regression_type()), 0);
            } else if (strcmp(node->name, "regC") == 0) {
                *result = cx_make(stat_reg_C(stat_get_regression_type()), 0);
            } else if (strlen(node->name) == 1 &&
                       node->name[0] >= 'A' && node->name[0] <= 'Z') {
                *result = mem->vars[node->name[0] - 'A'];
            } else {
                return ERR_ARGUMENT;
            }
            return ERR_NONE;

        case NODE_ASSIGN:
            err = eval_depth(node->left, mem, result, depth + 1);
            if (err != ERR_NONE) return err;
            if (strcmp(node->name, "M") == 0) {
                mem->mem_M = *result;
            } else if (strlen(node->name) == 1 &&
                       node->name[0] >= 'A' && node->name[0] <= 'Z') {
                mem->vars[node->name[0] - 'A'] = *result;
            } else {
                return ERR_ARGUMENT;
            }
            return ERR_NONE;

        case NODE_UNARYOP:
            err = eval_depth(node->left, mem, &lv, depth + 1);
            if (err != ERR_NONE) return err;
            if (node->name[0] == '-') *result = cx_make(-lv.re, -lv.im);
            else return ERR_SYNTAX;
            return ERR_NONE;

        case NODE_BINOP:
            err = eval_depth(node->left, mem, &lv, depth + 1);
            if (err != ERR_NONE) return err;
            err = eval_depth(node->right, mem, &rv, depth + 1);
            if (err != ERR_NONE) return err;

            if (strcmp(node->name, "and") == 0) {
                if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                *result = cx_make((double)((long long)lv.re & (long long)rv.re), 0.0);
                return ERR_NONE;
            }
            if (strcmp(node->name, "or") == 0) {
                if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                *result = cx_make((double)((long long)lv.re | (long long)rv.re), 0.0);
                return ERR_NONE;
            }
            if (strcmp(node->name, "xor") == 0) {
                if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                *result = cx_make((double)((long long)lv.re ^ (long long)rv.re), 0.0);
                return ERR_NONE;
            }
            if (strcmp(node->name, "xnor") == 0) {
                if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                *result = cx_make((double)(~((long long)lv.re ^ (long long)rv.re)), 0.0);
                return ERR_NONE;
            }
            if (strcmp(node->name, "nPr") == 0 || strcmp(node->name, "nCr") == 0) {
                if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                double fargs[2] = {lv.re, rv.re};
                err = eval_function(node->name, fargs, 2, mem, result);
                return err;
            }

            switch (node->name[0]) {
                case '+': *result = cx_add(lv, rv); break;
                case '-': *result = cx_sub(lv, rv); break;
                case '*': *result = cx_mul(lv, rv); break;
                case '/':
                    if (rv.re == 0.0 && rv.im == 0.0) return ERR_DIV_ZERO;
                    *result = cx_div(lv, rv);
                    break;
                case '^':
                    if (cx_is_real(lv) && cx_is_real(rv)) {
                        if (lv.re < 0.0 && rv.re != (long long)rv.re) return ERR_DOMAIN;
                        *result = cx_make(pow(lv.re, rv.re), 0.0);
                    } else {
                        /* Puissance complexe generale : z^w = exp(w * ln(z)) */
                        double r   = sqrt(lv.re*lv.re + lv.im*lv.im);
                        double th  = atan2(lv.im, lv.re);
                        double lnr = log(r);
                        /* w * ln(z) = (a+bi)(lnr + th*i)
                                     = a*lnr - b*th + (a*th + b*lnr)i */
                        double re_exp = rv.re*lnr - rv.im*th;
                        double im_exp = rv.re*th  + rv.im*lnr;
                        double mag    = exp(re_exp);
                        *result = cx_make(mag*cos(im_exp), mag*sin(im_exp));
                    }
                    break;
                case '%':
                    if (!cx_is_real(lv) || !cx_is_real(rv)) return ERR_DOMAIN;
                    if (rv.re == 0.0) return ERR_DIV_ZERO;
                    *result = cx_make(fmod(lv.re, rv.re), 0.0);
                    break;
                default:
                    return ERR_SYNTAX;
            }
            if (is_invalid(result->re) || is_invalid(result->im)) return ERR_OVERFLOW;
            return ERR_NONE;

        case NODE_FUNCTION: {
            if (node->argc > 16) return ERR_ARGUMENT;
            ComplexValue cargs[16];
            for (i = 0; i < node->argc; i++) {
                err = eval_depth(node->args[i], mem, &cargs[i], depth + 1);
                if (err != ERR_NONE) return err;
            }

            /* Fonctions qui acceptent des arguments complexes */
            int is_complex_func = (strcmp(node->name, "arg") == 0 ||
                                   strcmp(node->name, "conj") == 0 ||
                                   strcmp(node->name, "re") == 0 ||
                                   strcmp(node->name, "im") == 0 ||
                                   strcmp(node->name, "mod") == 0);

            if (is_complex_func && node->argc == 1) {
                /* Ces fonctions operent sur des nombres complexes */
                if (strcmp(node->name, "arg") == 0) {
                    /* arg(0) est indefini - Math ERROR */
                    if (cargs[0].re == 0.0 && cargs[0].im == 0.0)
                        return ERR_MATH;
                    *result = cx_make(cx_arg(cargs[0]), 0.0);
                    return ERR_NONE;
                }
                if (strcmp(node->name, "conj") == 0) {
                    *result = cx_conj(cargs[0]);
                    return ERR_NONE;
                }
                if (strcmp(node->name, "re") == 0) {
                    *result = cx_make(cargs[0].re, 0.0);
                    return ERR_NONE;
                }
                if (strcmp(node->name, "im") == 0) {
                    *result = cx_make(cargs[0].im, 0.0);
                    return ERR_NONE;
                }
                if (strcmp(node->name, "mod") == 0) {
                    *result = cx_make(cx_mod(cargs[0]), 0.0);
                    return ERR_NONE;
                }
            }

            /* FIX 5: convertir ComplexValue → double[] pour eval_function */
            double real_args[16];
            int    all_real = 1;
            for (i = 0; i < node->argc; i++) {
                real_args[i] = cargs[i].re;
                if (!cx_is_real(cargs[i])) all_real = 0;
            }
            if (!all_real) return ERR_DOMAIN;  /* fonctions reelles seulement */
            err = eval_function(node->name, real_args, node->argc, mem, result);
            if (err != ERR_NONE) return err;
            if (is_invalid(result->re) || is_invalid(result->im)) return ERR_MATH;
            return ERR_NONE;
        }

        default:
            return ERR_SYNTAX;
    }
}

void eval_print_result(ComplexValue result, int complex_mode) {
    if (fabs(result.re) < 1e-10) result.re = 0.0;
    if (fabs(result.im) < 1e-10) result.im = 0.0;

    if (!complex_mode || result.im == 0.0) {
        /* Nombre reel pur */
        if (result.re == (long long)result.re &&
            result.re >= -1e15 && result.re <= 1e15) {
            printf("  = %.0f\n", result.re);
        } else {
            printf("  = %g\n", result.re);
        }
    } else if (result.re == 0.0) {
        /* Imaginaire pur */
        if (result.im == 1.0)       printf("  = i\n");
        else if (result.im == -1.0) printf("  = -i\n");
        else                        printf("  = %gi\n", result.im);
    } else {
        /* Complexe general */
        if (result.im > 0.0)
            printf("  = %g+%gi\n", result.re, result.im);
        else
            printf("  = %g%gi\n",  result.re, result.im);
    }
}

/* =========================================================
 *  Gestion des modes de la calculatrice
 * ========================================================= */

/* Fonctions disponibles par mode */
static const char *COMPLEX_FUNCTIONS[] = {
    "arg", "conj", "re", "im", "mod",
    NULL
};

static const char *STAT_FUNCTIONS[] = {
    "mean", "std", "var", "n", "sum", "min", "max",
    NULL
};

static const char *MATRIX_FUNCTIONS[] = {
    "det", "tr", "inv", "dim", "trans",
    NULL
};

static const char *BASE_N_FUNCTIONS[] = {
    "and", "or", "xor", "not", "shl", "shr",
    NULL
};

static int is_in_list(const char *name, const char **list) {
    for (int i = 0; list[i]; i++) {
        if (strcmp(name, list[i]) == 0) return 1;
    }
    return 0;
}

int is_function_valid_for_mode(const char *func_name, CalcMode mode) {
    /* Toutes les fonctions mathématiques de base sont disponibles partout */
    const char *BASE_MATH[] = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
        "log", "ln", "exp", "sqrt", "cbrt", "abs",
        "floor", "ceil", "round", "Pol", "Rec",
        "nPr", "nCr", "sqr", "cub", "fact",
        NULL
    };
    
    /* Si c'est une fonction de base, toujours valide */
    if (is_in_list(func_name, BASE_MATH)) return 1;
    
    switch (mode) {
        case MODE_CMPLX:
            return is_in_list(func_name, COMPLEX_FUNCTIONS);
        case MODE_STAT:
            return is_in_list(func_name, STAT_FUNCTIONS);
        case MODE_MATRIX:
            return is_in_list(func_name, MATRIX_FUNCTIONS);
        case MODE_BASE_N:
            return is_in_list(func_name, BASE_N_FUNCTIONS);
        default:
            /* En mode COMP, pas de fonctions spéciales */
            return 0;
    }
}

CalcMode detect_mode_from_expr(const char *expr) {
    /* Détecte automatiquement le mode nécessaire pour une expression */
    
    /* Vérifier la présence de l'unité imaginaire i (pas comme partie d'un mot) */
    const char *p = expr;
    while ((p = strchr(p, 'i')) != NULL) {
        /* Vérifier que ce n'est pas une partie d'un mot (comme sin, cos, etc.) */
        if ((p == expr || !isalnum((unsigned char)*(p-1))) &&
            (!isalnum((unsigned char)*(p+1)))) {
            return MODE_CMPLX;
        }
        p++;
    }
    
    /* Vérifier les fonctions complexes */
    const char **cf = COMPLEX_FUNCTIONS;
    for (int i = 0; cf[i]; i++) {
        char pattern[32];
        snprintf(pattern, sizeof(pattern), "%s(", cf[i]);
        if (strstr(expr, pattern)) return MODE_CMPLX;
    }
    
    /* Vérifier les fonctions statistiques */
    const char **sf = STAT_FUNCTIONS;
    for (int i = 0; sf[i]; i++) {
        char pattern[32];
        snprintf(pattern, sizeof(pattern), "%s(", sf[i]);
        if (strstr(expr, pattern)) return MODE_STAT;
    }
    
    /* Vérifier les fonctions matricielles */
    const char **mf = MATRIX_FUNCTIONS;
    for (int i = 0; mf[i]; i++) {
        char pattern[32];
        snprintf(pattern, sizeof(pattern), "%s(", mf[i]);
        if (strstr(expr, pattern)) return MODE_MATRIX;
    }
    
    /* Par défaut, mode standard */
    return MODE_COMP;
}

const char *mode_name(CalcMode mode) {
    switch (mode) {
        case MODE_COMP:   return "COMP";
        case MODE_CMPLX:  return "CMPLX";
        case MODE_STAT:   return "STAT";
        case MODE_MATRIX: return "MATRIX";
        case MODE_TABLE:  return "TABLE";
        case MODE_BASE_N: return "BASE-N";
        case MODE_EQN:    return "EQN";
        default:          return "UNKNOWN";
    }
}