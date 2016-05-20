/**
 * basictest.cpp
 * A fairly comprehensive test of the getopt options and parameter types.
 * TODO: not comprehensive yet!
 * Authors: Erich Gubler, erichdongubler@gmail.com
 */
#include "../include/getopt.h"
#include "../include/getoptdebug.h"
#include "../depends/cpputils/printalgorithms.h"

#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;
#include <functional> 
using std::function;
#include <sstream>
using std::ostringstream;
#include <string>
using std::string;
#include <stdexcept>
using std::logic_error;
#include <vector>
using std::vector;

template<typename...Ts>
void _fail_test(const string& testName, Ts&&...explanation_args)
{
	ostringstream error_message_stream;
	writeln(error_message_stream, testName, " -- ", explanation_args...);
	throw logic_error(error_message_stream.str());
}

void _print_test_header_(const string& testName)
{
	cout << "----" << testName << "----" << endl;
}

template<typename...GetOptArgs>
GetOpt::GetOptResult _run_getopt(vector<string>& args, GetOptArgs&&...getOptArgs)
{
	cout << "\t**args: " << range_printer(args.begin(), args.end()) << endl;
	GetOpt::printGetOptArgs(cout, "\t**getopt args", getOptArgs...);
	return GetOpt::getopt(args, getOptArgs...);
}

typedef std::function<void()> SetUpFunction;
typedef std::function<void(const string&)> TestValuesFunction;
template<typename...GetOptArgs>
void _test_success(const string& testName, vector<string> args, const vector<string>& expectedArgsAfter, SetUpFunction setUp, TestValuesFunction testParsedValues, GetOptArgs&&...getOptArgs)
{
	setUp();
	_print_test_header_(testName);
	try
	{
		auto result = _run_getopt(args, getOptArgs...);
	}
	catch(GetOpt::GetOptException& e)
	{
		cout << "****GetOpt Error: " << e.what() << endl;
	}
	cout << "\t**after: " << range_printer(args.begin(), args.end()) << endl;
	cout.flush();
	if(args != expectedArgsAfter)
	{
		_fail_test("expected args after getopt call to be \"", range_printer(expectedArgsAfter.begin(), expectedArgsAfter.end()));//, expectedArgsAfter.end()), "\", but is \"", range_printer(args.begin(), args.end()), "\"");
	}
	testParsedValues(testName);
}

template<typename...GetOptArgs>
void _test_failure(const string& testName, vector<string> args, GetOptArgs&&...getOptArgs)
{
	_print_test_header_(testName);
	try
	{
		auto result = _run_getopt(args, getOptArgs...);
	}
	catch(GetOpt::GetOptException& e)
	{
		cout << "\t**Caught expected error: " << e.what() << endl;
		return;
	}
	throw logic_error(testName);
}

void testheader(const string& s) { cout << "==== " << s << " TEST ====" << endl; }

void testbool()
{
	testheader("BOOL");
	bool b = false;
	SetUpFunction reset_bool = [&]() -> void
	{
		b = false;
	};
	auto check_bool_set = [&](bool expectedValue) -> TestValuesFunction
	{
		TestValuesFunction f = [&b, expectedValue](const string& testName)
		{
			if(b != expectedValue)
			{
				_fail_test(testName, "boolean value ", b, " does not match expected ", expectedValue);
			}
		};
		return f;
	};
	// Short opt tests
	_test_success("Bool: short opt parse"
		, {"this.exe", "-b"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b, GetOpt::config::noPassThrough
	);
	_test_success("Bool: parse only first arg with short opt (true)"
		, {"this.exe", "-b", "true"}
		, {"this.exe", "true"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b, GetOpt::config::noPassThrough
	);
	_test_success("Bool: parse only first arg with short opt (false)"
		, {"this.exe", "-b", "false"}
		, {"this.exe", "false"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b
	);
	_test_success("Bool: short true specified"
		, {"this.exe", "-btrue"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b
	);
	_test_success("Bool: short false specified"
		, {"this.exe", "-bfalse"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(false)
		, "blarg|b", &b
	);

	// Long opt tests
	_test_success("Bool: long opt parse"
		, {"this.exe", "--blarg"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b
	);
	_test_success("Bool: parse only first arg with long opt"
		, {"this.exe", "--blarg", "true"}
		, {"this.exe", "true"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b
	);
	_test_success("Bool: parse long opt with equals (true)"
		, {"this.exe", "--blarg=true"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(true)
		, "blarg|b", &b
	);
	_test_success("Bool: parse long opt with equals (false)"
		, {"this.exe", "--blarg=false"}
		, {"this.exe"}
		, reset_bool
		, check_bool_set(false)
		, "blarg|b", &b
	);
	_test_failure("Bool: fail required (no args)"
		, {"this.exe"}
		, GetOpt::config::required
		, "blarg|b", &b
	);
}

void teststring()
{
	testheader("STRING");
	string s = "";
	SetUpFunction reset_string = [&]() -> void
	{
		s = "";
	};
	auto check_string_set = [&](string expectedValue) -> TestValuesFunction
	{
		TestValuesFunction f = [&s, expectedValue](const string& testName)
		{
			if(s != expectedValue)
			{
				_fail_test(testName, "string \"", s, "\" does not match expected \"", expectedValue, "\"");
			}
		};
		return f;
	};
	_test_success("String: parse short opt with second arg"
		, {"this.exe", "meh", "-a", "bar"}
		, {"this.exe", "meh"}
		, reset_string
		, check_string_set("bar")
		, GetOpt::config::required
		, "asdf|a", &s
	);
	_test_success("String: parse short opt with content in first arg"
		, {"this.exe", "-afoo"}
		, {"this.exe"}
		, reset_string
		, check_string_set("foo")
		, GetOpt::config::required
		, "asdf|a", &s
	);
	_test_success("String: parse long opt with second arg"
		, {"this.exe", "--asdf", "true"}
		, {"this.exe"}
		, reset_string
		, check_string_set("true")
		, GetOpt::config::required
		, "asdf|a", &s
	);
	_test_success("String: parse long opt with equals (one arg)"
		, {"this.exe", "--asdf=true"}
		, {"this.exe"}
		, reset_string
		, check_string_set("true")
		, GetOpt::config::required
		, "asdf|a", &s
	);
	_test_failure("String: fail required string with dangling arg"
		, {"this.exe", "--asdf"}
		, GetOpt::config::required
		, "asdf|a", &s
	);
	_test_failure("String: fail required string with no args at all"
		, {"this.exe"}
		, GetOpt::config::required
		, "asdf|a", &s
	);
}

int main(int argc, char** argv)
{
	testbool();
	teststring();

	cout << "Test harness complete." << endl;
	return 0;
}