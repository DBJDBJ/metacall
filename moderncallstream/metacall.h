#pragma once
#ifndef _UNICODE
#error Unicode is mandatory!
#endif
#define DBJ_STR(a) #a
#define DBJ_COMMENT(s) __pragma( comment( user, DBJ_STR(s) ) )

/* https://msdn.microsoft.com/en-us/library/kb57fad8.aspx */
#include <stdarg.h>
#include <cstdio>
#include <typeinfo>
/*
2015-06-02		DBJ		Renamed CallStream to MetaCall

http://www.dbj.org/2014/12/18/callstream-part-3-c-binding/
*/

/* Call Stream bridges are spearated so they can be moved to a spearate header easier */

namespace dbj {
	namespace metacall {
		/*
		CallStream depends on the existence of this function.
		it dispatches the calls received to the concretized bridge function as found by compiler.
		if there is no required bridge function specialization found there is always a generic one.
		this is the root of the whole family of bridge functions
		*/
		template<typename T> inline void bridge(const T & t_, va_list & vl)
		{
			printf("\nGeneric bridge used for type %s ", typeid(t_).name());
		}
		/*
		Bridge specializations. If compiler does not find a specialized bridge it needs
		it will call the generic bridge above.
		The int bridge specialization
		*/
		template<> inline void bridge<int>(const int & j, va_list & /*vl*/)
		{
			printf("\nGeneric bridge used for 'int' type, value: %d ", j);
		}
		/*	literal strings bridge	*/
		inline void bridge(const char * s, va_list & /*vl*/)
		{
			printf("\nGeneric bridge used for 'char *' type, value: %s ", s);
		}
		/*	char's bridge	*/
		template<> inline void bridge<char>(const char & c, va_list & /*vl*/)
		{
			printf("\nGeneric bridge used for 'char' type, value: %c ", c);
		}
		/*	bool's bridge	*/
		template<> inline void bridge<bool>(const bool & b, va_list & /*vl*/)
		{
			printf("\nGeneric bridge used for 'bool' type, value: %s ", (b ? "TRUE" : "FALSE"));
		}
	} // namespace metacall 

} // dbj

namespace dbj {
	namespace metacall {
		/*
		CallStream is a single non-generic type. It declares overloaded generic function call operators
		It depends on the existence of the family of 'bridge' functions
		At compile time first appropriate 'operator ()' is found and used on the instance of this class
		From inside 'operator ()' found, second an appropriate bridge is found and used
		to the concrete implementation 'behind'
		if there is no required bridge function specialization found there is always a generic one.
		Result is comprehensive decoupling between callers and implementations hidden behind a call stream.
		*/
		class CallStream {
		public:
			/*
			generic 'operator ()' for pointers and variable number of arguments following
			*/
			template<typename T> const CallStream & operator ()(const T * f, ...) const
			{
				va_list vl;
				va_start(vl, f);
				bridge(*f, vl);
				va_end(vl);
				return *this;
			}
			/*
			generic 'operator ()' for references and variable number of arguments following
			*/
			// __pragma(comment(user, "DBJ -- VC2015 does not allow references as arguments to va_start()?"))
DBJ_COMMENT("DBJ -- VC2015 does not allow references as arguments to va_start()?")
			template<typename T> const CallStream & operator ()(const T /*&*/ f, ...) const
			{
				va_list vl;
				va_start(vl, f);
				bridge(f, vl);
				va_end(vl);
				return *this;
			}
			/*
			concretized 'operator ()' for string literals and variable number of arguments following
			*/
/*
			template<> const CallStream & operator ()(const wchar_t * s, ...) const
			{
				va_list vl;
				va_start(vl, s);
				bridge(s, vl);
				va_end(vl);
				return *this;
			}
*/
		};
	} // metacall
} // dbj

