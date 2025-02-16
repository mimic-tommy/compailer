#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "parsetype.h"
#include "parse.h"
#include "error.h"

/* prototype */

static Token *parse_tokenize(char *pcStr);
static BOOL parse_readForwardOpt(const char *pcOpt);
static void parse_expectForwardOpt(const char *cOpt);
static int parse_expectForwardNum(void);
static BOOL parse_atEOF(void);
static Token *parse_newToken(TokenKind newKind, Token *pCurTok, char *pcNewStr);
static Node *parse_newNode(NodeKind newKind, Node *pNewLeft, Node *pNewRight);
static Node *parse_setNodeNum(int iNewVal);

/* sentence structure */

static void parse_program(void);         /* stmt (eof) */
static Node *parse_stmt(void);           /* expr ";" */
static Node *parse_expr(void);           /* assign */
static Node *parse_assign(void);         /* equality ("=" assign)? */
static Node *parse_equality(void);       /* relational ("==" relational | "!=" relational)* */
static Node *parse_rational(void);       /* add ("<" add | "<=" add | ">" add | ">=" add)* */
static Node *parse_additive(void);       /* mul ("+" mul | "-" mul)* */
static Node *parse_multiplicative(void); /* unary ("*" unary | "/" unary)* */
static Node *parse_urary(void);          /* ("+" | "-")? primary */
static Node *parse_primary(void);        /* num | ident | "(" expr ")" */

/* const options */

static const char *cs_colon = ";";
static const char *cs_assign = "=";
static const char *cs_equal = "==";
static const char *cs_nequal = "!=";
static const char *cs_less = "<";
static const char *cs_eless = "<=";
static const char *cs_more = ">";
static const char *cs_emore = ">=";
static const char *cs_plus = "+";
static const char *cs_minus = "-";
static const char *cs_astr = "*";
static const char *cs_slash = "/";
static const char *cs_lparen = "(";
static const char *cs_rparen = ")";

static Token *s_pCurTok;      /* Token currently focused on */
static Node *s_pCurNod;       /* Node currently focused on */
static LocalVar *s_pLocalVar; /* localval currently focused on */

Node *Parse(char *pcStr)
{
    s_pCurTok = parse_tokenize(pcStr);
    parse_program();
    return s_pCurNod;
}
/* --------------------Token-------------------- */

/* Tokenize a string passed from ahigher level and return a token pointer */
static Token *parse_tokenize(char *pcStr)
{
    Token head;
    head.pNextTok = NULL;
    Token *pCur = &head;

    while (*pcStr)
    {
        // space skip
        if (isspace(*pcStr))
        {
            pcStr++;
            continue;
        }
        if (*pcStr == '+' || *pcStr == '-')
        {
            pCur = parse_newToken(TK_RESERVED, pCur, pcStr++);
            continue;
        }
        if (isdigit(*pcStr))
        {
            pCur = parse_newToken(TK_NUM, pCur, pcStr);
            pCur->iVal = strtol(pcStr, &pcStr, 10);
            continue;
        }
        error("cannot tokenize");
    }
    parse_newToken(TK_EOF, pCur, pcStr);
    return head.pNextTok;
}
/*
When the next token is the expected symbol, read one token forward and
return true. Otherwise, return false.
cOpt : expected option
*/
static BOOL parse_readForwardOpt(const char *pcOpt)
{
    if (
        s_pCurTok->kind != TK_RESERVED ||
        strlen(pcOpt) != s_pCurTok->dwLen ||
        memcmp(s_pCurTok->pcStr, pcOpt, s_pCurTok->dwLen))
    {
        return FALSE;
    }
    s_pCurTok = s_pCurTok->pNextTok;
    return TRUE;
}
/*
If the next token is the expected symbol, read one token forward.
Otherwise, report an error.
cOpt : expected option
*/
static void parse_expectForwardOpt(const char *cOpt)
{
    if (s_pCurTok->kind != TK_RESERVED || strcmp(s_pCurTok->pcStr, cOpt))
    {
        error("not number", cOpt);
    }
    s_pCurTok = s_pCurTok->pNextTok;
}
/* If the next token is the expected symbol, read one token forward.
Otherwise, report an error.
*/
static int parse_expectForwardNum(void)
{
    if (s_pCurTok->kind != TK_NUM)
    {
        error("not num");
    }
    int iCurVal = s_pCurTok->iVal;
    s_pCurTok = s_pCurTok->pNextTok;
    return iCurVal;
}
/*
whether EOF
*/
static BOOL parse_atEOF(void)
{
    return (s_pCurTok->kind == TK_EOF);
}
/*
Create a new token and connect it to cur
*/
static Token *parse_newToken(TokenKind newKind, Token *pCurTok, char *pcNewStr)
{
    Token *pNewTok = calloc(1, sizeof(Token));
    pNewTok->kind = newKind;
    pNewTok->pcStr = pcNewStr;
    pCurTok->pNextTok = pNewTok;
    return pCurTok;
}

/* --------------------Node-------------------- */

/*
Create a new Node and connect it to cur
*/
static Node *parse_newNode(NodeKind newKind, Node *pNewLeft, Node *pNewRight)
{
    Node *pNewNod = calloc(1, sizeof(Node));
    pNewNod->kind = newKind;
    pNewNod->pLefthand = pNewLeft;
    pNewNod->pRighthand = pNewRight;
}
static Node *parse_setNodeNum(int iNewVal)
{
    Node *pNewNod = calloc(1, sizeof(Node));
    pNewNod->kind = ND_NUM;
    pNewNod->iVal = iNewVal;
    return pNewNod;
}

#define ARARY_LENGTH (100)
static Node *s_paCodes[ARARY_LENGTH];

void parse_program(void)
{
    int iIdx = 0;
    while (!parse_atEOF())
    {
        s_paCodes[iIdx++] = parse_stmt();
    }
    s_paCodes[iIdx] = NULL;
}
static Node *parse_stmt(void)
{
    Node *pNod = parse_expr();
    parse_expectForwardOpt(cs_colon);
    return pNod;
}
static Node *parse_expr(void)
{
    return parse_assign();
}
static Node *parse_assign(void)
{
    Node *pNod = parse_equality();
    if (parse_readForwardOpt(cs_assign))
    {
        pNod = parse_newNode(ND_ASSIGN, pNod, parse_assign());
    }
    return pNod;
}
static Node *parse_equality(void)
{
    Node *pNod = parse_rational();
    if (parse_readForwardOpt(cs_equal) | parse_readForwardOpt(cs_nequal))
    {
        pNod = parse_newNode(ND_IDENT, pNod, parse_equality());
    }
    return pNod;
}
static Node *parse_rational(void)
{
    Node *pNod = parse_additive();
    if (
        parse_readForwardOpt(cs_less) |
        parse_readForwardOpt(cs_eless) |
        parse_readForwardOpt(cs_more) |
        parse_readForwardOpt(cs_emore))
    {
        pNod = parse_rational();
    }
    return pNod;
}
static Node *parse_additive(void)
{
    Node *pNod = parse_multiplicative();
    if (parse_readForwardOpt(cs_plus) | parse_readForwardOpt(cs_minus))
    {
        pNod = parse_additive();
    }
    return pNod;
}
static Node *parse_multiplicative(void)
{
    Node *pNod = parse_urary();
    for (;;)
    {
        if (parse_readForwardOpt(cs_astr))
        {
            pNod = parse_newNode(ND_MUL, pNod, parse_multiplicative());
        }
        else if (parse_readForwardOpt(cs_slash))
        {
            pNod = parse_newNode(ND_DIV, pNod, parse_multiplicative());
        }
        else
        {
            return pNod;
        }
    }
}
static Node *parse_urary(void)
{
    if (parse_readForwardOpt(cs_plus))
    {
        return parse_primary();
    }
    if (parse_readForwardOpt(cs_minus))
    {
        return parse_newNode(ND_SUB, parse_setNodeNum(0), parse_primary());
    }
    return parse_primary();
}
static Node *parse_primary(void)
{
    if (parse_readForwardOpt(cs_lparen))
    {
        Node *pNod = parse_expr();
        parse_expectForwardOpt(cs_rparen);
        return pNod;
    }
    return parse_setNodeNum(parse_expectForwardNum());
}
static LocalVar *parse_findLocalVar(Token *pTok)
{
    for (LocalVar *var = s_pLocalVar; var; var = var->pNext)
    {
        if (var->dwLen = pTok->dwLen && !memcmp(pTok->pcStr, var->pcName, var->dwLen))
        {
            return var;
        }
    }
    return NULL;
}
