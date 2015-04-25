/**
 * Demo: help.cpp
 * Purpose: Demonstrates using builtin help features for getopt.h
 * Authors: Erich Gubler, erichdongubler@gmail.com
 * Date: 04-25-2015
 */
#include "getopt.h"
#include <iostream>
using namespace std;
using namespace GetOpt;

int main(int argc, char** argv)
{
	int number;
	string stuff;

	try
	{
		auto results = getopt(argc, argv,
				"n|number", "gimme a number", &number,
				"s|stuff", &stuff
			);
		if(results.result.helpWanted)
		{
			defaultGetoptPrinter("Usage: ", results.result.options);
			return 0;
		}
	}
	catch(GetOptException& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	cout << "number: " << number << endl;
	cout << "stuff: " << stuff << endl;
	return 0;
}