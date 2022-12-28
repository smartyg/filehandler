#ifndef _LIBGPSFILE2_UTILS_STRINGSBASE_
#define _LIBGPSFILE2_UTILS_STRINGSBASE_

#include <string>
#include <stdexcept>

namespace libgpsfile2::utils {
	template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Alloc = std::allocator<_CharT> >
	class StringsBase final {
	public:
		static bool isSigned (const std::basic_string<_CharT, _Traits, _Alloc>& string) {
			if (string.size () == 0)
				return false;
			const _CharT first_char = string[0];
			if (first_char == static_cast<const _CharT>('+') ||
				first_char == static_cast<const _CharT>('-')) return true;
			return false;
		}

		//TODO: fix for omit_first_char = true cases
		static void padLeft (std::basic_string<_CharT, _Traits, _Alloc>& string, const _CharT c, const int& min_length, const bool& omit_first_char = false) {
			const int omit_len = (omit_first_char ? 1 : 0);
			int len = static_cast<int>(string.size ()) - omit_len;
			if (min_length <= len) return;
			const int diff = min_length - len;
			for (int i = len; i < min_length; ++i) {
				if (i >= diff)
					string.append (1, string[i - diff + omit_len]);
				else
					string.append (1, c);
			}
			for (--len; len >= 0; --len) {
				if (len >= diff)
					string[len + omit_len] = string[len - diff + omit_len];
				else
					string[len + omit_len] = c;
			}
		}

		static void insertChar (std::basic_string<_CharT, _Traits, _Alloc>& string, const _CharT c, const int& p) {
			auto pos = string.size ();
			if (p > static_cast<int>(pos) || p < (-1 * static_cast<int>(pos))) throw std::out_of_range ("");
			const std::size_t insert_pos = p + (p < 0 ? pos : 0);

			string.append (1, string[--pos]);
			for (; pos > insert_pos; --pos) {
				string[pos] = string[pos - 1];
			}
			string[insert_pos] = c;
		}
	};
}

#endif /* _LIBGPSFILE2_UTILS_STRINGSBASE_ */
