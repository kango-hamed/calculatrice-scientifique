#ifndef CALC_CORE_H
#define CALC_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include "matrix.h"

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

const char *error_message(CalcError err);
void        error_print(CalcError err, const char *expr, int position);

/* =========================================================
 * Modes de la calculatrice
 * ========================================================= */
typedef enum {
    MODE_COMP,      /* Mode calcul standard          */
    MODE_CMPLX,     /* Mode nombres complexes        */
    MODE_STAT,      /* Mode statistiques             */
    MODE_MATRIX,    /* Mode matrices                 */
    MODE_TABLE,     /* Mode table de valeurs         */
    MODE_BASE_N,    /* Mode base-n                   */
    MODE_EQN        /* Mode equations                */
} CalcMode;

/* =========================================================
 * ComplexValue -- Type de base pour tous les nombres
 * ========================================================= */

typedef struct {
    int is_matrix;
    double re;    /* partie reelle      */
    double im;    /* partie imaginaire  */
    Matrix mat;
} ComplexValue;

/* Fonctions utilitaires complexes */
ComplexValue cx_make (double re, double im);
ComplexValue cx_add  (ComplexValue a, ComplexValue b);
ComplexValue cx_sub  (ComplexValue a, ComplexValue b);
ComplexValue cx_mul  (ComplexValue a, ComplexValue b);
ComplexValue cx_div  (ComplexValue a, ComplexValue b);
ComplexValue cx_conj (ComplexValue a);
double       cx_mod  (ComplexValue a);
double       cx_arg  (ComplexValue a);
int          cx_is_real(ComplexValue a);

/* =========================================================
 * token.h -- Types lexicaux partages entre tokenizer/parser
 * ========================================================= */

typedef enum {
    TOK_NUMBER,     /* 3.14, 42                    */
    TOK_IMAG,       /* unite imaginaire 'i'        */  /* ADDED */
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
    TOK_SQUARE,     /* x² (postfixe)               */
    TOK_CUBE,       /* x³ (postfixe)               */
    TOK_INV,        /* x⁻¹ (postfixe)              */
    TOK_FACT,       /* n! (postfixe)               */
    TOK_NPR,        /* nPr (permutation)           */
    TOK_NCR,        /* nCr (combinaison)           */
    TOK_END         /* fin de l'expression         */
} TokenType;

typedef struct {
    TokenType type;
    double    value;     /* si TOK_NUMBER : valeur reelle         */
                         /* si TOK_IMAG   : toujours 1.0          */
    char      name[32];  /* si TOK_FUNCTION/VARIABLE : nom        */
    int       position;  /* position dans la chaine d'expression  */
} Token;

/* =========================================================
 * ast.h -- Arbre Syntaxique Abstrait (AST)
 * ========================================================= */

typedef enum {
    NODE_NUMBER,    /* feuille : valeur numerique reelle  */
    NODE_IMAG,      /* feuille : unite imaginaire (0+1i)  */  /* ADDED */
    NODE_VARIABLE,  /* feuille : variable (A-Z, Ans)      */
    NODE_BINOP,     /* noeud : operateur binaire          */
    NODE_UNARYOP,   /* noeud : operateur unaire (neg)     */
    NODE_FUNCTION,  /* noeud : appel de fonction          */
    NODE_ASSIGN     /* noeud : affectation A = expr       */
} NodeType;

typedef struct ASTNode {
    NodeType        type;
    double          value;      /* NODE_NUMBER : valeur scalaire      */
                                /* NODE_IMAG   : coefficient imag (1) */
    char            name[32];   /* NODE_VARIABLE : nom var            */
                                /* NODE_FUNCTION : nom fonction       */
                                /* NODE_BINOP    : operateur char      */
                                /* NODE_ASSIGN   : nom variable cible  */
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
 * ========================================================= */

#define MAX_TOKENS 256

CalcError tokenize(const char *expr, Token *out, int *count, int *err_pos);

/* =========================================================
 * parser.h -- Analyseur syntaxique par descente recursive
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
 * ========================================================= */

#define NB_VARS 26   /* variables A-Z */

typedef struct {
    ComplexValue vars[NB_VARS];  /* variables A-Z (peuvent etre complexes) */
    ComplexValue mat_vars[3];    /* MatA, MatB, MatC                       */
    ComplexValue mem_M;          /* memoire independante M                 */
    ComplexValue ans;            /* derniere reponse (Ans)                 */
    int          angle_deg;      /* 1 = degres, 0 = radians                */
    int          complex_mode;   /* 1 = mode complexe actif                */
    CalcMode     current_mode; /* Mode actif de la calculatrice          */
} CalcMemory;

void      eval_memory_init (CalcMemory *mem);
CalcError eval             (const ASTNode *node, CalcMemory *mem, ComplexValue *result);
void      eval_print_result(ComplexValue result, int complex_mode);

/* Vérification des fonctions par mode */
int is_function_valid_for_mode(const char *func_name, CalcMode mode);
CalcMode detect_mode_from_expr(const char *expr);
const char *mode_name(CalcMode mode);

#endif /* CALC_CORE_H */