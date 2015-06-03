#pragma once
#ifndef _UNICODE
#error Unicode is mandatory!
#endif

namespace dbj {
	namespace fsh {

		// static string table of format strings
		// http://en.wikipedia.org/wiki/Printf_format_string
		static std::vector< wchar_t *> string_table_of_format_strings{
			L"?",
			L"%c",
			L"%s",
			L"%d",
			L"%f",
			L"%x",
			L"%u"
		};

		// t2fs --> type to format string
		// as defined by printf() rules
		template <class T> wchar_t * t2f(const T &)
		{
			return string_table_of_format_strings[0];
		}

		template <>	wchar_t * t2f(const wchar_t &)
		{
			return string_table_of_format_strings[1];
		}

		// eofh --> end of harvesting mark
		std::wstring eofh{ L"" };

		std::wstring t2fsh_result = eofh;

		// t2fsh == type 2 format string harvester
		template<typename T, typename ... Types>
		std::wstring t2fsh(const T & first)
		{
			return eofh;
		}

		template<typename T, typename ... Types>
		std::wstring t2fsh(const T & first, Types ... rest)
		{
			while (eofh != t2fsh(first, rest...)) {
				t2fsh_result += t2f(first);
			}
			return t2fsh_result;
		}

		namespace test {
			void format_string_harvester() {

				std::wstring fmt_str = t2fsh(1, true, "ANSI", L"UNICODE", 3.14);

			}
		} // test

	} // fsh
} // dbj