
#include "metacall.h"
#include<functional>


namespace customer {

	typedef int(*pfType)(const wchar_t *, va_list & vl);

	//auto printf = [](wchar_t const*  _Format, ...) -> int {} ;

	int printf (wchar_t const*  _Format, va_list & vl)
	{
		return wprintf(_Format,vl);
	}

}

namespace dbj {
	namespace metacall {

 /*	
 customer made this bridge to reach the customers internals	
 */
inline void bridge (const customer::pfType pf , va_list & vl )
{
	// printf("\nSpecialized bridge used for 'customer::printf()' type, value: %s ", typeid(pf).name());
	wchar_t const * fmt_ = va_arg(vl, wchar_t const *);
	pf(fmt_,vl);
}

	} // namespace metacall 

} // dbj

int wmain(int argc, wchar_t ** argv)
{
	using namespace dbj::metacall;

	// also note the variable number of arguments
	CallStream	& cs = CallStream();
	cs(L"add", 1, 2)
		// goes to overloaded bridge for handling string literal
		('X', 1, 2, 3)
		// to specialized bridge for handling char
		(7, 6, 8, 9)
		//  to specialized bridge for handling int
		(true, 11)
		//  to specialized bridge for handling bool
		(customer::printf, L"this", L"goes", L"to", L"generic", L"bridge");
	// and we know where the above will go
}