#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"
#include "errors.h"

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

/**
 * @brief Initialise le parser.
 * @param p      Parser a initialiser.
 * @param tokens Tableau de tokens.
 * @param count  Nombre de tokens.
 * @param expr   Expression originale (pour les messages d'erreur).
 */
void parser_init(Parser *p, Token *tokens, int count, const char *expr);

/**
 * @brief Analyse les tokens et construit l'AST.
 * @param p Parser initialise.
 * @return Racine de l'AST, ou NULL en cas d'erreur.
 */
ASTNode *parser_parse(Parser *p);

/**
 * @brief Affiche l'erreur du parser sur stderr.
 */
void parser_print_error(const Parser *p);

#endif /* PARSER_H */
