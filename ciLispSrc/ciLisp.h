/**
* Name: Jose de Jesus Rodriguez Rivas
* Lab: ciLisp
* Date: 04/24/19
**/
#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper { // must be in sync with funcs in resolveFunc()
    NEG_OPER, // 0
    ABS_OPER, // 1
    EXP_OPER, // 2
    SQRT_OPER, // 3
    ADD_OPER, // 4
    SUB_OPER, // 5
    MULT_OPER, // 6
    DIV_OPER, // 7
    REMAINDER_OPER, // 8
    LOG_OPER, // 9
    POW_OPER, // 10
    MAX_OPER, // 11
    MIN_OPER, // 12
    EXP2_OPER, // 13
    CBRT_OPER, // 14
    HYPOT_OPER, // 15
    SIN_OPER, // 16
    COS_OPER, // 17
    TAN_OPER, // 18
    PRINT_OPER, // 19
    READ_OPER,
    RAND_OPER,
    EQUAL_OPER,
    SMALLER_OPER,
    LARGER_OPER,
    CUSTOM_FUNC=255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);

typedef enum {
    NUM_TYPE, FUNC_TYPE, SYMBOL_TYPE, COND_TYPE
} AST_NODE_TYPE;

typedef enum { NO_TYPE, INTEGER_TYPE, REAL_TYPE } DATA_TYPE;

typedef enum {
    VARIABLE_TYPE,
    LAMBDA_TYPE,
    ARG_TYPE
} SYM_TYPE;

typedef struct stack_node {
    struct ast_node *val;
    struct stack_node *next;
} STACK_NODE;

typedef struct return_value {
    DATA_TYPE type;
    double value;
} RETURN_VALUE;

typedef struct {
    RETURN_VALUE value;
} NUMBER_AST_NODE;

typedef struct {
    char *name;
    struct ast_node *opList;
} FUNCTION_AST_NODE;

typedef struct symbol_table_node {
    SYM_TYPE type;
    DATA_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct stack_node *stack;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

typedef struct symbol_ast_node {
    char *name;
} SYMBOL_AST_NODE;

typedef struct {
    struct ast_node *cond;
    struct ast_node *zero;
    struct ast_node *nonzero;
} COND_AST_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    struct ast_node *parent;
    union {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        COND_AST_NODE condition;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *real_number(double value);
AST_NODE *int_number(int value);

AST_NODE *function(char *funcName, AST_NODE *opList);

AST_NODE *conditional(AST_NODE *cond, AST_NODE *nonzero, AST_NODE *zero);

void freeNode(AST_NODE *p);

RETURN_VALUE eval(AST_NODE *ast);

/**
 * Creates a symbol table node
 * @param symbol
 * @param s_expr
 * @return
 */
SYMBOL_TABLE_NODE *createSymbol(DATA_TYPE type, char *symbol, AST_NODE *s_expr);

/**
 * Adds let_elem to the given list
 * @param let_list
 * @param let_elem
 */
SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *let_list, SYMBOL_TABLE_NODE *let_elem);

AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *let_section, AST_NODE *s_expr);

AST_NODE *symbol(char *symb);

void freeSymbolTable(SYMBOL_TABLE_NODE *node);

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *symbol);

AST_NODE *addNodeToList(AST_NODE *toAdd, AST_NODE *list);

SYMBOL_TABLE_NODE *createArgList(char *symbol);

SYMBOL_TABLE_NODE *addSymbolToArgList(char *symbol, SYMBOL_TABLE_NODE *arg_list);

SYMBOL_TABLE_NODE *createLambda(DATA_TYPE type, char*ident, SYMBOL_TABLE_NODE *argList, AST_NODE *body);



#endif
