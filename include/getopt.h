/**
 * getopt.h
 * Provides GNU-compliant getopt interface
 * Authors: Erich Gubler, erichdongubler@gmail.com
 * Date: 4/17/2015
 */
#ifndef GETOPT_H
#define GETOPT_H

// Core functionality
#include <iostream>// ostream for help printing
#include <string>
#include <vector>
#include <unordered_set> // opts matching
#include <sstream> // conversions between most types
#include <stdexcept> // runtime_error
#include <type_traits> // is_integral
#include <typeinfo> // use typeid(...) for error message for conversions
#include <utility> // move

// Case conversion
#include <algorithm> // transform
#include <cctype> // tolower

// String case conversions
#include <cctype> // tolower
#include <algorithm> // transform

#include <tuple> // Tuple return for C++-style convenience interface

namespace getopt
{

	class GetOptException : public std::runtime_error
	{
	public:
		GetOptException(const std::string& s) : std::runtime_error(s){}
	};

	// Conforms to [D's config values](http://dlang.org/phobos/std_getopt.html#.config)
	enum class config
	{
		caseSensitive,
		caseInsensitive,
		passThrough,
		noPassThrough,
		bundling,
		noBundling,
		stopOnFirstNonOption,
		keepEndOfOptions,
		required,
	};

	class GetOptConfiguration
	{
	public:
		bool passThrough = false;
		bool bundling = false;
		bool required = false;
		bool keepEndOfOptions = false;
		bool caseSensitive = false;
		bool stopOnFirstNonOption = false;

		void set(config configOption)
		{
			switch(configOption)
			{
				case getopt::config::caseInsensitive:
					this->caseSensitive = true;
					break;
				case getopt::config::caseSensitive:
					this->caseSensitive = true;
					break;
				case getopt::config::passThrough:
					this->passThrough = true;
					break;
				case getopt::config::noPassThrough:
					this->passThrough = false;
					break;
				case getopt::config::bundling:
					this->bundling = true;
					break;
				case getopt::config::noBundling:
					this->bundling = false;
					break;
				case getopt::config::required:
					this->required = true;
					break;
				case getopt::config::keepEndOfOptions:
					this->keepEndOfOptions = true;
					break;
				case getopt::config::stopOnFirstNonOption:
					this->stopOnFirstNonOption = true;
					break;
			}
		}
	};

	class Option
	{
	private:
		Option(auto&& sos, auto&& los) : shortOpts(sos), longOpts(los){}
	public:
		bool isIncremental = false;
		std::unordered_set<char> shortOpts;
		std::unordered_set<std::string> longOpts;

		Option(std::string optSpec)
		{
			// std::cout << "optSpec: " << optSpec << std::endl;
			auto specSize = optSpec.size();
			if(specSize == 0)
				throw std::logic_error("Spec size must be greater than 0");// Currently, D's getopt crashes on this case
			// Check if incremental opt
			if(optSpec[specSize - 1] == '+')
			{
				isIncremental = true;
				optSpec.erase((specSize--) - 1);
			}

			decltype(this->shortOpts) shortOpts;
			decltype(this->longOpts) longOpts;
			// Split into long and short opts
			std::size_t found = optSpec.find('|');
			do
			{
				auto optMatch = optSpec.substr(0, found);
				switch(optMatch.size())
				{
					case 0:
						break;
					case 1:
						shortOpts.insert(optMatch[0]);
						break;
					default:
						longOpts.insert(optMatch);
						break;
				}
				if(found != std::string::npos)
				{
					optSpec = optSpec.substr(found + 1);
					found = optSpec.find('|');
				}
				else
					break;
			}
			while(true);
			this->shortOpts = std::move(shortOpts);
			this->longOpts = std::move(longOpts);
		}
	};

	struct GetOptResult
	{
	public:
		std::vector<Option> options;
		bool helpWanted;

		// Nonstandard
		bool parsing = true;

		GetOptResult() = default;
		// TODO: Define move semantics
		// Problem: Was getting error with the following commented out:
		// GetOptResult(GetOptResult&& r)
		// 	: options(std::move(r.options))
		// 	, helpWanted(r.helpWanted)
		// 	, parsing(r.parsing)
		// {}
		// GetOptResult operator=(GetOptResult&& r)
		// {
		// 	this->options = std::move(r.options);
		// 	this->helpWanted = r.helpWanted;
		// 	this->parsing = r.parsing;
		// }
	};

	// Assignments from captured flags

	template<typename T>
	void throwConversionException(const std::string& s)
	{
		throw GetOptException("\"" + s + "\" is not convertible to type " + typeid(T).name());//TODO: Way to get user-friendly type name? typeid(...)?
	}

	// getoptassign is used to handle conversions from the string form to the desired type

	template<typename T>
	void getoptassign(T& t, const std::string& s)
	{
		std::stringstream ss(s);
		ss >> t;
		if(!ss || ss.peek() != EOF)
			throwConversionException<T>(s);
	}

	template<>
	void getoptassign<std::string>(std::string& t, const std::string& s)
	{
		t = s;
	}

	template<>
	void getoptassign<bool>(bool& t, const std::string& s)
	{
		auto scopy = s;
		transform(scopy.begin(), scopy.end(), scopy.begin(), ::tolower);
		if(scopy != "true" && scopy != "false")
			throwConversionException<bool>(scopy);
		t = (scopy == "true");
	}

	enum class FlagType
	{
		// All non-solitary flags have content associated with them
		NONE,
		SHORT_SOLITARY,
		SHORT,// Could be a bundled set of shorts
		LONG_SOLITARY,
		LONG,
	};

	// Mutates @arg to the matched opt string
	// If content is found, replaces @optContent
	FlagType flagType(std::string& arg, std::string& optContent, GetOptConfiguration& config)
	{
		FlagType type = FlagType::NONE;
		auto argSize = arg.size();
		if(argSize >= 2 && arg[0] == '-')// Must be an opt
		{
			if(arg[1] == '-')// Long opt
			{
				auto equalsPos = arg.find('=');
				auto foundEquals = (equalsPos != std::string::npos);
				auto opt = arg.substr(2, (foundEquals ? equalsPos - 2 : std::string::npos));
				if(equalsPos != std::string::npos)
				{
					type = FlagType::LONG;
					optContent = arg.substr(equalsPos + 1);
				}
				else
					type = FlagType::LONG_SOLITARY;
				arg = std::move(opt);
			}
			else// Short opt
			{
				if(argSize != 2)// "-oSTUFF" or bundled shorts
				{
					// Non-bundling: assume the rest as the opt input
					//TODO Bundling branch
					type = FlagType::SHORT;
					optContent = arg.substr(2);
				}
				else// Otherwise we have ["-o", "STUFF"]
					type = FlagType::SHORT_SOLITARY;
				arg = std::move(arg[1]);
			}
		}
		return type;
	}

	// SolitaryOptHandle is used to handle types where an opt by itself makes sense.
	// This is either bool or an incremental ('+' at end) opt (which should be an int).

	template<class T, class Enable = void>
	class SolitaryOptHandle
	{
	public:
		static bool handle(T& t, bool isIncremental)
		{
			return isIncremental;// This is behavior from D's getopt -- '+' at the end of non-integral types seems undefined and "handles" the opt
		}
	};
	 
	template<class T>
	class SolitaryOptHandle<T, typename std::enable_if<std::is_integral<T>::value >::type>
	{
	public:
		static bool handle(T& t, bool isIncremental)
		{
			if(isIncremental)
				++t;
			return isIncremental;
		}
	};

	template<>
	class SolitaryOptHandle<bool, typename std::enable_if<true>::type>
	{
	public:
		static bool handle(bool& t, bool isIncremental)
		{
			t = true;
			return true;
		}
	};

	// Exit case
	void getopthelper(std::vector<std::string>& args, decltype(args.size())& argsLimit, GetOptConfiguration& config, GetOptResult& result)
	{
		if(!config.passThrough)
		{
			for(auto i = 0; i < argsLimit; ++i)
			{
				std::string arg = args[i], content;
				switch(flagType(arg, content, config))
				{
					case FlagType::NONE:
						break;
					default:
						throw GetOptException("Unrecognized option " + args[i]);
				}
			}
		}
	}

	// config value
	template<typename...Ts>
	void getopthelper(std::vector<std::string>& args, decltype(args.size())& argsLimit, GetOptConfiguration& configuration, GetOptResult& result, config configOption, Ts&&...ts)
	{
		configuration.set(configOption);
		getopthelper(args, argsLimit, configuration, result, ts...);
	}

	// Optspec/variable pair
	template<typename T, typename...Ts>
	void getopthelper(std::vector<std::string>& args, decltype(args.size())& argsLimit, GetOptConfiguration& config, GetOptResult& result, const std::string& optSpec, T& t, Ts&&...ts)
	{
		// Construct the opt
		Option option(optSpec);
		bool foundFlag = false;
		if(result.parsing)// XXX: Does D return a complete Option[] after stopping?
		{
			std::string optContent;
			for(size_t argsIndex = 0; argsIndex < argsLimit;)// Flag find loop
			{
				std::string arg = args[argsIndex];
				// std::cout << "arg: " << arg << std::endl;
				if(!config.caseSensitive)
					std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

				bool foundHere = false;
				bool expectingContentNext = false;
				auto type = flagType(arg, optContent, config);
				switch(type)// Remember: arg and optContent are mutated!
				{
					case FlagType::NONE:
						if(config.stopOnFirstNonOption)
						{
							result.parsing = false;
							goto END_PARSE;
						}
						break;
					case FlagType::SHORT_SOLITARY:
					case FlagType::LONG_SOLITARY:
						expectingContentNext = true;
					case FlagType::LONG:
					case FlagType::SHORT:
						switch(type)
						{
							case FlagType::LONG:
							case FlagType::LONG_SOLITARY:
								foundHere = foundFlag = option.longOpts.count(arg);
								break;
							case FlagType::SHORT:
							case FlagType::SHORT_SOLITARY:
								foundHere = foundFlag = option.shortOpts.count(arg[0]);
								break;
						}
						break;
					default:
						throw std::logic_error("Uncaught flag type");
				}
				// Handle find result and shift; we'll handle the args index shift ourselves
				if(foundHere)
				{
					auto shiftArgs = [&]()
					{
						args.erase(args.begin() + argsIndex);
						--argsLimit;
					};
					shiftArgs();
					if(expectingContentNext)// Haven't gotten the content yet
					{
						expectingContentNext = false;
						if(SolitaryOptHandle<T>::handle(t, option.isIncremental))
							continue;
						else
						{
							if(argsLimit <= argsIndex)
								throw GetOptException("Expected input after option " + arg);
							optContent = args[argsIndex];
							shiftArgs();
						}
					}
					getoptassign(t, optContent);
					continue;
				}
				else
					++argsIndex;// Do the shift, since we took nothing
			}
		}
		END_PARSE:
		if(!foundFlag && config.required)
			throw GetOptException("Required option " + optSpec + " was not supplied");
		result.options.push_back(option);
		getopthelper(args, argsLimit, config, result, ts...);
	}


	template<typename T, typename...Ts>
	void getopthelper(std::vector<std::string>& args, decltype(args.size())& argsLimit, GetOptConfiguration& config, GetOptResult& result, const char* opt, T& t, Ts&&...ts)
	{
		getopthelper(args, argsLimit, config, result, std::string(opt), t, ts...);
	}

	bool findTerminatorIndex(std::vector<std::string>& args, decltype(args.size())& argsLimit)//XXX: Inline?
	{
		for(auto i = 0; i < args.size(); ++i)
			if(args[i] == "--")
			{
				argsLimit = i;
				break;
			}
	}

	// These are almost certainly the functions you want as an end user.

	void defaultGetoptPrinter(std::ostream& os, const std::string& message, std::vector<Option> options)
	{
  		os << message << std::endl;
		//TODO: Implement me! And make the format string customizable too!
		// D pseudocode of implementation necessary:
		/*foreach(it; opt)
		{
		    writefln("%*s %*s %s", lengthOfLongestShortOption, it.optShort,
		        lengthOfLongestLongOption, it.optLong, it.help);
		}*/
	}

	void defaultGetoptPrinter(const std::string& message, std::vector<Option> options)
	{
		defaultGetoptPrinter(std::cout, message, options);
	}

	template<typename...Args, bool = false>
	GetOptResult getopt(std::vector<std::string>& args, Args&&...getoptargs)
	{
		GetOptResult result;
		GetOptConfiguration config;
		auto argsLimit = args.size();
		bool hasTerminator = findTerminatorIndex(args, argsLimit);
		getopthelper(args, argsLimit, config, result, getoptargs...);
		if(hasTerminator && !config.keepEndOfOptions)
			args.erase(args.begin() + argsLimit);
		return result;
	}

	using GetOptResultTuple = std::tuple<GetOptResult, std::vector<std::string>>;
	template<typename...Args, bool = false>
	GetOptResultTuple getopt(int argc, char** argv, Args&&...getoptargs)
	{
		std::vector<std::string> args(argc);
		for(auto i = 0; i < argc; ++i)
			args[i] = argv[i];
		return make_tuple(getopt(args, getoptargs...), std::move(args));
	}
};

#endif