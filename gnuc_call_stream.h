#pragma once
// This file is a "Hello, world!" in C++ language by GCC for wandbox.
#include <iostream>
#include <cstdlib>
#include <stdarg.h>

#define UNSD(x) (void)sizeof(x)

namespace gnuc {
/////////////////////////////////////////////////////////////
void add(int L, int R)
{
	std::cout << "\n add( " << L << "," << R << " ) => " << (L + R);
}

using FP = typename void (*)(int, int);

void bridge(FP fun, va_list args_)
{
	fun(va_arg(args_, int), va_arg(args_, int));
}
/////////////////////////////////////////////////////////////

template<typename T>
struct my_functor {
	void operator ()(va_list args_) const 
	{
		T actual_arg_ = va_arg(args_, T);
		std::cout << "\n called my_functor( " << actual_arg_ << " ) ";
	}
};

template<typename T>
void bridge( my_functor<T> ftor_, va_list args_)
{
	ftor_(args_);
}


/////////////////////////////////////////////////////////////

class CallStream {
public:
	/*	generic 'operator ()' 	*/
	template<typename T>
	const CallStream & operator () ( T fun, ...) const
	{
		va_list vl;
		va_start(vl, fun);
		bridge(fun, vl);
		// (ftor)(vl) ;
		va_end(vl);
		return *this;
	}

};


void test ()
{
	CallStream cs;
	my_functor<int> ftor;

	cs(ftor, 1, 2, 3)(add, 42, 13);

	std::cout << std::endl;
}

} // gnuc
