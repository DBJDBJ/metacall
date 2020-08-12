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

	inline void add_ref_arg(accumulator_type & acr, int a1, int a2)
	{
		accumulator_type& acc = acr;
		acc.push(a1 + a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1 + a2);
	}

	inline void add_ref_wrap_arg(accumulator_ref acr, int a1, int a2)
	{
		accumulator_type& acc = acr;
		acc.push(a1+a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1+a2);
	}

// ---------------------------------------------------------------------------------

	inline void add_arg_pass_ptr(accumulator_type * acr, int a1, int a2)
	{
		auto& acc = *acr;
		acc.push(a1 + a2);
		dbj::print::green("\n %s() pushed : %d onto stack", __func__, a1 + a2);
	}

} // my

int sum_of_squares_test (int argc, char* argv[])
{
	namespace mc = dbj::metacall;
	mc::default_mc	meta_call;

	my::accumulator_type accum{};

	dbj::print::white("\n\nstack argument as a reference\n");
	meta_call(my::add_ref_arg, accum, 3, 4);

	dbj::print::white("\n\nstack argument as a reference wrapper\n");
	meta_call(my::add_ref_wrap_arg, ref(accum), 2, 3) ;

	dbj::print::white("\n\nstack argument as a native pointer\n");
	meta_call(my::add_arg_pass_ptr, &accum, 2, 9) ;

	return EXIT_SUCCESS;
}
/*
-----------------------------------------------------------------------------------------
*/
int grand_sample(int argc, char * argv[])
{
		namespace mc = dbj::metacall;
		mc::default_async_mca async_meta_call;
		mc::default_mc	meta_call ;

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
			(true, 11);

		/* make a functor to deal with single int's */
		user::generic_command<int> my_ftor("LABEL");

		/*Again the same single CallStream is used */
		async_meta_call(my_ftor, 9)(my_ftor, 6);

		// user defined streamer
		user::bool_streamer ubs;

		// user::bool_streamer can also process other metacalls same as the
		// default meta call can 
		ubs(true, 42)(false, 13)( my_ftor, 42 ) ;

		return EXIT_SUCCESS;
}

/*
-----------------------------------------------------------------------------------------
*/
int main(int argc, char* argv[])
{
	
	sum_of_squares_test(argc, argv);
    grand_sample(argc, argv);
	dbj::print::green("\n\n");
	return EXIT_SUCCESS;
}