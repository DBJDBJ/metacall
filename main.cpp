// call_stream_cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <stack>
#include <functional>
#include "metacall_modernes.h"
// simple console coloring 
// not part of call stream
#include "console_util/dbj_color.h"

// ad-hoc example : sum of squares
namespace my {

	using accumulator_type = typename std::stack<int>;
	using accumulator_ref = std::reference_wrapper< accumulator_type >;
	inline accumulator_type accum{};

// in the metacall universe functions do not return values
// dbjdbj:
// the ridiculous ammount of C++ cruft is required 
// to be able to pass the first arg as reference
// and have std::invocable recognize these functions as callable?
// is it me making some obvious mistake ?
// passing the first arg by value just works ... as expected
	inline void add(accumulator_ref acr, int a1, int a2)
	{
		accumulator_type& acc = acr;
		acc.push(a1+a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1+a2);
	}
	inline void sqr(accumulator_ref acr, int a1)
	{
		accumulator_type & acc = acr ;
		acc.push(a1*a1);
		dbj::print::green("\n  %s(%d) pushed : %d onto stack", __func__, a1, a1*a1);
	}
	inline void sum_of_squares (accumulator_ref acr )
	{
		accumulator_type& acc = acr;
		if (acc.size() < 2)
		{
			dbj::print::red("\n  %s() , stack size is : %d, can not operate.", __func__, (int)acc.size() );
			return;
		}
		const int a1 = acc.top(); acc.pop();
		const int a2 = acc.top(); acc.pop();
		dbj::print::green("\n %s() , rezult: %d", __func__, a1 + a2);
	}

	// ---------------------------------------------------------------------------------
	inline void simple_add(accumulator_type * acr, int a1, int a2)
	{
		auto& acc = *acr;
		acc.push(a1 + a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1 + a2);
	}
	inline void simple_sqr(accumulator_type * acr, int a1)
	{
		auto& acc = *acr;
		acc.push(a1 * a1);
		dbj::print::green("\n  %s(%d) pushed : %d onto stack", __func__, a1, a1 * a1);
	}
	inline void simple_sum_of_squares(accumulator_type * acr)
	{
		auto& acc = *acr;
		if (acc.size() < 2)
		{
			dbj::print::red("\n  %s() , stack size is : %d, can not operate.", __func__, (int)acc.size());
			return;
		}
		const int a1 = acc.top(); acc.pop();
		const int a2 = acc.top(); acc.pop();
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
		// std::ref must be used so that 
		// std::reference_wrapper is properly created
		(my::sqr, ref(my::accum), 2)
		(my::sqr, ref(my::accum), 4)
		(my::sum_of_squares, ref(my::accum) ) ;

	dbj::print::white("\n\nstack argument as a native pointer\n");

	meta_call
		// accumulator as a pointer
		(my::simple_sqr, &my::accum, 2)
		(my::simple_sqr, &my::accum, 4)
		(my::simple_sum_of_squares, &my::accum);

	return EXIT_SUCCESS;
}
int grand_sample(int argc, char * argv[])
{
		namespace mc = dbj::metacall;
		mc::default_mca async_meta_call;
		mc::default_mc	meta_call ;
		auto cst = async_meta_call;

		// since user::processor inherits the default processor
		// and just expands on its behaviour
		// we can freely slot it in
		// the default proc. behaviour is preserved
		//  mc::call_streamer< user::processor > cst;

		// goes to overloaded processor for handling string literals
		meta_call("add", 1, 2)
			// pre-existing function is immediately callable from a "call stream"
			(user::add, 4, 5, 6)
			// if found this is dispatched to specialized processor for handling char
			('X', 1, 2, 3)
			// if found this is dispatched to specialized processor for handling int list
			(7, 6, 8, 9)
			// if found this is dispatched to specialized processor for handling bool
			(true, 11)
			// and we can guess by now where this will go
			( std::ref( std::cout ), "this", "goes", "to", "generic", "processor");

		/* make a functor to deal with single int's */
		user::generic_command<int> my_ftor("LABEL");

		/*Again the same single CallStream is used */
		meta_call(my_ftor, 9)(my_ftor, 6);

		user::bool_streamer ubs;

		// user::bool_streamer can also process other metacalls same as the
		// default meta call can 
		ubs(true, 42)(false, 13)( my_ftor, 42 ) ;

		return EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
	
	// sum_of_squares_test(argc, argv);
    grand_sample(argc, argv);
	dbj::print::green("\n\n");
	return EXIT_SUCCESS;
}