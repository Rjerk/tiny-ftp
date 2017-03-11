#include "error.h"

void error_Msg(const std::string& msg)
{
    std::cerr << msg << std::endl;
    return ;
}

void error_Exit(const std::string& msg)
{
    std::cerr << msg << std::endl;
    exit(1);
}
