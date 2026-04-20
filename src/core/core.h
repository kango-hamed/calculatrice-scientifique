#ifndef CALC_CORE_H
#define CALC_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>

/* =========================================================
 * errors.h -- Gestion des erreurs (F-CO-07 a F-CO-11)
 * ========================================================= */

typedef enum {
    ERR_NONE = 0,
    ERR_SYNTAX,       /* expression mal formee          */
    ERR_MATH,         /* erreur mathematique generale   */
    ERR_STACK,        /* pile trop profonde             */
    ERR_ARGUMENT,     /* mauvais argument               */
    ERR_MEMORY,       /* allocation memoire echouee     */
    ERR_DIV_ZERO,     /* division par zero              */
    ERR_DOMAIN,       /* argument hors domaine          */
    ERR_OVERFLOW      /* depassement de plage           */
} CalcError;

static inline const char *error_message(CalcError err) {
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

static inline void error_print(CalcError err, const char *expr, int position) {
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
 * token.h -- Types lexicaux partages entre tokenizer/parser
 * ========================================================= */

typedef enum {
    TOK_NUMBER,     /* 3.14, 42                    */
    TOK_PLUS,       /* +                           */
    TOK_MINUS,      /* -                           */
    TOK_MUL,        /* * ou x                      */
    TOK_DIV,        /* /                           */
    TOK_POW,        /* ^                           */
    TOK_MOD,        /* %                           */
    TOK_LPAREN,     /* (                           */
    TOK_RPAREN,     /* )                           */
    TOK_FUNCTION,   /* sin, cos, log, sqrt...      */
    TOK_VARIABLE,   /* A-F, M, X, Y, Ans           */
    TOK_COMMA,      /* , separateur d'arguments    */
    TOK_ASSIGN,     /* = affectation variable      */
    TOK_AND,        /* and  operateur logique ET   */
    TOK_OR,         /* or   operateur logique OU   */
    TOK_XOR,        /* xor  ou-exclusif            */
    TOK_XNOR,       /* xnor non-ou-exclusif        */
    TOK_END         /* fin de l'expression         */
} TokenType;

typedef struct {
    TokenType type;
    double    value;       /* si TOK_NUMBER           */
    char      name[32];    /* si TOK_FUNCTION/VARIABLE */
    int       position;    /* position dans la chaine  */
} Token;

/* =========================================================
 * ast.h -- Arbre Syntaxique Abstrait (AST)
 * ========================================================= */

typedef enum {
    NODE_NUMBER,    /* feuille : valeur numerique     */
    NODE_VARIABLE,  /* feuille : variable (A-Y, Ans)  */
    NODE_BINOP,     /* noeud : operateur binaire      */
    NODE_UNARYOP,   /* noeud : operateur unaire (neg) */
    NODE_FUNCTION,  /* noeud : appel de fonction      */
    NODE_ASSIGN     /* noeud : affectation A = expr   */
} NodeType;

typedef struct ASTNode {
    NodeType        type;
    double          value;      /* NODE_NUMBER   : la valeur          */
    char            name[32];   /* NODE_VARIABLE : nom                */
                                /* NODE_FUNCTION : nom fonction       */
                                /* NODE_BINOP    : operateur (+,-...) */
                                /* NODE_ASSIGN   : nom variable       */
    struct ASTNode *left;       /* enfant gauche / operande 1         */
    struct ASTNode *right;      /* enfant droit  / operande 2         */
    struct ASTNode **args;      /* arguments fonction                 */
    int             argc;       /* nombre d'arguments                 */
    int             position;   /* position dans l'expression         */
} ASTNode;

ASTNode *ast_make_number  (double value, int pos);
ASTNode *ast_make_variable(const char *name, int pos);
ASTNode *ast_make_binop   (char op, ASTNode *left, ASTNode *right, int pos);
ASTNode *ast_make_unaryop (char op, ASTNode *operand, int pos);
ASTNode *ast_make_function(const char *name, ASTNode **args, int argc, int pos);
ASTNode *ast_make_assign  (const char *var, ASTNode *expr, int pos);
void     ast_free         (ASTNode *node);

/* =========================================================
 * tokenizer.h -- Decoupage de l'expression en tokens
 * Exigences : F-CO-01, F-CO-03, F-CO-05
 * ========================================================= */

#define MAX_TOKENS 256

CalcError tokenize(const char *expr, Token *out, int *count, int *err_pos);

/* =========================================================
 * parser.h -- Analyseur syntaxique par descente recursive
 *
 * Grammaire (du moins au plus prioritaire) :
 *   expression -> term    (('+' | '-') term)*
 *   term       -> power   (('*' | '/' | '%') power)*
 *   power      -> unary   ('^' power)*          [assoc. droite]
 *   unary      -> '-' unary | primary
 *   primary    -> NUMBER | VARIABLE ['=' expression]
 *              |  FUNCTION '(' args ')'
 *              |  '(' expression ')'
 *   args       -> expression (',' expression)*
 * ========================================================= */

typedef struct {
    Token      *tokens;    /* tableau issu du tokenizer  */
    int         count;     /* nombre de tokens           */
    int         pos;       /* position courante          */
    CalcError   error;     /* code d'erreur              */
    int         error_pos; /* position de l'erreur       */
    const char *expr;      /* expression originale       */
} Parser;

void     parser_init       (Parser *p, Token *tokens, int count, const char *expr);
ASTNode *parser_parse      (Parser *p);
void     parser_print_error(const Parser *p);

/* =========================================================
 * evaluator.h -- Evaluation de l'AST
 * Exigences : F-CO-07 a F-CO-10, F-FN-01 a F-FN-22
 * ========================================================= */

#define NB_VARS 26   /* variables A-Z */

typedef struct {
    double vars[NB_VARS];  /* variables A-Z              */
    double mem_M;          /* memoire independante M     */
    double ans;            /* derniere reponse (Ans)     */
    int    angle_deg;      /* 1 = degres, 0 = radians    */
} CalcMemory;

void      eval_memory_init (CalcMemory *mem);
CalcError eval             (const ASTNode *node, CalcMemory *mem, double *result);
void      eval_print_result(double result);

#endif /* CALC_CORE_H */
