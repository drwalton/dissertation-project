#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>

class Exception 
{
public:
	Exception(const std::string& msg) {this->msg = msg;};
	std::string msg;
};

#endif
