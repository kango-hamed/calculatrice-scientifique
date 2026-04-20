#ifndef TOKEN_H
#define TOKEN_H

/* =========================================================
 * token.h -- Types lexicaux partagés entre tokenizer/parser
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
    char      name[32];   /* si TOK_FUNCTION/VARIABLE */
    int       position;   /* position dans la chaine  */
} Token;

#endif /* TOKEN_H */
