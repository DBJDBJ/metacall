#pragma once
#include <string>
#include "dbj_metacall.h"

// user defined extensions to the default call stream
// name of this file is cynical omage to the site
// name https://www.modernescpp.com/

namespace user
{
	namespace mc = dbj::metacall ;

	/* 
	user defined processor (UDP)
	This UPD, takes care of 
	string literals and bools sent as the first arg

	dbj::metacall::call_streamer< user::processor > ucs;

	ucs(true,1,2,3)("oopsy!",4,5,6) ;

	*/
	struct bool_processor  final : mc::default_processor
	{

		/*
		user does not need to inherit from the default processor
		but it can, usually for convenience
		specificaly becuase we have inherited from the default processor
		we use it's operators to disptach calls which are not for bool
		as first argument
		*/
		using mc::default_processor::operator();
		/*
		in user define processor we take care of bool literals too
		*/
		template <typename ... Args>
		void operator () (bool bool_arg_, Args ... args_) const 
		{
			auto args_tup = std::make_tuple(args_...);
			auto arg_count_ = std::tuple_size_v< decltype(args_tup) >;

			int a1 = std::get<0>(args_tup);

#ifdef DBJ_TRACE_BRIDGE
	dbj::print::green("\n " DBJ_FUNCSIG " received: %s and %d as argument", DBJ_BOOLALPHA( bool_arg_) , a1 );
#endif
		}

	};

	using bool_streamer = typename dbj::metacall::call_streamer<bool_processor>;
/*
user defined comand
also a generic comand example

requirements:
- it has to have call operator of this signature
	template <typename... Args>
	void operator()(const T & , Args... ) const
*/
template <typename T>
class generic_command 
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

// any pre-existing function is immediately 
// callable from a "meta call"
//
// mc(add, 1,2,3) ;
//
void add(int a1, int a2, int a3) 
{
#ifdef DBJ_TRACE_BRIDGE
	dbj::print::blue("\n " DBJ_FUNCSIG " received: %d,%d,%d", a1, a2,a3);
#endif
}

} // namespace user
