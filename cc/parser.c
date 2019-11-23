#include <stdio.h>

#include "lexer.c"

DECLARE_CONTAINER(Token, Token);
extern TokenIterator *token_input;

void preprocessor(void) {
    LexGenerator *lexer = LexGenerator_alloc();
    while (!LexGenerator_eof(lexer)) {
        Token t = Lex(lexer);
        switch (t.kind) {
            case TOKEN_IDENTIFIER: ;
        }
        // yield (t);
    }
    // yield_eof;
}

void parser(void) {
    while (!Token_eof(token_input)) {
        Token t = Token_next(token_input);
        // ...
        printf("%s", t.lexeme);
    }
}
