
#include "../afx.h"
#include <any>
#include <optional>


template<typename T>
inline auto super_any(T const& v_)
{
	using optitype = std::optional< T >;
	auto any_ = std::any(v_);
	// now the proper lambda
	auto lambada_ = [&]() -> optitype {

		if (any_.has_value()) {
			static optitype opti_{ v_ };
			return opti_;
		}
		// else
		return std::nullopt;
	};

	return std::make_pair(lambada_, any_);
}

template<typename T>
inline auto make_any(T const& v_ = 0)
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
				// any bellow 
				return optitype{ std::any_cast<T>(any_) };
			},
			// any handled by the lambda above 
			// is the second element of the pair 
				std::any(v_)
				);
}

template<typename T>
inline decltype(auto) any_handler(T const& arg_ )
{
	struct {
		using optitype = std::optional< T >;
		// handler      
		optitype operator () () const {
			if (v_.has_value()) {
				static optitype uniq_{ std::any_cast<T>(v_) };
				return uniq_;
			}
			return std::nullopt; // else
		}
			mutable std::any v_{};
	} holder_{ 
		std::any(arg_) 
	};
		return holder_;
}

void test_dbj_any_util(int argc, char* argv[])
{
	auto [h1, a1] = super_any(42);
	int a42 = *h1();
	auto [h2, a2] = make_any(42);
	int b42 = *h2(a2);
	auto h3 = any_handler(42);
	int j = *h3();
}
