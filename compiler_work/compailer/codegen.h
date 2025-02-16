/* header for asm code generater codegen.c */
#ifndef  _CODEGEN_
#define _CODEGEN_

extern void MakeHeadTemplate(void);
extern void MakeTaleTemplate(void);

extern void CodeGen(Node *node);

#endif

