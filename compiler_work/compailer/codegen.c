#include <stdio.h>
#include <stdlib.h>

#include "sharetype.h"
#include "codegen.h"
#include "error.h"
#define gen(node) CodeGen(node)

inline void MakeHeadTemplate(void)
{
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
}
inline void MakePlogTemplate(void)
{
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");
}
inline void MakeTaleTemplate(void)
{
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}
static void gen_lval(Node *node)
{
    if (node->kind != ND_VAR)
        error("The left-hand side value of the assignment is not a variable");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->dwOffset);
    printf("  push rax\n");
}
/* prototype */
void CodeGen(Node *node);
void CodeGen(Node *node)
{
    switch (node->kind)
    {
    case ND_NUM:
        printf("  push %d\n", node->iVal);
        return;
    case ND_VAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->pLefthand);
        gen(node->pRighthand);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->pLefthand);
    gen(node->pRighthand);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  imul rax, rdi\n");
        break;
    case '/':
        printf("  cqo\n");
        printf("  idiv rdi\n");
    }

    printf("  push rax\n");
}