#ifndef CC_LEXER_H__
#define CC_LEXER_H__

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../common/public/generator.h"
#include "../common/public/list.h"

enum TokenKind {
    TOKEN_NONE,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
};

typedef enum TokenKind TokenKind;

extern CharIterator *input;

struct Token {
    TokenKind kind;
    char *lexeme;
};
typedef struct Token Token;

DECLARE_GENERATOR(Token, Lex)

#endif // CC_LEXER_H__
