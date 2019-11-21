#include <stdio.h>

#include "lexer.c"

DECLARE_CONTAINER(Token, Token);
extern TokenIterator *token_input;

void preprocessor() {
    lex_generator *lexer = create_lex_generator();
    while (!lex_generator_eof(lexer)) {
        Token t = lex(lexer);
        switch (t.kind) {
            case TOKEN_IDENTIFIER: ;
        }
        // yield (t);
    }
    // yield_eof;
}

void parser() {
    while (!Token_eof(token_input)) {
        Token t = Token_next(token_input);
        // ...
        printf("%s", t.lexeme);
    }
}
