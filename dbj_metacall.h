#pragma once

/*
the default call stream
*/

#include <iostream>
#include <type_traits>
#include <stdarg.h>
#include <tuple>
#include <future>

// comment this out to stop extensive reporting
#define DBJ_TRACE_BRIDGE

#ifdef DBJ_TRACE_BRIDGE
// simple console coloring 
// not part of call stream
#include "console_util/dbj_color.h"
#endif

/*

the default processor is where all call streamer calls are processed and dispatched
in case of the "commands" in the format (command_base,arg1,arg2,...)
if first argument is not callable
coresponding sink will be called before this processor is called
otherwise the user will know it is not called 

by delivering a more complex processor users can change
the call stream functionality dramatically

example: facade for a database behind

// dbf is a db facade
// db_processor is a instance 
// things have been done while this instance 
// has been constructed to define the behaviour of the
// processor and thus exhibit the behaviour of
// this mechanism
dbj::call_stream::call_streamer dbf( db_processor );

// store {0,A}{1,B} then print {1,B}
dbf(db::set, "0", "A" )(db::set, "1", "B" )(db::get, "1", std::cout) ;

Above seems trivial but remember: the processor hides the important behavior
implemented. For example syntax or data validity checkers etc. Or just a focal
point where wan can lo the traffic, or redirect the traffic, etc ...

*/

namespace dbj {
	namespace metacall
	{
/*
Default processor
each call can be a 'command' + variable number of arguments
or not 
*/
struct default_processor  
{
	/*
	user defined processors must be functors implementing
	this method
	*/
	template <typename T, typename... Args>
	void operator () (T const& cmd_, Args... args_) const
	{
	// functor inheriting from dbj::command_base
	// will be treated differently
	using namespace std;
	using ARGTYPE = decay_t<T>;
	constexpr bool  invocable = is_invocable_v<ARGTYPE, Args...>;

#ifdef DBJ_TRACE_BRIDGE
	dbj::print::white("\n------------------------------------------------------------");
	dbj::print::red(
	"\nGeneric Bridge "
	", argument type: %s"
	", argument is invocable: %s\n",
	typeid(ARGTYPE).name(), DBJ_BOOLALPHA(invocable)
);
#endif
		if constexpr (invocable)
		{
			// commnad must be a functor with call operator with variable
			// number of arguments
			// of some tother invocable object
			cmd_(args_...);
		}
		else {
			// anything else, needs to have it's
			// processor implemented
			// see the example bellow for string literals
#ifdef DBJ_TRACE_BRIDGE
			dbj::print::white("\nType: %s, has no processor implemented", typeid(cmd_).name());
#endif
		}
	}

}; // default_processor

/*
this functor delivers the "default calling experience"
to the clients
*/
template<typename PROC>
class call_streamer final
{
	PROC proc_;

public:

	template < typename CMD_, typename... Args>
	const call_streamer& operator()(CMD_ cmd_, Args... args_) const
	{
#ifdef DBJ_METACALL_ASYNC
		auto must_not_discard_ = std::async(
			proc_, cmd_, args_...
		);
#else
		proc_( cmd_, args_...) ;
#endif
		return *this;
	}
};

	// the default SYNC metacall definition
	using default_mc = call_streamer< default_processor >;

	/*
	this functor delivers the async calling experience
	to the clients

	behaviour can be very different thus it will exist in 
	the same time as the sync version for particular uses
	when order of calls is not relevant
	*/
	template<typename PROC>
	class call_streamer_async final
	{
		PROC proc_;

	public:

		template < typename CMD_, typename... Args>
		const call_streamer_async& operator()(CMD_ cmd_, Args... args_) const
		{
			[[maybe_unused]]auto must_not_discard_ = std::async(
				proc_, cmd_, args_...
			);
			return *this;
		}
	};

	// the default ASYNC metacall definition
	using default_mca = call_streamer_async< default_processor >;

	}	// namespace metacall
} // namespace dbj
