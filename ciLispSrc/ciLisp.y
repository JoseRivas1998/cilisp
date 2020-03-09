/**
* Name: Jose de Jesus Rodriguez Rivas
* Lab: ciLisp
* Date: 04/24/19
**/
%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolNode;
}

%token <sval> FUNC SYMBOL
%token <dval> REAL_NUMBER INTEGER_NUMBER
%token LPAREN RPAREN EOL QUIT LET COND LAMBDA INTEGER REAL

%type <astNode> s_expr s_expr_list
%type <symbolNode> let_elem let_section let_list arg_list

%%

program:
	s_expr EOL {
	fprintf(stderr, "yacc: program ::= s_expr EOL\n");
	if ($1) {
	    RETURN_VALUE val = eval($1);
	    printf("%.2lf", val.value);
	    freeNode($1);
	}
	};

s_expr:
	REAL_NUMBER {
	fprintf(stderr, "yacc: s_expr ::= NUMBER\n");
	$$ = real_number($1);
	}
	| INTEGER_NUMBER {
	$$ = int_number($1);
	}
	| SYMBOL {
	fprintf(stderr, "yacc: SYMBOL = %s\n", $1);
	$$ = symbol($1);
	}
	| LPAREN FUNC s_expr_list RPAREN {
	fprintf(stderr, "yacc: LPAREN FUNC{%s} s_expr_list RPAREN\n", $2);
	$$ = function($2, $3);
	}
	| LPAREN COND s_expr s_expr s_expr RPAREN {
	fprintf(stderr, "yacc: LPAREN COND s_expr s_expr s_expr RPAREN\n");
	$$ = conditional($3, $4, $5);
	}
	| LPAREN let_section s_expr RPAREN {
	$$ = setSymbolTable($2, $3);
	}
	| LPAREN SYMBOL s_expr_list RPAREN {
	fprintf(stderr, "LPAREN SYMBOL{%s} s_expr_list RPAREN\n", $2);
	$$ = function($2, $3);
	}
	| QUIT {
	fprintf(stderr, "yacc: s_expr ::= QUIT\n");
	exit(EXIT_SUCCESS);
	}
	| LPAREN s_expr RPAREN {
	$$ = $2;
	}
	| error {
	fprintf(stderr, "yacc: s_expr ::= error\n");
	yyerror("unexpected token");
	$$ = NULL;
	};
s_expr_list:
	/*empty*/{
		$$ = NULL;
	}
	| s_expr s_expr_list {
	fprintf(stderr, "yacc: s_expr s_expr_list\n");
	$$ = addNodeToList($1, $2);
	}
	| s_expr {
	fprintf(stderr, "yacc: s_expr {s_expr_list}\n");
	$$ = $1;
	}

let_section:
	/*empty*/{
		$$ = NULL;
	}
	| LPAREN let_list RPAREN {
		$$ = $2;
	};
let_list:
	let_list let_elem {
		$$ = addSymbolToList($1, $2);
	}
	| LET let_elem {
		fprintf(stderr, "yacc: LET let_elem\n");
		$$ = $2;
	};
let_elem:
	LPAREN INTEGER SYMBOL s_expr RPAREN {
		fprintf(stderr, "yacc: LPAREN INTEGER SYMBOL{%s} s_expr RPAREN\n", $3);
		$$ = createSymbol(INTEGER_TYPE, $3, $4);
	}
	| LPAREN REAL SYMBOL s_expr RPAREN {
		fprintf(stderr, "yacc: LPAREN REAL SYMBOL{%s} s_expr RPAREN\n", $3);
		$$ = createSymbol(REAL_TYPE, $3, $4);
	}
	| LPAREN SYMBOL s_expr RPAREN {
		fprintf(stderr, "LPAREN SYMBOL{%s} s_expr RPAREN\n", $2);
		$$ = createSymbol(NO_TYPE, $2, $3);
	}
	| LPAREN INTEGER SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
		//fprintf(stderr, "yacc: LPAREN INTEGER SYMBOL{%s} s_expr RPAREN\n", $3);
		$$ = createLambda(INTEGER_TYPE, $3, $6, $8);
	}
	| LPAREN REAL SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
		//fprintf(stderr, "yacc: LPAREN INTEGER SYMBOL{%s} s_expr RPAREN\n", $3);
		$$ = createLambda(REAL_TYPE, $3, $6, $8);
	}
	| LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
		//fprintf(stderr, "yacc: LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
		$$ = createLambda(NO_TYPE, $2, $5, $7);
	}
arg_list:
	SYMBOL arg_list {
		fprintf(stderr, "yacc: SYMBOL{%s} ARG_LIST\n", $1);
		$$ = addSymbolToArgList($1, $2);
	}
	| SYMBOL {
		fprintf(stderr, "yacc: SYMBOL{%s} arg_list\n", $1);
		$$ = createArgList($1);
	}
%%

