#include <any>
#include <optional>

template<typename T> auto make_any(T const& v_ = 0) 
{
	using optitype = std::optional< T >; 
	return // a pair 
		std::make_pair( 
			// handler lambda     
			[=](std::any any_) -> optitype 
			{ 
		    if (!any_.has_value()) 
			    return std::nullopt; 
	         // crucialy here we use the T given to 
			 // make_any in the first place 
			 // to store the value in the optional
	         return optitype{ std::any_cast<T>(any_) }; 
			}, 
// any handled by the lambda above 
// is the second element of the pair 
	made std::any{ v_ } ); 
}

void __cdecl test_dbj_any_util(int argc, char* argv[])
{
}
