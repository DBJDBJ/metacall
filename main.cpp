// call_stream_cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <stack>
#include "metacall_modernes.h"
// simple console coloring 
// not part of call stream
#include "console_util/dbj_color.h"

// ad-hoc example : sum of squares
namespace my {

	using accumulator_type = std::stack<int>;
	inline accumulator_type accum{};
	// in the metacall paradigm functions do not return values
	inline void add(accumulator_type * acc, int a1, int a2)
	{
		acc->push(a1+a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1+a2);
	}
	inline void sqr(accumulator_type * acc, int a1)
	{
		acc->push(a1*a1);
		dbj::print::green("\n  %s(%d) pushed : %d onto stack", __func__, a1, a1*a1);
	}
	inline void sum_of_squares (accumulator_type * acc )
	{
		if (acc->size() < 2)
		{
			dbj::print::red("\n  %s() , stack size is : %d, can not operate.", __func__, (int)acc->size() );
			return;
		}
		const int a1 = acc->top(); acc->pop();
		const int a2 = acc->top(); acc->pop();
		dbj::print::green("\n %s() , rezult: %d", __func__, a1 + a2);
	}
} // my

int sum_of_squares_test (int argc, char* argv[])
{
	namespace mc = dbj::metacall;
	mc::default_mc	meta_call;

	// pre-existing function is immediately callable from a "call stream"
	// call order is from left to right
	meta_call
		(my::sqr, &my::accum, 2)
		(my::sqr, &my::accum, 4)
		(my::sum_of_squares, &my::accum ) ;

	return EXIT_SUCCESS;
}
int grand_sample(int argc, char * argv[])
{
		namespace mc = dbj::metacall;
		mc::default_mca async_meta_call;
		// mc::default_mc	meta_call ;
		auto cst = async_meta_call;

		// since user::processor inherits the default processor
		// and just expands on its behaviour
		// we can freely slot it in
		// the default proc. behaviour is preserved
		//  mc::call_streamer< user::processor > cst;

		// goes to overloaded processor for handling string literals
		cst("add", 1, 2)
			// pre-existing function is immediately callable from a "call stream"
			(user::add, 4, 5, 6)
			// if found this is dispatched to specialized processor for handling char
			('X', 1, 2, 3)
			// if found this is dispatched to specialized processor for handling int list
			(7, 6, 8, 9)
			// if found this is dispatched to specialized processor for handling bool
			(true, 11)
			// and we can guess by now where this will go
			(&std::cout, "this", "goes", "to", "generic", "processor");

		/* make a functor to deal with single int's */
		user::generic_command<int> my_ftor("LABEL");

		/*Again the same single CallStream is used */
		cst(my_ftor, 9)(my_ftor, 6);

		return EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
	
	sum_of_squares_test(argc, argv);
    // grand_sample(argc, argv);
	dbj::print::green("\n\n");
	return EXIT_SUCCESS;
}