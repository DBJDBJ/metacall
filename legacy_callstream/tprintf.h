#pragma once
#ifndef _UNICODE
#error Unicode is mandatory!
#endif

#define DBJ_CHAR wchar_t

#define DBJ_STR(a) #a
#define DBJ_COMMENT(s) __pragma( comment( user, DBJ_STR(s) ) )
/*
https://msdn.microsoft.com/en-us/library/kb57fad8.aspx
*/
#include <assert.h>
#include<functional>
#include <stdarg.h>
#include <cstdio>
#include <memory>
#include <typeinfo>
#include <string>
#include <vector>

namespace dbj {
	namespace prt {

		// Usage:
		//   CBar* pBar = checked_cast<CBar *>(pFoo);
		//
		// NOTE: this is MicroSoft code
		//
		template <class TypeFrom, class TypeTo>
		__forceinline
			TypeTo *checked_cast(TypeFrom *p)
		{
			assert(dynamic_cast<TypeTo *>(p));
			return static_cast<TypeTo *>(p);
		}
		//------------------------------------------------------------------------------
		__forceinline	const std::wstring  cast(const std::wstring & p) {
			return p ;
		}
		__forceinline	const std::wstring  cast(const int & p) {
			return std::to_wstring(p);
		}

		//------------------------------------------------------------------------------
		std::wstring buf{};

void tprintf(const DBJ_CHAR* format) // base function
{
	buf += format;
}

/*
Inspired by: http://en.cppreference.com/w/cpp/language/parameter_pack
*/
template<typename T, typename... Targs>
void tprintf(const DBJ_CHAR* format, T value, Targs... Fargs) // recursive variadic function
{
	for (; *format != '\0'; format++) {
		if (*format == '%') {
			/* Magic happens here
			   value must have a proper way of giving a string representation of itself to the wstring
			*/
			buf +=  dbj::prt::cast(value);
			tprintf(format +  sizeof(DBJ_CHAR), Fargs...); // recursive call
			return;
		}
		buf += *format;
	}
}



namespace test {
	void unit1() {
		
		prt::buf.resize(0);
		prt::tprintf(L"% %", L"Abracadabra", 123);

		wprintf(L"%s", prt::buf.c_str());

		prt::buf.resize(0);

	}
}
	} // prt
} // dbj
