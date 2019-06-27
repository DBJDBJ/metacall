#pragma once

#include <string>
#include "dbj_call_stream.h"

// https://wandbox.org/permlink/6K3wg44UXnnbV0A7

namespace user
{
	namespace cs = dbj::call_stream;

	/* 
	user defined processor 
	user does not need to inherit from the default processor
	but it can ... in this case to take care of 
	string literals and bools sent as the first arg

	dbj::call_stream::call_streamer< user::processor > ucs;

	ucs(true,1,2,3)("oopsy!",4,5,6) ;

	*/
	struct processor : cs::default_processor {

		/*
		specificaly use the operators from the base
		*/
		using cs::default_processor::operator();
		/*
		in user define processor we take care of bool literals too
		*/
		template <typename ... Args>
		void operator () (bool bool_arg_, Args ... args_) const 
		{
			auto args_tup = std::make_tuple(args_...);
			auto arg_count_ = std::tuple_size_v< decltype(args_tup) >;
#ifdef DBJ_TRACE_BRIDGE
	dbj::print::green("\n " __FUNCSIG__ " received: %d and %d arguments", bool_arg_, arg_count_);
#endif
		}

	};
/*
user defined comand
also a generic comand example

requirements:
- it has to inherit from the dbj::command_base
- it has to have call operator of this signature
	template <typename... Args>
	void operator()(const T & , Args... ) const
*/
template <typename T>
class generic_command : public cs::command_base
{
	mutable std::string tag_{};
	mutable T value_{};

public:

	typedef T value_type;
	typedef generic_command type;
	generic_command(const char *tag_arg)
		: tag_(tag_arg == nullptr ? "UNKNOWN" : tag_arg)
	{
	}

	/* mandatory method */
	template <typename... Args>
	void operator()(const T & first_arg_ , Args... /*args_*/) const
	{
		// save the first argument
		this->value_ = first_arg_;
		this->show();
	}

	/* non mandatory method */
	void show(std::ostream &out_ = std::cout) const
	{
#ifdef DBJ_TRACE_BRIDGE
		dbj::print::green(
			"Generic CMD type %s, tag: %s, holding: %s",
			typeid(*this).name(), this->tag_.c_str(), typeid(T).name()
		);
#endif
	}
};

// pre-existing function is immediately callable from a "call stream"
void add(int a1, int a2, int a3) 
{
#ifdef DBJ_TRACE_BRIDGE
	dbj::print::blue("\n " __FUNCSIG__ " received: %d,%d,%d", a1, a2,a3);
#endif
}

} // namespace user

inline void test_modern_call_stream()
{
	namespace cs = dbj::call_stream;

	// cs::default_cs	cst ;
	// since user::processor inherits the default processor
	// this just expands on its behaviour
	cs::call_streamer< user::processor > cst;

	// goes to overloaded processor for handling string literals
	cst("add", 1, 2)
	// pre-existing function is immediately callable from a "call stream"
	(user::add,4,5,6)
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
