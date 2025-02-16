#include "sharetype.h"
#include "codegen.h"
#include "error_main.h"
#include "parse.h"
#include "9cc.h"

int main(int argc ,char **argv)
{
    if(argc !=2)
    {
        Error_TermArgument();
        return 1;
    }
    char* user_input = argv[1];

    MakeHeadTemplate();
    CodeGen(Parse(user_input));
    MakeTaleTemplate();
    
    return 0;
}