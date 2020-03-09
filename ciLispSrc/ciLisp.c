/**
* Name: Jose de Jesus Rodriguez Rivas
* Lab: ciLisp
* Date: 04/24/19
**/

#include "ciLisp.h"

#define NaN NAN

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    exit(0);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//
// find out which function it is
//


char *func[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "sin",
        "cos",
        "tan",
        "print",
        "read",
        "rand",
        "equal",
        "smaller",
        "larger",
        ""
};

bool seeded = false;

OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (func[i][0] != '\0') {
        if (strcmp(func[i], funcName) == 0)
            return i;

        i++;
    }
    return CUSTOM_FUNC;
}

/**
 * Create a node for a real number
 * @param value The double value of the node
 * @return A NUM_TYPE ast node with a real value
 */
AST_NODE *real_number(double value) {
    AST_NODE *p = calloc(1, sizeof(AST_NODE));
    if (p == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value.type = REAL_TYPE;
    p->data.number.value.value = value;

    return p;
}

/**
 * Create a node for an integer number
 * @param value The int value of the node
 * @return A NUM_TYPE ast node with an integer value
 */
AST_NODE *int_number(int value) {
    AST_NODE *p = calloc(1, sizeof(AST_NODE));
    if (p == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value.type = INTEGER_TYPE;
    p->data.number.value.value = value;

    return p;
}

/**
 * Creates an ast node for a function call. <br />
 * All nodes in opList have their parents set to the new node
 * @param funcName The name of the function to be called
 * @param opList The s_expressions of the operands as a linked list
 * @return A FUNC_TYPE ast node holding the function name and the opList
 */
AST_NODE *function(char *funcName, AST_NODE *opList) {
    AST_NODE *p = calloc(1, sizeof(AST_NODE));
    if (p == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.opList = opList;
    AST_NODE *cN = opList;
    while (cN != NULL) {
        cN->parent = p;
        cN = cN->next;
    }

    return p;
}

/**
 * Creates an ast node for a conditional expression. <br />
 * The cond, nonzero, and zero nodes will have their parents set to the new node.
 * @param cond The condition to be evaluated, 0 means false, a number not equal to zero means true
 * @param nonzero The expression to evaluate when cond returns true
 * @param zero The expression to evaluate when the cond returns false
 * @return The COND_TYPE ast node holding the conditional statement.
 */
AST_NODE *conditional(AST_NODE *cond, AST_NODE *nonzero, AST_NODE *zero) {
    AST_NODE *p = calloc(1, sizeof(AST_NODE));
    if (p == NULL) {
        yyerror("Out of memory");
    }

    p->type = COND_TYPE;
    p->data.condition.cond = cond;
    p->data.condition.nonzero = nonzero;
    p->data.condition.zero = zero;

    cond->parent = p;
    zero->parent = p;
    nonzero->parent = p;

    return p;

}

/**
 * Recursively frees all nodes in the given chain.<br/>
 * Also frees all string identifiers, symbol tables, and other embedded ast nodes.
 * @param p
 */
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    freeNode(p->next);

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        freeNode(p->data.function.opList);
    }

    if (p->type == SYMBOL_TYPE) {
        free(p->data.symbol.name);
    }

    if (p->type == COND_TYPE) {
        freeNode(p->data.condition.cond);
        freeNode(p->data.condition.nonzero);
        freeNode(p->data.condition.zero);
    }

    freeSymbolTable(p->symbolTable);

    free(p);
}

/**
 * Generates a pseudorandom number using rand().<br/>
 * If the generator has not been seeded, srand() is called with current time.
 * @return
 */
double getRand() {
    time_t t;
    if (!seeded) {
        srand((unsigned) time(&t));
        seeded = true;
    }
    return rand();
}

/**
 * Evaluates a function with no parameters
 * @param out The reference to the return value that will be returned
 * @param operation The operation to evalulate
 */
void evalParamless(RETURN_VALUE *out, OPER_TYPE operation) {
    switch (operation) {
        case READ_OPER:
            printf("read := ");
            scanf("%lf", &out->value);
            out->type = floor(out->value) == out->value ? INTEGER_TYPE : REAL_TYPE;
            while ((getchar()) != '\n');
            break;
        case RAND_OPER:
            out->value = getRand();
            out->type = floor(out->value) == out->value ? INTEGER_TYPE : REAL_TYPE;
        default:
            break;
    }
}

/**
 * Evaluates a function that takes one operand
 * @param out The reference to the return value that will be returned
 * @param operation The operation to evaluate
 * @param p The original s_expr (unused)
 * @param opList The operand list of the function call
 */
void evalUnary(RETURN_VALUE *out, OPER_TYPE operation, AST_NODE *p, AST_NODE *opList) {
    RETURN_VALUE p1 = eval(opList);
    double op = p1.value;
    out->type = p1.type;
    if (opList->next != NULL) {
        printf("WARNING: Too many arguments for operation %s\n", func[operation]);
    }
    switch (operation) {
        case NEG_OPER:
            out->value = -op;
            break;
        case ABS_OPER:
            out->value = fabs(op);
            break;
        case EXP_OPER:
            out->value = exp(op);
            break;
        case SQRT_OPER:
            out->value = sqrt(op);
            break;
        case LOG_OPER:
            out->value = log(op);
            break;
        case EXP2_OPER:
            out->value = exp2(op);
            break;
        case CBRT_OPER:
            out->value = cbrt(op);
            break;
        case SIN_OPER:
            out->value = sin(op);
            break;
        case COS_OPER:
            out->value = cos(op);
            break;
        case TAN_OPER:
            out->value = tan(op);
            break;
        default:
            break;
    }
}

/**
 * Evaluates a function that takes two operands.
 * @param out The reference to the return value that will be returned
 * @param operation The operation to evaluate
 * @param p The original s_expr (unused)
 * @param opList The operand list of the function call
 */
void evalBinary(RETURN_VALUE *out, OPER_TYPE operation, AST_NODE *p, AST_NODE *opList) {
    if (opList->next == NULL) {
        printf("Expected two params for operation: %s\n", func[operation]);
        return;
    }
    RETURN_VALUE p1 = eval(opList);
    RETURN_VALUE p2 = eval(opList->next);
    out->type = REAL_TYPE;
    if (p1.type == INTEGER_TYPE && p2.type == INTEGER_TYPE) {
        out->type = INTEGER_TYPE;
    }
    double op1 = p1.value;
    double op2 = p2.value;
    if (opList->next->next != NULL) {
        printf("WARNING: Too many arguments for operation: %s\n", func[operation]);
    }
    switch (operation) {
        case SUB_OPER:
            out->value = op1 - op2;
            break;
        case DIV_OPER:
            if (isnan(op2) || op2 == 0) {
                out->value = NaN;
                out->type = NO_TYPE;
            } else {
                out->value = op1 / op2;
            }
            break;
        case REMAINDER_OPER:
            out->value = remainder(op1, op2);
            break;
        case POW_OPER:
            out->value = pow(op1, op2);
            break;
        case MAX_OPER:
            out->value = fmax(op1, op2);
            break;
        case MIN_OPER:
            out->value = fmin(op1, op2);
            break;
        case HYPOT_OPER:
            out->value = hypot(op1, op2);
            break;
        case EQUAL_OPER:
            out->type = INTEGER_TYPE;
            out->value = op1 == op2 ? 1 : 0;
            break;
        case LARGER_OPER:
            out->type = INTEGER_TYPE;
            out->value = op1 > op2 ? 1 : 0;
            break;
        case SMALLER_OPER:
            out->type = INTEGER_TYPE;
            out->value = op1 < op2 ? 1 : 0;
            break;
        default:
            break;
    }
}

/**
 * Logic for the print function.
 * @param out The reference to the return value that will be returned
 * @param param The value to print
 * @param isFirst Determines whether to print the "=> "
 */
void printFunc(RETURN_VALUE *out, RETURN_VALUE param, bool isFirst) {
    if (isFirst) {
        printf("=> ");
    }
    out->type = param.type;
    out->value = param.value;
    if (param.type == INTEGER_TYPE) {
        out->value = (int) out->value;
        printf("%d ", (int) out->value);
    } else {
        printf("%.2lf ", out->value);
    }
}

/**
 * Evaluates a function with any number of params
 * @param out The reference to the return value that will be returned
 * @param operation The operation to evaluate
 * @param p The original s_expr (unused)
 * @param opList The operand list of the function call
 */
void evalArbitrary(RETURN_VALUE *out, OPER_TYPE operation, AST_NODE *p, AST_NODE *opList) {
    if (!opList) {
        return;
    }
    RETURN_VALUE param1 = eval(opList);
    out->value = param1.value;
    out->type = param1.type == NO_TYPE ? REAL_TYPE : param1.type;

    AST_NODE *cN = opList->next;
    if (operation == PRINT_OPER) printFunc(out, param1, true);
    while (cN != NULL) {
        RETURN_VALUE param = eval(cN);
        if (out->type == INTEGER_TYPE && param.type != INTEGER_TYPE) {
            out->type = REAL_TYPE;
        }
        switch (operation) {
            case ADD_OPER:
                out->value += param.value;
                break;
            case MULT_OPER:
                out->value *= param.value;
                break;
            case PRINT_OPER:
                printFunc(out, param, false);
                break;
            default:
                break;
        }
        cN = cN->next;
    }
    if (operation == PRINT_OPER) printf("\n");

}

/**
 * Finds the user defined function with the given name.<br />
 * Recursively searches through the parent of the given node.
 * @param lambda The name of the function to search for
 * @param p The lowest scope to search.
 * @return The symbol table node containing the function, or null if it was not found.
 */
SYMBOL_TABLE_NODE *findLambda(char *lambda, AST_NODE *p) {
    SYMBOL_TABLE_NODE *cN = p->symbolTable;
    while (cN != NULL) {
        if (cN->type == LAMBDA_TYPE && strcmp(lambda, cN->ident) == 0) {
            return cN;
        }
        cN = cN->next;
    }
    if (p->parent != NULL) {
        return findLambda(lambda, p->parent);
    }
    return NULL;

}

/**
 * Pushes an expression to the given argument stack.<br/>
 * The expression is evaluated and pushed as a NUM_TYPE node.
 * @param arg The argument to push to the stack
 * @param val The expression that will be evaluated
 */
void pushToStack(SYMBOL_TABLE_NODE *arg, AST_NODE *val) {
    STACK_NODE *newHead = calloc(1, sizeof(STACK_NODE));
    newHead->next = arg->stack;
    RETURN_VALUE actualValue = eval(val);
    if (actualValue.type == INTEGER_TYPE) {
        newHead->val = int_number((int) actualValue.value);
    } else {
        newHead->val = real_number((actualValue.value));
    }
    arg->stack = newHead;
}

/**
 * Pops the top off the given arg's stack.
 * @param arg The arg to pop the stack
 * @return False if the stack was empty, true if it was not.
 */
bool popFromStack(SYMBOL_TABLE_NODE *arg) {
    if (arg->stack == NULL) {
        return false;
    }
    STACK_NODE *newHead = arg->stack->next;
    freeNode(arg->stack->val); // when pushing, this was created
    free(arg->stack);
    arg->stack = newHead;
    return true;
}

/**
 * Pushes the given opList to the correct args in given s_expr.<br/>
 * Accounts for the fact that lambda may have non argument variables.<br/>
 * Reports if there were too many arguments given, or too little.
 * @param lambda
 * @param opList
 * @return
 */
bool pushArgsToStack(AST_NODE *lambda, AST_NODE *opList) {
    AST_NODE *currentOp = opList;
    SYMBOL_TABLE_NODE *currentArg = lambda->symbolTable;
    while (currentArg != NULL && currentOp != NULL) {
        if (currentArg->type == ARG_TYPE) {
            pushToStack(currentArg, currentOp);
            currentOp = currentOp->next;
        }
        currentArg = currentArg->next;
    }
    if (currentOp != NULL) {
        yyerror("Too many arguments");
        return false;
    }

    while (currentArg != NULL) {
        if (currentArg->type == ARG_TYPE) {
            yyerror("No enough arguments");
            return false;
        }
        currentArg = currentArg->next;
    }

    return true;
}

/**
 * Pops all arguments for the given function once
 * @param lambda
 */
void popArgsFromStack(AST_NODE *lambda) {
    SYMBOL_TABLE_NODE *currentArg = lambda->symbolTable;
    while (currentArg != NULL) {
        if (currentArg->type == ARG_TYPE) {
            popFromStack(currentArg);
        }
        currentArg = currentArg->next;
    }
}

/**
 * Evaluates a custom function
 * @param out The return value to store output of function
 * @param p The function to evaluate.
 */
void evalCustom(RETURN_VALUE *out, AST_NODE *p) {
    SYMBOL_TABLE_NODE *lambdaNode = findLambda(p->data.function.name, p);
    if(lambdaNode == NULL) {
        yyerror("Function not found");
    }
    AST_NODE *lambdaValue = lambdaNode->val;
    pushArgsToStack(lambdaValue, p->data.function.opList);
    *out = eval(lambdaValue);
    popArgsFromStack(lambdaValue);
}

/**
 * Evaluates a function node
 * @param p The node to evaluate
 * @return The result of the function call
 */
RETURN_VALUE evalFunc(AST_NODE *p) {
    RETURN_VALUE result;
    result.type = NO_TYPE;
    OPER_TYPE op = resolveFunc(p->data.function.name);
    switch (op) {
        case NEG_OPER:
        case ABS_OPER:
        case EXP_OPER:
        case SQRT_OPER:
        case LOG_OPER:
        case EXP2_OPER:
        case CBRT_OPER:
        case SIN_OPER:
        case COS_OPER:
        case TAN_OPER:
            evalUnary(&result, op, p, p->data.function.opList);
            break;
        case SUB_OPER:
        case DIV_OPER:
        case REMAINDER_OPER:
        case POW_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case HYPOT_OPER:
        case EQUAL_OPER:
        case SMALLER_OPER:
        case LARGER_OPER:
            evalBinary(&result, op, p, p->data.function.opList);
            break;
        case ADD_OPER:
        case MULT_OPER:
        case PRINT_OPER:
            evalArbitrary(&result, op, p, p->data.function.opList);
            break;
        case READ_OPER:
        case RAND_OPER:
            evalParamless(&result, op);
            break;
        case CUSTOM_FUNC:
            evalCustom(&result, p);
            break;
        default:
            break;
    }
    return result;
}

/**
 * Evalates a variable symbol
 * @param variable The variable to evaluate
 * @return The value stored in the variable
 */
RETURN_VALUE evalVariable(SYMBOL_TABLE_NODE *variable) {
    double literalVal = eval(variable->val).value;
    if (variable->val_type == INTEGER_TYPE) {
        literalVal = (int) literalVal;
    }
    RETURN_VALUE result;
    result.type = variable->val_type;
    result.value = literalVal;
    return result;
}

/**
 * Evaluates the top of the given arg's stack
 * @param arg The arg to evaluate
 * @return The value at the top of the arg's stack, or zero if it was null
 */
RETURN_VALUE evalArg(SYMBOL_TABLE_NODE *arg) {
    RETURN_VALUE result = (RETURN_VALUE) {NO_TYPE, 0.0};
    if (arg->stack != NULL) {
        result = eval(arg->stack->val);
    }
    return result;
}

/**
 * Evaluates a symbol node, should only be for variables and args. <br/>
 * Uses a nested loop to search up the scope of the symbol.
 * @param p The symbol node
 * @return The value at the given symbol.
 */
RETURN_VALUE evalSymbol(AST_NODE *p) {
    AST_NODE *parent = p;
    while (parent != NULL) {
        SYMBOL_TABLE_NODE *cN = parent->symbolTable;
        while (cN != NULL) {
            if (strcmp(cN->ident, p->data.symbol.name) == 0) {
                if (cN->type == VARIABLE_TYPE) {
                    return evalVariable(cN);
                } else if (cN->type == ARG_TYPE) {
                    return evalArg(cN);
                }
            }
            cN = cN->next;
        }
        parent = parent->parent;
    }
    char *error = malloc(128 * sizeof(char));
    sprintf(error, "The variable %s is undefined", p->data.symbol.name);
    yyerror(error);
    free(error);
    exit(0);
}

/**
 * Evaluates a conditional.
 * @param p The conditional expression
 * @return The value of the zero expression of the condition's value was zero, the other one otherwise.
 */
RETURN_VALUE evalCond(AST_NODE *p) {
    RETURN_VALUE condition = eval(p->data.condition.cond);

    if (condition.value == 0)
        return eval(p->data.condition.zero);
    return eval(p->data.condition.nonzero);
}

/**
 * Evaluates the given expression
 * @param p The expression to evaluate
 * @return
 */
RETURN_VALUE eval(AST_NODE *p) {

    RETURN_VALUE result = (RETURN_VALUE) {NO_TYPE, 0.0};
    if (!p) {
        return result;
    }

    if (p->type == NUM_TYPE) {
        result = p->data.number.value;
    }

    if (p->type == FUNC_TYPE) {
        result = evalFunc(p);
    }

    if (p->type == SYMBOL_TYPE) {
        result = evalSymbol(p);
    }

    if (p->type == COND_TYPE) {
        result = evalCond(p);
    }

    return result;
}

/**
 * Creates a variable with the given type, name and value<br/>
 * If type is null, the variable will be set to NO_TYPE<br/>
 * If the s_expr is a call to read or rand, evaluate the function and set the variables value to a number containing the result.
 * @param type The type of the variable
 * @param symbol The name of the variable
 * @param s_expr The value of the variable
 * @return A symbol table node representing the variable
 */
SYMBOL_TABLE_NODE *createSymbol(DATA_TYPE type, char *symbol, AST_NODE *s_expr) {
    SYMBOL_TABLE_NODE *p = calloc(1, sizeof(SYMBOL_TABLE_NODE));

    if (p == NULL) {
        yyerror("out of memory");
    }
    p->val_type = type;

    p->ident = symbol;
    p->val = s_expr;
    p->type = VARIABLE_TYPE;

    if (s_expr->type == FUNC_TYPE) {
        OPER_TYPE oper = resolveFunc(s_expr->data.function.name);
        if (oper == READ_OPER || oper == RAND_OPER) {
            AST_NODE *newVal;
            RETURN_VALUE readVal = eval(s_expr);
            if (readVal.type == INTEGER_TYPE) {
                newVal = int_number((int) readVal.value);
            } else {
                newVal = real_number(readVal.value);
            }
            p->val = newVal;
            freeNode(s_expr);
        }
    }

    return p;
}

/**
 * Adds the given symbol to the beginning of the given list
 * @param let_list The list to add to
 * @param let_elem The element to add
 * @return The head of the list
 */
SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *let_list, SYMBOL_TABLE_NODE *let_elem) {
    if (let_elem == NULL) {
        return let_list;
    }

    if (let_elem->val == NULL) {
        if (let_elem->type == VARIABLE_TYPE) {
            return let_list;
        } else if (let_elem->next == NULL) {
            let_elem->next = let_list;
        } else {
            addSymbolToList(let_list, let_elem->next);
        }
        return let_elem;
    }

    SYMBOL_TABLE_NODE *symbol = findSymbol(let_list, let_elem);
    if (symbol == NULL) {
        let_elem->next = let_list;
        return let_elem;
    } else {
        symbol->val = let_elem->val;
        symbol->val->data = let_elem->val->data;
        freeSymbolTable(let_elem);
        return let_list;
    }
}

/**
 * Sets the symbol table of the given expression<br/>
 * The parent of all the values in the symbol table is set to the expression
 * @param let_section The symbol table
 * @param s_expr The expression
 * @return The given expression
 */
AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *let_section, AST_NODE *s_expr) {
    if (s_expr == NULL) {
        return NULL;
    }

    SYMBOL_TABLE_NODE *cN = let_section;
    while (cN != NULL) {
        cN->val->parent = s_expr;
        cN = cN->next;
    }

    s_expr->symbolTable = let_section;
    return s_expr;
}

/**
 * Creates a symbol ast node
 * @param symb The symbol
 * @return A SYMBOL_TYPE ast node
 */
AST_NODE *symbol(char *symb) {
    AST_NODE *p = calloc(1, sizeof(AST_NODE));
    if (p == NULL)
        yyerror("out of memory");

    p->type = SYMBOL_TYPE;
    p->data.symbol.name = malloc(strlen(symb) + 1);
    strcpy(p->data.symbol.name, symb);
    return p;
}

/**
 * Frees a given symbol table list recursively
 * @param node
 */
void freeSymbolTable(SYMBOL_TABLE_NODE *node) {
    if (node == NULL) {
        return;
    }
    freeSymbolTable(node->next);
    free(node->ident);
    freeNode(node->val);
    free(node);
}

/**
 * Searches for a symbol in a symbol table
 * @param symbolTable
 * @param symbol
 * @return The found symbol, or null
 */
SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *symbol) {
    if (symbol == NULL) {
        return NULL;
    }
    SYMBOL_TABLE_NODE *cNode = symbolTable;
    while (cNode != NULL) {
        if (strcmp(cNode->ident, symbol->ident) == 0) {
            return cNode;
        }
        cNode = cNode->next;
    }
    return NULL;
}

/**
 * Adds a symbol to the given argument list at the head of the list.
 * @param toAdd
 * @param list
 * @return The head of the list
 */
AST_NODE *addNodeToList(AST_NODE *toAdd, AST_NODE *list) {
    if (toAdd == NULL) {
        return list;
    }

    toAdd->next = list;
    return toAdd;
}

/**
 * Checks if a symbol is already an argument in an argument list
 * @param arg
 * @param arg_list
 * @return
 */
bool argInArgList(char *arg, SYMBOL_TABLE_NODE *arg_list) {
    SYMBOL_TABLE_NODE *cN = arg_list;
    while (cN != NULL) {
        if (strcmp(arg, cN->ident) == 0) {
            return true;
        }
        cN = cN->next;
    }
    return false;
}

/**
 * Constructs an argument list with one element
 * @param symbol The argument symbol
 * @return
 */
SYMBOL_TABLE_NODE *createArgList(char *symbol) {
    SYMBOL_TABLE_NODE *p = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    if (!p) {
        yyerror("Out of memory");
    }
    p->ident = calloc(1 + strlen(symbol), sizeof(char));
    strcpy(p->ident, symbol);
    p->type = ARG_TYPE;
    return p;
}

/**
 * Adds a symbol to the front of an existing arg list
 * @param symbol
 * @param arg_list
 * @return
 */
SYMBOL_TABLE_NODE *addSymbolToArgList(char *symbol, SYMBOL_TABLE_NODE *arg_list) {
    if (arg_list == NULL) {
        return createArgList(symbol);
    }
    if (argInArgList(symbol, arg_list)) {
        return arg_list;
    }
    SYMBOL_TABLE_NODE *arg = createArgList(symbol);
    arg->next = arg_list;
    return arg;
}

/**
 * Creates a user defined function node.
 * @param type
 * @param ident
 * @param argList
 * @param body
 * @return
 */
SYMBOL_TABLE_NODE *createLambda(DATA_TYPE type, char *ident, SYMBOL_TABLE_NODE *argList, AST_NODE *body) {
    SYMBOL_TABLE_NODE *p = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    if (!p) {
        yyerror("Out of memory");
    }
    if (!body) {
        yyerror("No function body");
    }

    p->val_type = type;

    p->type = LAMBDA_TYPE;
    p->ident = calloc(strlen(ident) + 1, sizeof(char));
    strcpy(p->ident, ident);

    body->symbolTable = addSymbolToList(body->symbolTable, argList);
    p->val = body;
    return p;
}
