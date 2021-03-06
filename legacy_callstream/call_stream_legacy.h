#pragma once
#include <iostream>
#include <string>
#include <stdarg.h>

// https://wandbox.org/permlink/6K3wg44UXnnbV0A7

namespace dbj {

	/*
	Abstract base to all call streams using functors, in case users
	prefer Functor OO pattern to functionl programming
	*/
	struct Functor {

		template< typename ... Args >
		void operator ()(Args ... args_) const = 0;
	};

	/*
	CallStream depends on the existence of this function.
	it dispatches the calls received to the concretized bridge function as found by compiler.
	if there is no required bridge function specialization found there is always a generic one.
	this is the root of the whole family of bridge functions
	*/
	template<typename T, typename ... Args>
	inline void bridge(T const& t_, Args ... args_)
	{
		std::cout << "\nBridge for type:" << typeid(t_).name();
	}

	template< typename ... Args>
	inline void bridge(Functor const & t_, Args ... args_)
	{
		std::cout << "\nBridge for Functor type:" << typeid(t_).name();
	}

	// If compiler does not find
	// a specialized bridge it needs
	// it will call the generic bridges above.

	/*
	CallStream is a single non-generic type. It one overloaded generic function call operator
	It depends on the existence of the family of 'bridge' functions.
	From inside 'operator ()' found, an appropriate bridge is found and used
	to the concrete implementation of the concrete type of the first argument
	if there is no required bridge function specialization found there is always a generic one.
	Result is comprehensive decoupling between callers and implementations hidden behind a call stream
	and a bridge.
	*/
	class CallStream {
	public:

		/*
		generic 'operator ()' for references and variable
		number of arguments following
		*/
		template<typename T, typename ... Args > 
		const CallStream & operator ()
			(T const & f, Args ... args_) const
		{
			dbj::bridge(f, args_ ...);
			return *this;
		}

		//template< typename ... Args > 
		//const CallStream & operator ()
		//	(Functor & ftor, Args ... args_) const
		//{
		//	ftor(ftor, args_ ...);
		//	return *this;
		//}
	};

} // dbj

//namespace dbj {
//
//	/* 
//	The CallStream bridge specialization that 
//	will reach to any kind-of-a Functor above 
//	*/
//		template<typename ... Args >
//	inline void bridge ( Functor * ftor_ , Args ... args_ )
//	{
//		// passing variable number of arguments 
//		// to the functor instance call operator 
//		(*ftor_)( args_ ...);
//	}
//}

namespace user {
	/*
	The generic functor which deals with different types of
	arguments sent through call stream.
	*/
	template<typename T>
	class tagged_functor : public dbj::Functor
	{
	public:
		typedef T					value_type;
		typedef tagged_functor		type;
		typedef tagged_functor* ptr_type;
	protected:
		std::string		tag_{};
		T				value_{};

		type& This() const { return *const_cast<ptr_type>(this); }

	public:

		tagged_functor(const char* tag_arg)
			: tag_(tag_arg == nullptr ? "UNKNOWN" : tag_arg)
		{
		}

		/*This functor requires exactly one argument of type T*/
		template<typename ... Args>
		 void operator ()( Args ... args_ )  const
		{
			// take the first argument, if any
			 if ( sizeof... args_ > 0)
						This().value_ = args_ ;
			this->show();
		}
		void show(std::ostream& out_ = std::cout) const
		{
			out_ << "\nTagged functor of type: " << typeid(*this).name()
				<< "\ntagged as: " << this->tag_.c_str()
				<< "\nis holding the type " << typeid(T).name()
				<< "\nand the value is: " << value_;
		}
	};
} // user
//  namespace dbj {
//
//	  using int_tagged_ftor = user::tagged_functor<int>;
//	  /*
//		The CallStream bridge specialization that
//		will reach to any kind-of-a Functor above
//		*/
//	    template<typename T>
//		inline void bridge (
//			const user::tagged_functor<T> & gf, va_list vl
//			)
//		{
//			// std::cout << std::endl << "Bridge to the " << typeid(gf).name();
//			gf(vl);
//			// passing to the functor variable number of arguments
//			// gf.show();
//			// showinf the type and value held
//		}
//}
//
inline void test_modern_call_stream()
{
	//
	// also note the variable number of arguments
	//
	dbj::CallStream && cs = dbj::CallStream{};

	cs("add", 1, 2)
		// goes to overloaded bridge for handling string literal
		('X', 1, 2, 3)
		// to specialized bridge for handling char
		(7, 6, 8, 9)
		// to specialized bridge for handling int
		(true, 11)
		// to specialized bridge for handling bool
		(&std::cout, "this", "goes", "to", "generic", "bridge");
	// and we know where the above will go

	/* make a functor to deal with single int's */
    dbj::Functor  const & my_ftor = user::tagged_functor<int>("LABEL");

	my_ftor(9);

	/*Again the same single CallStream is used */
	cs(my_ftor, 9)
		// "LABEL" and 9 are available to the implementation
		(my_ftor, 6);
	// "LABEL" and 6
}
