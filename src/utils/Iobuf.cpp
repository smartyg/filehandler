#include "config.h"
#include <features.h>

#include "libgpsfile2/utils/Iobuf.hpp"

#include <cstring>
#include <streambuf>
#include <stdexcept>
#include <gpsdata/utils/Logger.hpp>

#define BUFFER_STEP_SIZE (1024 * 1024 * sizeof(char_type))

using libgpsfile2::utils::basic_iobuf;
using gpsdata::utils::Logger;

template<typename _CharT, typename _Traits, typename _Alloc>
basic_iobuf<_CharT, _Traits, _Alloc>::basic_iobuf (void) : std::basic_streambuf<_CharT, _Traits> () {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);

	this->_M_out_beg = static_cast<char_type *>(malloc (BUFFER_STEP_SIZE));
	this->_M_out_cur = this->_M_out_beg;
	this->_M_out_end = this->_M_out_beg + BUFFER_STEP_SIZE;

	this->_M_in_beg = this->_M_out_beg;
	this->_M_in_cur = this->_M_in_beg;
	this->_M_in_end = this->_M_out_cur;

	this->_base_num_buffer = 0;
}

template<typename _CharT, typename _Traits, typename _Alloc>
basic_iobuf<_CharT, _Traits, _Alloc>::~basic_iobuf (void) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	if (this->_M_out_beg != NULL) free(this->_M_out_beg);
}

#ifdef _DEBUG
template<typename _CharT, typename _Traits, typename _Alloc>
void basic_iobuf<_CharT, _Traits, _Alloc>::printDebug (void) const {
	Logger::Log (Logger::DEBUG, __FILE__, __func__, __LINE__, "--- in buffer ---\n");
	DEBUG_MSG("begin:   %p\n", this->_M_in_beg);
	DEBUG_MSG("current: %p\n", this->_M_in_cur);
	DEBUG_MSG("end:     %p\n", this->_M_in_end);
	Logger::Log (Logger::DEBUG, __FILE__, __func__, __LINE__, "--- out buffer ---\n");
	DEBUG_MSG("begin:   %p\n", this->_M_out_beg);
	DEBUG_MSG("current: %p\n", this->_M_out_cur);
	DEBUG_MSG("end:     %p\n", this->_M_out_end);
	DEBUG_MSG("bytes shifted: %ld\n", this->_base_num_buffer);
	return;
}
#endif

template<typename _CharT, typename _Traits, typename _Alloc>
std::streamsize basic_iobuf<_CharT, _Traits, _Alloc>::getNumPut (void) const {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return this->_base_num_buffer + (this->_M_out_cur - this->_M_out_beg);
}

template<typename _CharT, typename _Traits, typename _Alloc>
std::streamsize basic_iobuf<_CharT, _Traits, _Alloc>::getNumGet (void) const {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return this->_base_num_buffer + (this->_M_in_cur - this->_M_in_beg);
}

template<typename _CharT, typename _Traits, typename _Alloc>
const _CharT *basic_iobuf<_CharT, _Traits, _Alloc>::getBuffer (void) const {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return this->_M_in_cur;
}

template<typename _CharT, typename _Traits, typename _Alloc>
void basic_iobuf<_CharT, _Traits, _Alloc>::consumeAllGet (void) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	this->_M_in_cur = this->_M_in_end;
}

template<typename _CharT, typename _Traits, typename _Alloc>
void basic_iobuf<_CharT, _Traits, _Alloc>::consumeGet (std::streamsize n) {
	DEBUG_MSG("basic_iobuf::%s (%ld)\n", __func__, n);
	this->_M_in_cur += n;
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::__string_type basic_iobuf<_CharT, _Traits, _Alloc>::str (void) const {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return __string_type(this->pbase(), this->pptr());
}

template<typename _CharT, typename _Traits, typename _Alloc>
void basic_iobuf<_CharT, _Traits, _Alloc>::str (const __string_type& __s) {
	DEBUG_MSG("basic_iobuf::%s (%s)\n", __func__, __s.data ());
	size_t len = __s.size ();
	const char_type *ptr = __s.data ();

	while (len > 0) {
		len -= this->xsputn(ptr, len);
		if (len > 0) {
			int_type c = this->overflow(*ptr);
			if (static_cast<char_type>(c) != *ptr) throw std::runtime_error("Can not allocate new memory for buffer");
			ptr++;
			len--;
		}
	}
}

template<typename _CharT, typename _Traits, typename _Alloc>
std::basic_streambuf<_CharT, _Traits> *basic_iobuf<_CharT, _Traits, _Alloc>::setbuf (char_type *b, std::streamsize s) {
	DEBUG_MSG("basic_iobuf::%s (%p, %ld)\n", __func__, b, s);
	return this;
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::pos_type basic_iobuf<_CharT, _Traits, _Alloc>::seekoff (off_type, std::ios_base::seekdir, std::ios_base::openmode) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return pos_type(off_type(-1));
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::pos_type basic_iobuf<_CharT, _Traits, _Alloc>::seekpos (pos_type, std::ios_base::openmode) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	return pos_type(off_type(-1));
}

template<typename _CharT, typename _Traits, typename _Alloc>
int basic_iobuf<_CharT, _Traits, _Alloc>::sync (void) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	// Syncronize the buffers, meaning update the end of the in-buffer.
	this->_M_in_end = this->_M_out_cur;
	return 0;
}

template<typename _CharT, typename _Traits, typename _Alloc>
std::streamsize basic_iobuf<_CharT, _Traits, _Alloc>::showmanyc() {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	// For this method compare to current out buffer pointer (pptr()) with the current in in buffer pointer.
	// It is not guaranteed that the the in-buffer end pointer is always up-to-date (in_avail()) is not (always) accurate.
	return this->_M_out_cur - this->_M_in_cur;
}

template<typename _CharT, typename _Traits, typename _Alloc>
std::streamsize basic_iobuf<_CharT, _Traits, _Alloc>::xsgetn(char_type* __s, std::streamsize __n) {
	DEBUG_MSG("basic_iobuf::%s (%p, %ld)\n", __func__, __s, __n);

	std::streamsize ret = std::min (__n, this->_M_in_end - this->_M_in_cur);
	memcpy (__s, this->_M_in_cur, ret);
	this->_M_in_cur += ret;
	return ret;
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::int_type basic_iobuf<_CharT, _Traits, _Alloc>::underflow (void) {
	DEBUG_MSG("basic_iobuf::%s ()\n", __func__);
	this->_M_in_end = this->_M_out_cur;
	if (this->_M_in_cur == this->_M_in_end) return traits_type::eof();
	else return traits_type::to_int_type(*this->_M_in_cur);
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::int_type basic_iobuf<_CharT, _Traits, _Alloc>::pbackfail(int_type __c) {
	DEBUG_MSG("basic_iobuf::%s (%d)\n", __func__, __c);
	if (this->_M_in_cur > this->_M_in_beg) {
		this->_M_in_cur--;
		*this->_M_in_cur = __c;
		return traits_type::to_int_type(__c);
	}
	else return traits_type::eof();
}

template<typename _CharT, typename _Traits, typename _Alloc>
std::streamsize basic_iobuf<_CharT, _Traits, _Alloc>::xsputn (const char_type* __s, std::streamsize __n) {
	DEBUG_MSG("basic_iobuf::%s (%p, %ld)\n", __func__, __s, __n);

	std::streamsize ret = __n;
	while (__n > this->_M_out_end - this->_M_out_cur) {
		if (__s[0] != this->overflow(__s[0])) throw std::runtime_error("Overflow returned the wrong character");
		__s++;
		__n--;
	}
	memcpy(this->_M_out_cur, __s, __n);

	this->_M_out_cur += __n;
	this->_M_in_end += __n;

	return ret;
}

template<typename _CharT, typename _Traits, typename _Alloc>
typename basic_iobuf<_CharT, _Traits, _Alloc>::int_type basic_iobuf<_CharT, _Traits, _Alloc>::overflow (int_type __c) {
	DEBUG_MSG("basic_iobuf::%s (%d)\n", __func__, __c);
	if (this->_M_in_cur > this->_M_in_beg) {
		// We will move the (valid) content of the buffer back to the beginning.
		memmove (this->_M_in_beg, this->_M_in_cur, this->_M_out_cur - this->_M_in_cur);

		// Add the number of bytes the buffer is moved back to the base counter.
		this->_base_num_buffer += this->_M_in_cur - this->_M_in_beg;
		this->_M_out_cur -= this->_M_in_cur - this->_M_in_beg;
		this->_M_in_cur = this->_M_in_beg;
		this->_M_in_end = this->_M_out_cur;

	} else {
		// There is no free space in the beginning of the buffer to consume, so increase the buffer.
		this->_M_in_beg = static_cast<char_type *>(realloc (this->_M_out_beg, (this->_M_out_end - this->_M_out_beg) + (1024 * sizeof(char_type))));
		if (this->_M_in_beg != NULL) {
			/* Buffer increasement is successful, now recalculate all pointers.
			 * _M_in_beg point to the new begin
			 * _M_out_beg still points to the old buffer
			 */
			this->_M_out_end = this->_M_in_beg + (this->_M_out_end - this->_M_out_beg) + (1024 * sizeof(char_type));
			this->_M_in_cur = this->_M_in_beg + (this->_M_in_cur - this->_M_out_beg);
			this->_M_out_cur = this->_M_in_beg + (this->_M_out_cur - this->_M_out_beg);
			this->_M_out_beg = this->_M_in_beg;
			this->_M_in_end = this->_M_out_cur;
		}
		else {
			this->_M_in_beg = this->_M_out_beg;
			return traits_type::eof();
		}
	}

	*this->_M_out_cur = __c;
	this->_M_out_cur++;
	this->_M_in_end = this->_M_out_cur;
	return traits_type::to_int_type(__c);
}

template class basic_iobuf<char>;