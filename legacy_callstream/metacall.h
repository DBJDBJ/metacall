#pragma once
#ifndef _UNICODE
#error Unicode is mandatory!
#endif
#define DBJ_STR(a) #a
#define DBJ_COMMENT(s) __pragma( comment( user, DBJ_STR(s) ) )
/* 
https://msdn.microsoft.com/en-us/library/kb57fad8.aspx 
*/

#include<functional>
#include <stdarg.h>
#include <cstdio>
#include <memory>
#include <typeinfo>
#include <string>
#include <vector>
/*
2015-06-02		DBJ		Renamed CallStream to MetaCall
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

		/*
		    meta target is kind-of-a functor

			NOTE: An interface class (C++ Component Extensions) cannot contain a data member unless it is also a property.
                  Anything other than a property or member function is not allowed in an interface. 
				  Furthermore, constructors, destructors, and operators are not allowed.
		*/

		typedef unsigned short int argsiz_T;

		class MetaTarget
		{
		public:
			/*
			to be implemented by inheritors
			*/
			template<typename ... Args>
			const MetaTarget & operator () ( Args ... params) = 0;
		};

		template<typename Target /*= MetaTarget*/ >
		class MetaCall {
			std::unique_ptr<Target> target_ ;
			// forbiden operations
			//MetaCall( const MetaCall & ) {  }
			//MetaCall & operator = (const MetaCall &) {  }
		public:
			MetaCall() : target_( new Target() ) {  }
			/*
			generic 'operator ()' for variable number of arguments
			*/
			template< class ... Args > 
			const MetaCall & operator ()(Args ... params) 
			{
				// const argsiz_T no_of_params = sizeof...(Args);
				try {
					   this->target_(params ...);
				}
				catch (...) {

				}
				return *this;
			}
		};
	} // metacall
} // dbj

namespace dbj {


	int sum()
	{
		return 0;
	}

	template<typename ... Types>
	int sum(int first, Types ... rest)
	{
		return first + sum(rest...);
	}

	// sum(1, 2, 3, 4, 5); 
	// returns 15

	class PrintTarget
		/* : dbj::metacall::MetaTarget  */
	{
	public:
		// template<typename ... Args> const MetaTarget & operator () () { return *this; }
		template<typename ... Args>
		const PrintTarget & operator () (Args ... params) const {

			wchar_t * _Format;
			va_list _ArgList;
			__crt_va_start(_ArgList, _Format);
			_Format = __crt_va_arg(_ArgList, wchar_t *);

			this->print(_Format, _ArgList);

			__crt_va_end(_ArgList);
			return *this;
		}

	int print(const wchar_t * _Format, ... ) const {
		int _Result;
		va_list _ArgList;
		__crt_va_start(_ArgList, _Format);
		_Result = _vfwprintf_l(stdout, _Format, NULL, _ArgList);
		__crt_va_end(_ArgList);
		return _Result;
	}

	};

	void test() {

		_ASSERT( 15 == sum(1, 2, 3, 4, 5) ); 
		// returns 15

		PrintTarget pt;

		pt(L"whatever",1,43,"L")(2)(3)(4);

		metacall::MetaCall<PrintTarget> ms;

		//ms(1);
	}

}


