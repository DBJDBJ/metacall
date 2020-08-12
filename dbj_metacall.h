#pragma once

/*
the default call stream
*/

#include <iostream>
#include <type_traits>
#include <stdarg.h>
#include <tuple>
#include <future>

#undef DBJ_FUNCSIG
#define DBJ_FUNCSIG " " 

#ifdef __PRETTY_FUNCTION__
#undef DBJ_FUNCSIG
#define DBJ_FUNCSIG __PRETTY_FUNCTION__
#endif // __PRETTY_FUNCTION__

#ifdef __FUNCSIG__
#undef DBJ_FUNCSIG
#define DBJ_FUNCSIG __FUNCSIG__
#endif // __FUNCSIG__

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
		template< class T >
		struct remove_cvref {
			typedef std::remove_cv_t<std::remove_reference_t<T>> type;
		};

		template< class T >
		using remove_cvref_t = typename remove_cvref<T>::type;

		// https://stackoverflow.com/questions/59654482/how-is-stdis-function-implemented

		template<class T>
		struct is_function : std::integral_constant<
			bool,
			!std::is_const<const T>::value && !std::is_reference<T>::value
		> {};

		template <class Ty_>
		inline constexpr bool is_function_v = is_function<Ty_>::value;

/*
Default processor
each call can be a 'command' + variable number of arguments
or not 


2020 AUG 12	DBJ	Design has changed to hopefully more resilient.
				Instead of using "call operators" on instances of processors
				we mandate one or more overloads of a static function called 'call'
				see bellow for details
*/
struct default_processor  
{
	using type = default_processor;
	/*
	user defined processors must be callable
	and implementing method with this signature
	but presumably on some user defined command 
	*/
	template <typename CMD_, typename... Args>
	static void call (CMD_ && cmd_, Args && ... args_)
	{
	// functor inheriting from dbj::command_base
	// will be treated differently
		using CMDTYPE = CMD_; //  metacall::remove_cvref_t<CMD_>;
	using namespace std;
	constexpr bool  invocable = is_invocable_v<CMD_, Args...>;
	constexpr bool  invocable_as_function = metacall::is_function_v< CMD_ >;

		if constexpr (invocable)
		{
#ifdef DBJ_TRACE_BRIDGE
			dbj::print::white("\n------------------------------------------------------------");
			dbj::print::blue(
				"\nGeneric Bridge , invocable command: %s\n", typeid(CMDTYPE).name());
#endif
			cmd_(args_...);
		}
		else if constexpr (invocable_as_function)
		{
#ifdef DBJ_TRACE_BRIDGE
			dbj::print::white("\n------------------------------------------------------------");
			dbj::print::blue(
				"\nGeneric Bridge , function command: %s\n", typeid(CMDTYPE).name());
#endif
			cmd_(args_...);
		}
		else {
			// anything else, needs to have it's
			// processor implemented
			// see the example bellow for string literals
#ifdef DBJ_TRACE_BRIDGE
			dbj::print::red("\nCommand type: %s\nHas no processor implemented, or is not invocable, or you have not given proper arguments?\n", typeid(CMDTYPE).name());
#endif
		}
	}

}; // default_processor

/*
this functor delivers the "default calling experience"
to the clients
*/
template<typename PROC>
struct call_streamer final
{
	using type = call_streamer;

	call_streamer() = default;
	// no copy
	call_streamer(call_streamer const &) = delete;
	call_streamer & operator = (call_streamer const &) = delete;
	// move
	call_streamer(call_streamer &&) = default ;
	call_streamer & operator = (call_streamer &&) = default;

	template < typename CMD_, typename... Args>
	const type & operator()(CMD_ cmd_, Args && ... args_) const
	{
		PROC::call( cmd_, args_...) ;
		return *this;
	}

	// lets just repeat the non-const operator
	template < typename CMD_, typename... Args>
	type & operator()(CMD_ cmd_, Args && ... args_)
	{
		static PROC proc_;

		PROC::call( cmd_, args_...) ;
		return *this;
	}

}; // call_streamer

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
	struct call_streamer_async final
	{
		using type = call_streamer_async;

		template < typename CMD_, typename... Args>
		const call_streamer_async& operator()(CMD_ && cmd_, Args && ... args_) const
		{
			auto caller = [&]( auto cm_ , auto ... params_  ) {
				PROC::call(cm_, params_ ...);
			};

			auto must_not_discard_ [[maybe_unused]] = std::async(
				caller, cmd_, args_...
			);

			return *this;
		}	
		//
		//// just repeat the non-const overlord
		//template < typename CMD_, typename... Args>
		//call_streamer_async& operator()(CMD_ cmd_, Args && ... args_) 
		//{
		//	static PROC proc_;

		//	auto must_not_discard_ [[maybe_unused]] = std::async(
		//		proc_, cmd_, args_...
		//	);
		//	return *this;
		//}
	};

	// the default ASYNC metacall definition
	using default_async_mca = call_streamer_async< default_processor >;

	}	// namespace metacall
} // namespace dbj
