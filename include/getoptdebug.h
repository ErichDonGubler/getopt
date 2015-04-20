/**
 * getoptdebug.h
 * Several debug functionsfor getopt that are useful and required too much thought to simply throw away.
 * Authors: Erich Gubler, erichdongubler@gmail.com
 */
#ifndef GETOPTDEBUG_H
#define GETOPTDEBUG_H

#include "getopt.h"

#include <iostream>

void printGetOptHelper(std::ostream& os){}
void printGetOptHelper(std::ostream& os, const getopt::config& c)
{
	os << '<';
	switch(c)
	{
		case getopt::config::caseInsensitive:
			os << "caseInsensitive";
			break;		
		case getopt::config::caseSensitive:
			os << "caseSensitive";
			break;		
		case getopt::config::passThrough:
			os << "passThrough";
			break;
		case getopt::config::noPassThrough:
			os << "noPassThrough";
			break;
		case getopt::config::bundling:
			os << "bundling";
			break;
		case getopt::config::noBundling:
			os << "noBundling";
			break;
		case getopt::config::required:
			os << "required";
			break;		
		case getopt::config::keepEndOfOptions:
			os << "keepEndOfOptions";
			break;		
		case getopt::config::stopOnFirstNonOption:
			os << "stopOnFirstNonOption";
			break;					
		default:
			os << "UNKNOWN_CONFIG_OPT";
			break;
	}
	os << ">; ";
}

template<typename T, typename...Args>
void printGetOptHelper(std::ostream& os, const std::string& s, const T& t, Args&&...args)
{
	os << "\"" << s << "\": " << t << "; ";
	printGetOptHelper(os, args...);
}

template<typename T, typename...Args>
void printGetOptHelper(std::ostream& os, const char* c, const T& t, Args&&...args)
{
	printGetOptHelper(os, std::string(c), t, args...);
}

template<typename...Args>
void printGetOptHelper(std::ostream& os, const getopt::config& c, Args&&...args)
{
	printGetOptHelper(os, c);
	printGetOptHelper(os, args...);
}

template<typename...Args>
void printGetOptArgs(std::ostream& os, const char* prepend, Args&&...args)
{
	os << prepend << ": { ";
	printGetOptHelper(os, args...);
	os << "}" << std::endl;
}

template<typename...Args>
void printGetOptArgs(std::ostream& os, const std::string& prepend, Args&&...args)
{
	os << prepend << ": { ";
	printGetOptHelper(os, args...);
	os << "}" << std::endl;
}

//REMOVE: Debug functions
void variadicAddresses(){}
template<typename...Ts>
void variadicAddresses(getopt::config& c, Ts&&...ts)
{
	std::cout << "<config>; ";
}
template<typename T, typename...Ts>
void variadicAddresses(T& t, Ts&&...ts)
{
	std::cout << typeid(T).name() << ": " << t << ", address = " << &t << "; ";
	variadicAddresses(ts...);
}

#endif