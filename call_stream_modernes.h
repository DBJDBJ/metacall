#pragma once
#include <iostream>
#include <string>
#include <type_traits>
#include <stdarg.h>

// https://wandbox.org/permlink/6K3wg44UXnnbV0A7

namespace dbj
{

/*
This command_base is used just to "mark" user defined functors
wishing to participate as "commands"
*/
struct command_base
{
	/*
	in modern C++ one can not have 
	virtual template methods
	*/
	template <typename... Args>
	/*virtual*/ void operator()(Args... args_) const = delete;
};

/*
here we execute the "commands"
if arguments are not in the format (command_base,arg1,arg2,...)
coresponding sink will be called before this processor is called
*/
template <typename T, typename... Args>
inline void processor(T const & cmd_, Args... args_)
{
	// functor inheriting from dbj::command_base
	// will be treated differently
	// NOTE: I could use my own is_callbale type trait here
	// but that will render a surface for abuse attacks
	using namespace std;
	using ARGTYPE = decay_t<T>;
	using FTRTYPE = decay_t<command_base>;
	constexpr bool  is_functor = is_base_of_v< FTRTYPE, ARGTYPE >;
	constexpr bool  invocable = is_invocable_v<ARGTYPE,Args...>;

#define DBJ_TRACE_BRIDGE

#ifdef DBJ_TRACE_BRIDGE
	cout << boolalpha
	    << "\n------------------------------------------------------------"
	    << "\nGeneric Bridge: \t"
		<< "\tArgument type: " << typeid(ARGTYPE).name()
		// << "\tFunctor base type: " << typeid(FTRTYPE).name()
		<< "\tArgument is functor offsping: " << is_functor
		<< "\tArgument is invocable: " << invocable << endl ;
#endif

	if constexpr (is_functor) 
	{
		// functor must have call operator with variable
		// number of arguments
		cmd_( args_...);
	}
	else {
		// anything else, needs to have it's
		// processor implemented
		cout << "\nType: " << typeid(cmd_).name() << ", has no processor implemented";
	}
}

//template <typename T, typename... Args>
//using bridge_type = 
//void (*)(T const& sink_, Args... args_);


class call_streamer final
{

public:
	/*
	this operator deliver the "calling experience"
	to the clients
	each call is a 'command' and variable number of arguments
	*/
	template < typename CMD_, typename... Args>
	const call_streamer& operator()(CMD_ cmd_, Args... args_) const
	{
		dbj::processor( cmd_, args_...);
		return *this;
	}
};

} // namespace dbj

namespace user
{
/*
user defined functor example
also a generic functor example

requirements:
- it has to inherit from the dbj::command_base
- it has to have call operator of this signature
	template <typename... Args>
	void operator()(const T & , Args... ) const
*/
template <typename T>
class tagged_functor : public dbj::command_base
{
public:
	typedef T value_type;
	typedef tagged_functor type;
	typedef tagged_functor *ptr_type;

protected:
	mutable std::string tag_{};
	mutable T value_{};

	// type &This() const { return *const_cast<ptr_type>(this); }

public:
	tagged_functor(const char *tag_arg)
		: tag_(tag_arg == nullptr ? "UNKNOWN" : tag_arg)
	{
	}

	/* mandatory method */
	template <typename... Args>
	void operator()(const T & first_arg_ , Args... /*args_*/) const
	{
		// take the first argument, if any
		this->value_ = first_arg_;
		this->show();
	}

	/* non mandatory method */
	void show(std::ostream &out_ = std::cout) const
	{
		out_ << "\n\nTagged functor of type: " << typeid(*this).name()
			 << ", tagged as: " << this->tag_.c_str()
			 << ",is holding the type " << typeid(T).name()
			 << ", and the value is: " << value_;
	}
};
} // namespace user

inline void test_modern_call_stream()
{
	//
	// also note the variable number of arguments
	//
	dbj::call_streamer	cs ;

	// goes to overloaded processor for handling string literals
	cs("add", 1, 2)
	// to specialized processor for handling char
	('X', 1, 2, 3)
	// to specialized processor for handling int
	(7, 6, 8, 9)
	// to specialized processor for handling bool
	(true, 11)
	// and we can guess by now where the above will go
	(&std::cout, "this", "goes", "to", "generic", "processor");

	/* make a functor to deal with single int's */
	user::tagged_functor<int> my_ftor("LABEL");

	/*Again the same single CallStream is used */
	cs(my_ftor, 9)(my_ftor, 6);
 }
