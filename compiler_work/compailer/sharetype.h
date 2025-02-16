/* token , node ,Vlen */
#ifndef _SHARE_TYPE_
#define _SHERE_TYPE_

#define FALSE (0)
#define TRUE (1)
typedef unsigned int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef enum
{
    ND_ADD,    /* add (+) */
    ND_SUB,    /* subtraction (-) */
    ND_MUL,    /* multiplication (*) */
    ND_DIV,    /* division (/) */
    ND_ASSIGN, /* assign (=) */
    ND_EQL,    /* ==(!=) */
    ND_IDENT,  /* <> */
    ND_VAR,    /* local variable */
    ND_NUM     /* number */
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;    /* node type */
    Node *pLefthand;  /* lefthand side */
    Node *pRighthand; /* righthand side  */
    int iVal;         /* only use with kind being ND_NUM */
    DWORD dwOffset;   /* only use with kind being ND_VAR  */
                      /* offset is a member that represents the offset from the base pointer of a local variable. */
};

typedef struct LocalVar LocalVar;

struct LocalVar
{
    LocalVar *pNext; /* next local variable or null */
    char *pcName;    /* name of local variable */
    DWORD dwLen;     /* name length */
    DWORD dwOffset;  /* offset from  */
};
#endif
