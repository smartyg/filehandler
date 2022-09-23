#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEUTILS_
#define _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEUTILS_

#include <string>
#include <stdexcept>

namespace libgpsfile2::provider::internal {
	void StringPadLeft (std::string&, const char, const int&);
	void stringinsertChar (std::string&, const char, const int&);

	void StringPadLeft (std::string& string, const char c, const int& min_length) {
		int len = static_cast<int>(string.size ());
		if (min_length <= len) return;
		const int diff = min_length - len;
		for (int i = len; i < min_length; ++i) {
			if (i >= diff)
				string.append (1, string[i - diff]);
			else
				string.append (1, c);
		}
		for (--len; len >= 0; --len) {
			if (len >= diff)
				string[len] = string[len - diff];
			else
				string[len] = c;
		}
	}

	void stringinsertChar (std::string& string, const char c, const int& p) {
		auto pos = string.size ();
		if (p > static_cast<int>(pos) || p < (-1 * static_cast<int>(pos))) throw std::out_of_range ("");
		const std::size_t insert_pos = p + (p < 0 ? pos : 0);

		string.append (1, string[--pos]);
		for (; pos > insert_pos; --pos) {
			string[pos] = string[pos - 1];
		}
		string[insert_pos] = c;
	}
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEUTILS_ */
