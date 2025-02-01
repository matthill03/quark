#include "ast.h"

ASTNode* ast_init() {
    ASTNode* new_ast = malloc(sizeof(ASTNode));
    if (!new_ast) {
        printf("Failed to allocate memory for new ast root\n");
        exit(EXIT_FAILURE);
    }

    new_ast->type = AST_PROGRAM;
    new_ast->value.program.functions = ast_create_empty();
    new_ast->next = NULL;

    return new_ast;
}

void ast_append_node(ASTNode** branch_root, ASTNode* node_to_append) {
    if (!branch_root || !node_to_append) { return; }

    if ((*branch_root)->type == AST_NONE) {
        (*branch_root)->type = node_to_append->type;
        (*branch_root)->next = node_to_append->next;
        (*branch_root)->value = node_to_append->value;
        return;
    }

    ASTNode* last_node = *branch_root;
    while (last_node->next != NULL) {
        last_node = last_node->next;
    }
    last_node->next = node_to_append;
}

ASTNode* ast_create_empty() {
    ASTNode* new_ast = calloc(1, sizeof(ASTNode));
    if (!new_ast) {
        printf("Failed to allocate memory for new ast empty node\n");
        exit(EXIT_FAILURE);
    }

    new_ast->type = AST_NONE;

    return new_ast;

}

ASTNode* ast_create_fn_decl(const char* name, const char* return_type, ASTNode* body) {
    if (!body) {
        printf("Body AST node passed into fn decl is not valid\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_fn_decl_node = malloc(sizeof(ASTNode));
    new_fn_decl_node->type = AST_FUNCTION_DECL;

    new_fn_decl_node->value.function_decl.name = name;
    new_fn_decl_node->value.function_decl.return_type = return_type;
    new_fn_decl_node->value.function_decl.body = body;
    new_fn_decl_node->next = NULL;

    return new_fn_decl_node;
}

ASTNode* ast_create_var_decl(const char* name, const char* type, ASTNode* value) {
    if (value->type != AST_LITERAL) {
        printf("Var decl value must be a literal\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_var_decl_node = malloc(sizeof(ASTNode));
    if (!new_var_decl_node) {
        printf("Failed to allocate memory for var decl node\n");
        exit(EXIT_FAILURE);
    }

    new_var_decl_node->type = AST_VARIABLE_DECL;
    new_var_decl_node->value.variable_decl.name = name;
    new_var_decl_node->value.variable_decl.type = type;
    new_var_decl_node->value.variable_decl.value = value;
    new_var_decl_node->next = NULL;

    return new_var_decl_node;
}

ASTNode* ast_create_literal(const char* type, int value) {
    ASTNode* new_literal_node = malloc(sizeof(ASTNode));
    if (!new_literal_node) {
        printf("Failed to allocate memory for new literal node\n");
        exit(EXIT_FAILURE);
    }

    new_literal_node->type = AST_LITERAL;
    new_literal_node->value.literal.int_value = value;
    new_literal_node->value.literal.type = type;
    new_literal_node->next = NULL;

    return new_literal_node;
}

ASTNode* ast_create_return_stmt(ASTNode* value) {
    if (!value || value->type != AST_LITERAL) {
        printf("Value given in ret stmt node creation is not valid\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_ret_stmt_node = malloc(sizeof(ASTNode));
    if (!new_ret_stmt_node) {
        printf("Failed to allocate memory for new return stmt node\n");
        exit(EXIT_FAILURE);
    }

    new_ret_stmt_node->type = AST_RETURN_STMT;
    new_ret_stmt_node->value.return_stmt.value = value;
    new_ret_stmt_node->next = NULL;

    return new_ret_stmt_node;
}

void print_ast(ASTNode* root) {
    if (root->type != AST_PROGRAM) {
        printf("Top level root must be of type AST_PROGRAM\n");
        exit(EXIT_FAILURE);
    }

    printf("Program -> %p\n", root);
    ASTNode* current_node = root->value.program.functions;
    print_ast_node(current_node);
    while (current_node->next) {
        current_node = current_node->next;
        print_ast_node(current_node);
    }
}

void print_ast_node(ASTNode* node) {
    switch(node->type) {
        case AST_FUNCTION_DECL:
            print_ast_fn_decl(node);
            break;
        case AST_VARIABLE_DECL:
            print_ast_var_decl(node);
            break;
        case AST_RETURN_STMT:
            print_ast_ret_stmt(node);
            break;
        default:
            printf("Type (%d) not supported in a body\n", node->type);
    }
}

void print_ast_fn_decl(ASTNode* node) {
    if (node->type != AST_FUNCTION_DECL) {
        printf("To printf function decl you pust give a function decl\n");
        exit(EXIT_FAILURE);
    }

    printf("|-- Function def: Name -> %s, Return -> %s, Body -> %p\n", node->value.function_decl.name, node->value.function_decl.return_type, node->value.function_decl.body);
    ASTNode* current_body_node = node->value.function_decl.body;
    printf("    |-- ");
    print_ast_node(current_body_node);

    while (current_body_node->next) {
        current_body_node = current_body_node->next;
        printf("    |-- ");
        print_ast_node(current_body_node);
    }
}

void print_ast_var_decl(ASTNode* node) {
    if (node->type != AST_VARIABLE_DECL) {
        printf("To print variable decl you pust give a variable decl\n");
        exit(EXIT_FAILURE);
    }

    printf("Var definition: Name -> %s, Type -> %s, Value -> %d\n", node->value.variable_decl.name, node->value.variable_decl.type, node->value.variable_decl.value->value.literal.int_value);
}

void print_ast_ret_stmt(ASTNode* node) {
    if (node->type != AST_RETURN_STMT) {
        printf("To print return stmt you pust give a return stmt, (%d)\n", node->type);
        exit(EXIT_FAILURE);
    }

    printf("Return Stmt: Value -> %d, Type -> %s\n", node->value.return_stmt.value->value.literal.int_value, node->value.return_stmt.value->value.literal.type);
}
