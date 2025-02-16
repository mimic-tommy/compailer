#ifndef _PARSE_TYPE_
#define _PARSE_TYPE_

#include "sharetype.h"

typedef enum
{
    TK_RESERVED, /* symbol */
    TK_IDENT,    /* identifer */
    TK_NUM,      /* number */
    TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind;  /* token type */
    Token *pNextTok; /* next token */
    int iVal;        /* value use only when kind is TK_NUM */
    char *pcStr;     /* token string */
    DWORD dwLen;     /* token length */
};

#endif
