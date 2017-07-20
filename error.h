#ifndef _ERROR_H_
#define _ERROR_H_

#include <string>
#include <iostream>
#include <cstdlib>

namespace ftp {

void error_Exit(const std::string&);
void error_Msg(const std::string&);

}

#endif
