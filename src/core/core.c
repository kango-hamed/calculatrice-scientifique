#include "core.h"

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
    "sin", "cos", "tan",
    "asin", "acos", "atan",
    "sinh", "cosh", "tanh",
    "asinh", "acosh", "atanh",
    "log", "ln", "exp",
    "sqrt", "cbrt", "abs",
    "floor", "ceil", "round",
    "Pol", "Rec",
    NULL
};

static const char *KNOWN_VARIABLES[] = {
    "Ans", "A", "B", "C", "D", "E", "F", "M", "X", "Y",
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
    return (prev == TOK_NUMBER   ||
            prev == TOK_RPAREN   ||
            prev == TOK_VARIABLE);
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

        if (expr[i] == 'p' && i + 1 < len && expr[i+1] == 'i') {
            if (needs_implicit_mul(out, n))
                if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            t.type  = TOK_NUMBER;
            t.value = PI;
            i += 2;
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

static ASTNode *parse_power(Parser *p) {
    ASTNode *base = parse_unary(p);
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

static double to_rad  (double x, int deg) { return deg ? x * PI / 180.0 : x; }
static double from_rad(double x, int deg) { return deg ? x * 180.0 / PI : x; }
static int    is_invalid(double x)        { return isinf(x) || isnan(x); }

#define MAX_EVAL_DEPTH 200

static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             double *result, int depth);

void eval_memory_init(CalcMemory *mem) {
    memset(mem->vars, 0, sizeof(mem->vars));
    mem->mem_M     = 0.0;
    mem->ans       = 0.0;
    mem->angle_deg = 1;
}

static CalcError eval_function(const char *name, double *args, int argc,
                               CalcMemory *mem, double *result) {
    if (strcmp(name, "sin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = sin(to_rad(args[0], mem->angle_deg)); return ERR_NONE;
    }
    if (strcmp(name, "cos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cos(to_rad(args[0], mem->angle_deg)); return ERR_NONE;
    }
    if (strcmp(name, "tan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (mem->angle_deg) {
            double mod = fmod(fabs(args[0]), 180.0);
            if (fabs(mod - 90.0) < 1e-10) return ERR_DOMAIN;
        }
        *result = tan(to_rad(args[0], mem->angle_deg)); return ERR_NONE;
    }
    if (strcmp(name, "asin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = from_rad(asin(args[0]), mem->angle_deg); return ERR_NONE;
    }
    if (strcmp(name, "acos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = from_rad(acos(args[0]), mem->angle_deg); return ERR_NONE;
    }
    if (strcmp(name, "atan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = from_rad(atan(args[0]), mem->angle_deg); return ERR_NONE;
    }
    if (strcmp(name, "sinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = sinh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "cosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = cosh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "tanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = tanh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "asinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = asinh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "acosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 1.0) return ERR_DOMAIN;
        *result = acosh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "atanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) >= 1.0) return ERR_DOMAIN;
        *result = atanh(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "exp") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] > EXP_MAX) return ERR_OVERFLOW;
        *result = exp(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "ln") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] <= 0.0) return ERR_DOMAIN;
        *result = log(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "log") == 0) {
        if (argc == 1) {
            if (args[0] <= 0.0) return ERR_DOMAIN;
            *result = log10(args[0]); return ERR_NONE;
        }
        if (argc == 2) {
            if (args[0] <= 0.0 || args[0] == 1.0) return ERR_ARGUMENT;
            if (args[1] <= 0.0) return ERR_DOMAIN;
            *result = log(args[1]) / log(args[0]); return ERR_NONE;
        }
        return ERR_ARGUMENT;
    }
    if (strcmp(name, "sqrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 0.0) return ERR_DOMAIN;
        *result = sqrt(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "cbrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cbrt(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "abs") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = fabs(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "floor") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = floor(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "ceil") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = ceil(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "round") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = round(args[0]); return ERR_NONE;
    }
    if (strcmp(name, "Pol") == 0) {
        double x, y, r, theta;
        if (argc != 2) return ERR_ARGUMENT;
        x = args[0]; y = args[1];
        r     = sqrt(x * x + y * y);
        theta = from_rad(atan2(y, x), mem->angle_deg);
        printf("  r = %g, theta = %g\n", r, theta);
        *result = r; return ERR_NONE;
    }
    if (strcmp(name, "Rec") == 0) {
        double r, theta_rad, xv, yv;
        if (argc != 2) return ERR_ARGUMENT;
        r         = args[0];
        theta_rad = to_rad(args[1], mem->angle_deg);
        xv = r * cos(theta_rad);
        yv = r * sin(theta_rad);
        printf("  x = %g, y = %g\n", xv, yv);
        *result = xv; return ERR_NONE;
    }
    return ERR_ARGUMENT;
}

CalcError eval(const ASTNode *node, CalcMemory *mem, double *result) {
    return eval_depth(node, mem, result, 0);
}

static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             double *result, int depth) {
    CalcError err;
    double    lv, rv;
    double    args[16];
    int       i;

    if (depth > MAX_EVAL_DEPTH) return ERR_STACK;
    if (!node) return ERR_SYNTAX;

    switch (node->type) {

        case NODE_NUMBER:
            *result = node->value;
            return ERR_NONE;

        case NODE_VARIABLE:
            if (strcmp(node->name, "Ans") == 0) {
                *result = mem->ans;
            } else if (strcmp(node->name, "M") == 0) {
                *result = mem->mem_M;
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
            if (node->name[0] == '-') *result = -lv;
            else return ERR_SYNTAX;
            return ERR_NONE;

        case NODE_BINOP:
            err = eval_depth(node->left, mem, &lv, depth + 1);
            if (err != ERR_NONE) return err;
            err = eval_depth(node->right, mem, &rv, depth + 1);
            if (err != ERR_NONE) return err;

            if (strcmp(node->name, "and")  == 0) {
                *result = (double)((long long)lv & (long long)rv); return ERR_NONE;
            }
            if (strcmp(node->name, "or")   == 0) {
                *result = (double)((long long)lv | (long long)rv); return ERR_NONE;
            }
            if (strcmp(node->name, "xor")  == 0) {
                *result = (double)((long long)lv ^ (long long)rv); return ERR_NONE;
            }
            if (strcmp(node->name, "xnor") == 0) {
                *result = (double)(~((long long)lv ^ (long long)rv)); return ERR_NONE;
            }

            switch (node->name[0]) {
                case '+': *result = lv + rv; break;
                case '-': *result = lv - rv; break;
                case '*': *result = lv * rv; break;
                case '/':
                    if (rv == 0.0) return ERR_DIV_ZERO;
                    *result = lv / rv;
                    break;
                case '^':
                    if (lv < 0.0 && rv != (long long)rv) return ERR_DOMAIN;
                    *result = pow(lv, rv);
                    break;
                case '%':
                    if (rv == 0.0) return ERR_DIV_ZERO;
                    *result = fmod(lv, rv);
                    break;
                default:
                    return ERR_SYNTAX;
            }
            if (is_invalid(*result)) return ERR_OVERFLOW;
            return ERR_NONE;

        case NODE_FUNCTION:
            if (node->argc > 16) return ERR_ARGUMENT;
            for (i = 0; i < node->argc; i++) {
                err = eval_depth(node->args[i], mem, &args[i], depth + 1);
                if (err != ERR_NONE) return err;
            }
            err = eval_function(node->name, args, node->argc, mem, result);
            if (err != ERR_NONE) return err;
            if (is_invalid(*result)) return ERR_MATH;
            return ERR_NONE;

        default:
            return ERR_SYNTAX;
    }
}

void eval_print_result(double result) {
    if (fabs(result) < 1e-10)
        result = 0.0;
    if (result == (long long)result &&
        result >= -1e15 && result <= 1e15) {
        printf("  = %.0f\n", result);
    } else {
        printf("  = %g\n", result);
    }
}
