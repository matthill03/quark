#ifndef Q_AST_H
#define Q_AST_H
#include "stdio.h"
#include "stdlib.h"

// ----- AST -----
typedef enum {
    AST_PROGRAM = 0,
    AST_FUNCTION_DECL,
    //AST_FUNCTION_CALL,
    AST_VARIABLE_DECL,
    AST_RETURN_STMT,
    AST_LITERAL,
    AST_NONE,
} ASTNodeType;

typedef union {
    struct {
        // This could act as the 'context' for the file??
        // struct ASTNode* varibles;
        struct ASTNode* functions;
    } program;

    struct {
        const char* name;
        const char* return_type;
        struct ASTNode* body;
    } function_decl;

    struct {
        // TODO: Make const a possible thing.
        // bool mut;
        const char* name;
        const char* type;
        struct ASTNode* value;
    } variable_decl;

    struct {
        struct ASTNode* value;
    } return_stmt;

    struct {
        int int_value;
        const char* type;
    } literal;
} ASTNodeValue;

typedef struct ASTNode {
    ASTNodeType type;
    ASTNodeValue value;
    struct ASTNode* next;
} ASTNode;

ASTNode* ast_init();
void ast_append_node(ASTNode** branch_root, ASTNode* node_to_append);
ASTNode* ast_create_empty();
ASTNode* ast_create_fn_decl(const char* name, const char* return_type, ASTNode* body);
ASTNode* ast_create_var_decl(const char* name, const char* type, ASTNode* value);
ASTNode* ast_create_literal(const char* type, int value);
ASTNode* ast_create_return_stmt(ASTNode* value);

void print_ast(ASTNode* root);
void print_ast_node(ASTNode* node);
void print_ast_fn_decl(ASTNode* node);
void print_ast_var_decl(ASTNode* node);
void print_ast_ret_stmt(ASTNode* node);

#endif
