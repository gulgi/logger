#pragma once

#include <string>
#include <memory>
#include <sstream>

namespace Logging_variadic
{
	template<typename T>
	void variadic_print(std::stringstream& stream, const T& t)
	{
		stream << t;
	}

	template<typename ... T>
	void variadic_print(std::stringstream& stream, const T& ... t)
	{
		(void)std::initializer_list<int>{ (variadic_print(stream, t), 0)... };
	}
}

class Logging
{
public:
	Logging();
	~Logging();

	void log(const char* s);

	template<typename ... T>
	void log(const T& ... args)
	{
		std::stringstream stream;
		Logging_variadic::variadic_print(stream, args...);
		log(stream.str().c_str());
	}

private:
	struct Logging_data;
	std::unique_ptr<Logging_data> _data;

	static void thread_func(Logging_data* data);
};
