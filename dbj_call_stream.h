#pragma once

/*
the default call stream
*/

#include <iostream>
#include <type_traits>
#include <stdarg.h>
#include <tuple>

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
	namespace call_stream
	{

		/*
		This command_base is used just to "mark" user defined functors
		wishing to participate as "commands"
		*/
		struct command_base
		{
			/*
			in modern C++ one can not have virtual template methods
			*/
			template <typename... Args>
			/*virtual*/ void operator()(Args... args_) const = delete;
		};

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
				using FTRTYPE = decay_t<command_base>;
				constexpr bool  is_functor = is_base_of_v< FTRTYPE, ARGTYPE >;
				constexpr bool  invocable = is_invocable_v<ARGTYPE, Args...>;

#ifdef DBJ_TRACE_BRIDGE
	dbj::print::white("\n------------------------------------------------------------");
	dbj::print::red(
	"\nGeneric Bridge "
	", argument type: %s"
	", argument is functor offsping: %s"
	", argument is invocable: %s\n",
	typeid(ARGTYPE).name(), DBJ_BOOLALPHA(is_functor), DBJ_BOOLALPHA(invocable)
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
					dbj::print::white("\nType: %s, has no processor implemented", typeid(cmd_).name());
				}
			}

		}; // default_processor

/*
this functor delivers the "calling experience"
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
		proc_(cmd_, args_...);
		return *this;
	}
};

		using default_cs = call_streamer< default_processor >;

	}	// namespace call_stream
} // namespace dbj
