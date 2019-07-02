// call_stream_cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "afx.h"
#include "call_stream_modernes.h"
#include "object_store/dbj_object_store.h"

int main(int argc, char * argv[])
{
	test_dbj_any_util(argc,argv);
	dbj::odm::storage_test();

		namespace cs = dbj::call_stream;

		// cs::default_cs	cst ;
		// since user::processor inherits the default processor
		// this just expands on its behaviour
		cs::call_streamer< user::processor > cst;

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
}
