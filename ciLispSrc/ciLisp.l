/**
* Name: Jose de Jesus Rodriguez Rivas
* Lab: ciLisp
* Date: 04/24/19
**/
%option noyywrap
%option nounput
%option noinput

%{
    #include "ciLisp.h"
%}

digit [0-9]
integer_number [+-]?{digit}+
real_number {integer_number}\.{digit}+
func "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"|"exp2"|"cbrt"|"hypot"|"sin"|"cos"|"tan"|"print"|"read"|"rand"|"equal"|"smaller"|"larger"
letter [a-zA-Z]
symbol {letter}+
%%

{real_number} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: NUMBER dval = %lf\n", yylval.dval);
    return REAL_NUMBER;
    }

{integer_number} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: NUMBER dval = %lf\n", yylval.dval);
    return INTEGER_NUMBER;
    }

"cond" {
    fprintf(stderr, "lex: COND\n");
    return COND;
    }

"quit"|"exit"|"QQ"|"/ff" {
    fprintf(stderr, "lex: QUIT\n");
    return QUIT;
    }

"let" {
    fprintf(stderr, "lex: LET\n");
    return LET;
    }

"integer" {
    fprintf(stderr, "lex: INTEGER\n");
    return INTEGER;
    }

"real" {
    fprintf(stderr, "lex: REAL\n");
    return REAL;
    }

"lambda" {
    fprintf(stderr, "lex: LAMBDA\n");
    return LAMBDA;
    }

{func} {
    yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: FUNC sval = %s\n", yylval.sval);
    return FUNC;
    }

{symbol} {
    yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: SYMBOL sval = %s\n", yylval.sval);
    return SYMBOL;
    }

"(" {
    fprintf(stderr, "lex: LPAREN\n");
    return LPAREN;
    }

")" {
    fprintf(stderr, "lex: RPAREN\n");
    return RPAREN;
    }

[\n] {
    fprintf(stderr, "lex: EOL\n");
    YY_FLUSH_BUFFER;
    return EOL;
    }

[ \t] {}; /* skip whitespace */

. { // anything else
    printf("ERROR: invalid character: >>%s<<\n", yytext);
    }

%%

/*
 * DO NOT CHANGE THE FOLLOWING CODE CRITICAL FOR REPL!
 */
int main(void) {

       freopen("/dev/null", "w", stderr); // except for this line that can be uncommented to throw away debug printouts

    char *s_expr_str = NULL;
    size_t s_expr_str_len = 0;
    YY_BUFFER_STATE buffer;
    while (true) {
        printf("\n> ");
        getline(&s_expr_str, &s_expr_str_len, stdin);
        s_expr_str[s_expr_str_len++] = '\0';
        s_expr_str[s_expr_str_len++] = '\0';
        buffer = yy_scan_buffer(s_expr_str, s_expr_str_len);
        yyparse();
        yy_delete_buffer(buffer);
    }
}
