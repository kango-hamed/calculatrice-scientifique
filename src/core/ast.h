#ifndef AST_H
#define AST_H

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

#endif /* AST_H */
