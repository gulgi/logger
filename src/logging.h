#pragma once

#include <string>
#include <memory>
#include <sstream>

namespace
{
	void variadic_print(std::string& result, const char *s)
	{
		while (*s)
		{
			if (*s == '\\')
			{
				if (*(s + 1) == '%')
					++s;
			}
			else if (*s == '%')
			{
				// Error.. Not enough arguments.
			}
			result += *s++;
		}
	}

	template<typename T, typename... Args>
	void variadic_print(std::string& result, const char *s, T value, Args... args)
	{
		while (*s)
		{
			if (*s == '%')
			{
				std::stringstream stream;
				stream << value;
				result += stream.str();
				variadic_print(result, s + 1, args...); // call even when *s == 0 to detect extra arguments
				return;
			}
			else if (*s == '\\')
			{
				if (*(s + 1) == '%')
				{
					++s;
				}
			}
			result += *s++;
		}
		// If it gets here, extra argument(s) were provided.    Report in debug or..?
	}
}

class Logging
{
public:
	Logging();
	~Logging();

	void log(const char* s);

	template<typename T, typename... Args>
	void log(const char* s, T arg1, Args... args)
	{
		std::string buffer;
		variadic_print(buffer, s, arg1, args...);
		log(buffer.c_str());
	}

private:
	struct Logging_data;
	std::unique_ptr<Logging_data> _data;

	static void thread_func(Logging_data* data);
};
