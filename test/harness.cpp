/**
 * basictest.cpp
 * A fairly comprehensive test of the getopt options and parameter types.
 * TODO: not comprehensive yet!
 * Authors: Erich Gubler, erichdongubler@gmail.com
 */
#include "../include/getopt.h"
#include "../include/getoptdebug.h"
#include "cpputils/printalgorithms.h"

#include <stdexcept>
using std::logic_error;
#include <vector>
#include <iostream>

#include <algorithm>
#include <iterator>

using namespace std;

void _assert(bool test, const string& msg)
{
	if(!test)
		throw logic_error(msg);
}

template<typename...GetOptArgs>
void _test(bool expectingError, const string& msg, vector<string> args, GetOptArgs&&...getoptargs)
{
	bool success = (expectingError ? false : true);
	cout << "args: " << print_range<vector<string>>(args) << endl;
	try
	{
		getopt::getopt(args, getoptargs...);
	}
	catch(getopt::GetOptException& e)
	{
		cout << "****Error: " << e.what() << endl;
		success = (expectingError ? true : false);
	}
	printGetOptArgs(cout, "\t**getopt args", getoptargs...);
	cout << "\t**args after: " << print_range<vector<string>>(args) << endl;
	_assert(success, msg);
}

void testheader(const string& s) { cout << "==== " << s << " TEST ====" << endl; }

void testbool()
{
	testheader("BOOL");
	bool b = false;
	_test(false, "test"
		, {"this.exe", "-b", "true"}
		, getopt::config::required
		, "blarg|b", b, getopt::config::noPassThrough
	);
	b = false;
	_test(false, "test"
		, {"this.exe", "-btrue"}
		, getopt::config::required
		, "blarg|b", b
	);
	b = false;
	_test(false, "test"
		, {"this.exe", "--blarg", "true"}
		, getopt::config::required
		, "blarg|b", b
	);
	b = false;
	_test(false, "test"
		, {"this.exe", "--blarg=true"}
		, getopt::config::required
		, "blarg|b", b
	);
	b = false;
	_test(false, "test"
		, {"this.exe", "--blarg"}
		, getopt::config::required
		, "blarg|b", b
	);
	b = false;
	_test(true, "test"
		, {"this.exe"}
		, getopt::config::required
		, "blarg|b", b
	);
}

void teststring()
{
	testheader("STRING");
	string s;
	_test(false, "test"
		, {"this.exe", "meh", "-a", "bar"}
		, getopt::config::required
		, "asdf|a", s
	);
	s = "";
	_test(false, "test"
		, {"this.exe", "-afoo"}
		, getopt::config::required
		, "asdf|a", s
	);
	s = "";
	_test(false, "test"
		, {"this.exe", "--asdf", "true"}
		, getopt::config::required
		, "asdf|a", s
	);
	s = "";
	_test(false, "test"
		, {"this.exe", "--asdf=true"}
		, getopt::config::required
		, "asdf|a", s
	);
	s = "";
	_test(true, "test"
		, {"this.exe", "--asdf"}
		, getopt::config::required
		, "asdf|a", s
	);
	s = "";
	_test(true, "test"
		, {"this.exe"}
		, getopt::config::required
		, "asdf|a", s
	);
}

int main(int argc, char** argv)
{
	testbool();
	teststring();

	return 0;
}