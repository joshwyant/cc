#include "lexer.h"

char *make_string(CharList *list);
Token make_token(TokenKind kind, CharList *buffer);

// Iterator function. Call multiple times to get a stream of tokens.
DEFINE_GENERATOR(/* Data type: */ Token, /* Name: */ Lex,
// State variables:
    char, t,
    Token, new_t,
    CharList *, buffer) {
  buffer = CharList_alloc();
  while (!Char_eof(input)) {
    // Skip over whitespace
    while (!Char_eof(input) && isspace(t = Char_next(input))) {
    }

    // Read identifiers
    if (isalpha(t) || t == '_') {
      while (isalpha(t) || isdigit(t) || t == '_') {
        CharList_add(buffer, t);
        if (Char_eof(input))
          goto make_ident;
        t = Char_next(input);
      }
    }
  make_ident:
    if (List_count((List *)buffer)) {
      yield (make_token(TOKEN_IDENTIFIER, buffer));
      continue;
    }
    if (isdigit(t)) {
      CharList_add(buffer, t);
      if (t == '0') {
      }
    }
  make_number:
    if (List_count((List *)buffer)) {
      yield (make_token(TOKEN_NUMBER, buffer));
      continue;
    }
  }
end:
  List_free((List *)buffer);
  yield_eof;
}

char *make_string(CharList *list) {
    if (!list) return NULL;
    size_t len = List_count((List *)list);
    char *string_buffer = calloc(len + 1, sizeof(char));
    memcpy(string_buffer, CharList_get_data(list), len);
    List_clear((List *)list);
    return string_buffer;
}

Token make_token(TokenKind kind, CharList *buffer) {
    Token token = {kind, make_string(buffer)};
    return token;
}
