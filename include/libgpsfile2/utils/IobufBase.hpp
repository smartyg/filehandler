#ifndef _LIBGPSFILE2_UTILS_IOBUFBASE_
#define _LIBGPSFILE2_UTILS_IOBUFBASE_

#include <cstdlib>
#include <cstring>
#include <ios>
#include <string>
#include <streambuf>
#include <algorithm>
#include <stdexcept>
#include <Logger.hpp>

namespace libgpsfile2::utils {

	/*
	 * IobufBase
	 * IobufBase is a moving in-/output buffer.
	 * The full buffer consists of 3 parts:
	 * consumed buffer - This is the part that was put into the buffer an already taken out (get).
	 * actual buffer   - This is the current real/valid data in the buffer, it was put in, but not get (yet).
	 * reserved buffer - Part that is still available to put data into.
	 *
	 * This can be visualized by the following schematic:
	 * |-------------- buffer --------------|
	 * |- consumed -|- actual -|- reserved -|
	 * |------ in buffer ------|
	 *              |------ out buffer -----|
	 * 1            2          3            4
	 *
	 * the numbers refer the to variables used in this class:
	 * 1: _buf_beg - begin of the full buffer
	 * 2: _buf_get - Current position for `get` operations
	 * 3: _buf_put - Current position for `put` operations
	 * 4: _buf_end - end of the full buffer
	 *
	 * `put` operations expend the in(put) buffer to the back, if this reach the end of `full buffer``overflow` is called.
	 * `get` operations shrinks the out(put) buffer from the front.
	 *
	 * Once the reserved part reach 0 (when calling overflow), first is attempted to move `actual buffer` back to the begin of the `full buffer`.
	 * In effect this turns the space of `consumed buffer` into `reserved buffer` and leaves space to `put` more character. If this is not possible,
	 * because `actual buffer` uses the whole space of `full buffer`, `full buffer` is doubled in size and `reserved buffer` is as big as the size of
	 * `full buffer` previously was.
	 * The `underflow` operation only succeeds when `consumed buffer`is bigger than zero (0)
	 * The `str` operation returns or sets the `actual buffer`
	 */
	template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Alloc = std::allocator<_CharT> >
	class IobufBase final : public std::basic_streambuf<_CharT, _Traits> {
	private:
		typedef _CharT char_type;
		typedef _Traits traits_type;
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::pos_type pos_type;
		typedef typename traits_type::off_type off_type;
		typedef std::basic_string<char_type, _Traits, _Alloc> __string_type;

		char_type* _buf_beg;     ///< Start of buffer area.
		char_type* _buf_get;     ///< Current get position.
		char_type* _buf_put;     ///< Current put position.
		char_type* _buf_end;     ///< End of buffer area.
		std::streamsize _base_num_buffer;

	public:
		explicit IobufBase (const std::size_t& buffer_size = (1024 * 1024)) : std::basic_streambuf<_CharT, _Traits> () {
			DEBUG_MSG ("IobufBase::{:s} ({:d})\n", __func__, buffer_size);

			this->_buf_beg = static_cast<char_type *>(std::malloc (buffer_size * sizeof(char_type)));
			this->_buf_get = this->_buf_beg;
			this->_buf_put = this->_buf_get;
			this->_buf_end = this->_buf_beg + buffer_size * sizeof(char_type);
			this->_base_num_buffer = 0;
		}

		~IobufBase (void) {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			if (this->_buf_beg != NULL) std::free (this->_buf_beg);
		}

		std::streamsize getNumPut (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_base_num_buffer + (this->_buf_put - this->_buf_beg);
		}

		std::streamsize getNumGet (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_base_num_buffer + (this->_buf_get - this->_buf_beg);
		}

		const _CharT* getBuffer (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_buf_get;
		}

		void consumeAllGet (void) {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			// consume the full `actual buffer` by moving point 2 to point 3.
			this->_buf_get = this->_buf_put;
		}

		std::streamsize getTotalSize (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_buf_end - this->_buf_beg;
		}

		std::streamsize getConsumed (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_buf_get - this->_buf_beg;
		}

		std::streamsize getReserved (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return this->_buf_end - this->_buf_put;
		}

		void consumeGet (const std::streamsize& n) {
			DEBUG_MSG ("IobufBase::{:s} ({:d})\n", __func__, n);
			// Consume `n` elements of the `actual buffer`
			this->_buf_get += std::min (n, this->_buf_put - this->_buf_get);
		}

		void printDebug (void) const {
#ifdef _DEBUG
			cpplogger::Logger::get() (cpplogger::Level::DEBUG, "--- in buffer ---\n");
			DEBUG_MSG ("begin:   {:p}\n", fmt::ptr (this->_buf_beg));
			DEBUG_MSG ("current: {:p}\n", fmt::ptr (this->_buf_get));
			DEBUG_MSG ("end:     {:p}\n", fmt::ptr (this->_buf_put));
			cpplogger::Logger::get() (cpplogger::Level::DEBUG, "--- out buffer ---\n");
			DEBUG_MSG ("begin:   {:p}\n", fmt::ptr (this->_buf_get));
			DEBUG_MSG ("current: {:p}\n", fmt::ptr (this->_buf_put));
			DEBUG_MSG ("end:     {:p}\n", fmt::ptr (this->_buf_end));
			DEBUG_MSG ("bytes shifted: {:d}\n", this->_base_num_buffer);
#endif
			return;
		}

		// Get and set:
		/**
		 *  @brief  Copying out the string buffer.
		 *  @return  A copy of one of the underlying sequences.
		 *
		 *  <em>If the buffer is only created in input mode, the underlying
		 *  character sequence is equal to the input sequence; otherwise, it
		 *  is equal to the output sequence.</em> [27.7.1.2]/1
		 */
		__string_type str (void) const {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return __string_type (this->_buf_get, this->_buf_put);
		}

		/**
		 *  @brief  Setting a new buffer.
		 *  @param  __s  The string to use as a new sequence.
		 *
		 *  Deallocates any previous stored sequence, then copies @a s to
		 *  use as a new one.
		 */
		void str (const __string_type& __s) {
			DEBUG_MSG ("IobufBase::{:s} ({:p})\n", __func__, fmt::ptr (__s.data ()));
			size_t len = __s.size ();
			const char_type *ptr = __s.data ();

			// First, set the end of `actual buffer` to the begin of `actual buffer` (emptying it)
			this->_buf_put = this->_buf_get;

			for (std::size_t i = 0; i < len; ++i) {
				if (this->_buf_put == this->_buf_end) {
					int_type c = this->overflow (ptr[i]);
					if (static_cast<char_type>(c) != ptr[i]) throw std::runtime_error ("Can not allocate new memory for buffer");
				} else {
					this->_buf_get[i] = ptr[i];

					// Now increase the size of `actual buffer`
					++this->_buf_put;
				}
			}
		}

	private:
		// [27.5.2.4.2] buffer management and positioning
		/**
		 *  @brief  Manipulates the buffer.
		 *
		 *  Each derived class provides its own appropriate behavior.  See
		 *  the next-to-last paragraph of
		 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html#io.streambuf.buffering
		 *  for more on this function.
		 *
		 *  @note  Base class version does nothing, returns @c this.
		 */
		std::basic_streambuf<char_type,_Traits> *setbuf (char_type *b, std::streamsize s) override {
			DEBUG_MSG ("IobufBase::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (b), s);
			return this;
		}

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		pos_type seekoff (off_type offset, std::ios_base::seekdir type, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override {
			DEBUG_MSG ("IobufBase::{:s} ({:d}, {:d}, {:d})\n", __func__, offset, type, mode);

			if (type != std::ios_base::cur)
				return pos_type(off_type(-1));

			if (mode == std::ios_base::in) {
				if (offset >= (this->_buf_beg - this->_buf_get) && offset <= (this->_buf_put - this->_buf_get)) {
					this->_buf_get += offset;
					return pos_type(off_type(this->_base_num_buffer + this->_buf_get));
				} else
					return pos_type(off_type(-1));
			} else if (mode == std::ios_base::out) {
				if (offset >= (this->_buf_get - this->_buf_put) && offset <= (this->_buf_end - this->_buf_put)) {
					this->_buf_put += offset;
					return pos_type(off_type(this->_base_num_buffer + this->_buf_put));
				} else
					return pos_type(off_type(-1));
			} else
				return pos_type(off_type(-1));
		}

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) override {
			DEBUG_MSG ("IobufBase::{:s} ({:d}, {:d})\n", __func__, pos, mode);

			if (mode == std::ios_base::in)
				return this->seekoff (pos - off_type(this->_buf_get - this->_buf_beg), std::ios_base::cur, mode);
			else if (mode == std::ios_base::out)
				return this->seekoff (pos - off_type(this->_buf_put - this->_buf_beg), std::ios_base::cur, mode);

			return pos_type(off_type(-1));
		}

		/**
		 *  @brief  Synchronizes the buffer arrays with the controlled sequences.
		 *  @return  -1 on failure.
		 *
		 *  Each derived class provides its own appropriate behavior,
		 *  including the definition of @a failure.
		 *  @note  Base class version does nothing, returns zero.
		 */
		int sync (void) override {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			return 0;
		}

	public:
		// [27.5.2.4.3] get area
		/**
		 *  @brief  Investigating the data available.
		 *  @return  An estimate of the number of characters available in the
		 *           input sequence, or -1.
		 *
		 *  <em>If it returns a positive value, then successive calls to
		 *  @c underflow() will not return @c traits::eof() until at
		 *  least that number of characters have been supplied.  If @c
		 *  showmanyc() returns -1, then calls to @c underflow() or @c
		 *  uflow() will fail.</em> [27.5.2.4.3]/1
		 *
		 *  @note  Base class version does nothing, returns zero.
		 *  @note  The standard adds that <em>the intention is not only that the
		 *         calls [to underflow or uflow] will not return @c eof() but
		 *         that they will return immediately.</em>
		 *  @note  The standard adds that <em>the morphemes of @c showmanyc are
		 *         @b es-how-many-see, not @b show-manic.</em>
		 */
		std::streamsize showmanyc (void) override {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);
			// For this method compare to current out buffer pointer (pptr()) with the current in in buffer pointer.
			// It is not guaranteed that the the in-buffer end pointer is always up-to-date (in_avail()) is not (always) accurate.
			return this->_buf_put - this->_buf_get;
		}

	private:
		/**
		 *  @brief  Multiple character extraction.
		 *  @param  __s  A buffer area.
		 *  @param  __n  Maximum number of characters to assign.
		 *  @return  The number of characters assigned.
		 *
		 *  Fills @a __s[0] through @a __s[__n-1] with characters from the input
		 *  sequence, as if by @c sbumpc().  Stops when either @a __n characters
		 *  have been copied, or when @c traits::eof() would be copied.
		 *
		 *  It is expected that derived classes provide a more efficient
		 *  implementation by overriding this definition.
		 */
		std::streamsize xsgetn (char_type* __s, const std::streamsize __n) override {
			DEBUG_MSG ("IobufBase::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (__s), __n);

			const std::streamsize ret = std::min (__n, this->_buf_put - this->_buf_get);
			std::memcpy (__s, this->_buf_get, ret);
			this->_buf_get += ret;
			return ret;
		}

		/**
		 *  @brief  Fetches more data from the controlled sequence.
		 *  @return  The first character from the <em>pending sequence</em>.
		 *
		 *  Informally, this function is called when the input buffer is
		 *  exhausted (or does not exist, as buffering need not actually be
		 *  done).  If a buffer exists, it is @a refilled.  In either case, the
		 *  next available character is returned, or @c traits::eof() to
		 *  indicate a null pending sequence.
		 *
		 *  For a formal definition of the pending sequence, see a good text
		 *  such as Langer & Kreft, or [27.5.2.4.3]/7-14.
		 *
		 *  A functioning input streambuf can be created by overriding only
		 *  this function (no buffer area will be used).  For an example, see
		 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		int_type underflow (void) override {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);

			// If `actual buffer` is zero length, the buffer is empty.
			if (this->_buf_get == this->_buf_put) return traits_type::eof ();
			else return traits_type::to_int_type (*this->_buf_get);
		}

		int_type uflow (void) override {
			DEBUG_MSG ("IobufBase::{:s} ()\n", __func__);

			// If `actual buffer` is zero length, the buffer is empty.
			if (this->_buf_get == this->_buf_put) return traits_type::eof ();
			else return traits_type::to_int_type (*this->_buf_get++);
		}

		// [27.5.2.4.4] putback
		/**
		 *  @brief  Tries to back up the input sequence.
		 *  @param  __c  The character to be inserted back into the sequence.
		 *  @return  eof() on failure, <em>some other value</em> on success
		 *  @post  The constraints of @c gptr(), @c eback(), and @c pptr()
		 *         are the same as for @c underflow().
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		int_type pbackfail (const int_type __c  = traits_type::eof ()) override {
			DEBUG_MSG ("IobufBase::{:s} ({:d})\n", __func__, __c);
			if (this->_buf_get > this->_buf_beg) {
				--this->_buf_get;
				if (__c != traits_type::eof ())
					*this->_buf_get = __c;
				return traits_type::to_int_type (*this->_buf_get);
			}
			else return traits_type::eof ();
		}

		// Put area:
		/**
		 *  @brief  Multiple character insertion.
		 *  @param  __s  A buffer area.
		 *  @param  __n  Maximum number of characters to write.
		 *  @return  The number of characters written.
		 *
		 *  Writes @a __s[0] through @a __s[__n-1] to the output sequence, as if
		 *  by @c sputc().  Stops when either @a n characters have been
		 *  copied, or when @c sputc() would return @c traits::eof().
		 *
		 *  It is expected that derived classes provide a more efficient
		 *  implementation by overriding this definition.
		 */
		std::streamsize xsputn (const char_type* __s, const std::streamsize __n) override {
			DEBUG_MSG ("IobufBase::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (__s), __n);

			std::streamsize len = __n;
			while (len > this->_buf_end - this->_buf_put) {
				if (int_type(__s[0]) != this->overflow (__s[0])) throw std::runtime_error ("Overflow returned the wrong character");
				__s++;
				len--;
			}
			std::memcpy (this->_buf_put, __s, len);

			this->_buf_put += len;

			return __n;
		}

		/**
		 *  @brief  Consumes data from the buffer; writes to the
		 *          controlled sequence.
		 *  @param  __c  An additional character to consume.
		 *  @return  eof() to indicate failure, something else (usually
		 *           @a __c, or not_eof())
		 *
		 *  Informally, this function is called when the output buffer
		 *  is full (or does not exist, as buffering need not actually
		 *  be done).  If a buffer exists, it is @a consumed, with
		 *  <em>some effect</em> on the controlled sequence.
		 *  (Typically, the buffer is written out to the sequence
		 *  verbatim.)  In either case, the character @a c is also
		 *  written out, if @a __c is not @c eof().
		 *
		 *  For a formal definition of this function, see a good text
		 *  such as Langer & Kreft, or [27.5.2.4.5]/3-7.
		 *
		 *  A functioning output streambuf can be created by overriding only
		 *  this function (no buffer area will be used).
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		int_type overflow (const int_type __c = traits_type::eof ()) override {
			DEBUG_MSG ("IobufBase::{:s} ({:d})\n", __func__, __c);

			// Check if this is a real overflow, or if function was called for convenience.
			if (this->_buf_put == this->_buf_end) {
				if (this->_buf_get > this->_buf_beg) {
					// We will move the (valid) content of the buffer back to the beginning.
					std::memmove (this->_buf_beg, this->_buf_get, this->_buf_put - this->_buf_get);

					// Add the number of bytes the buffer is moved back to the base counter.
					this->_base_num_buffer += this->_buf_get - this->_buf_beg;
					this->_buf_put -= this->_buf_get - this->_buf_beg;
					this->_buf_get = this->_buf_beg;
				} else {
					// There is no free space in the beginning of the buffer to consume.
					// This means that `actual buffer` is equal to `full buffer`.
					// Get the size of the current `actual/full buffer`, this size will be used to recalculate the other pointers.
					const off_type s = this->_buf_end - this->_buf_beg;

					// Reallocate the buffer and double the size.
					this->_buf_beg = static_cast<char_type *>(std::realloc (this->_buf_beg, 2 * s));
					if (this->_buf_beg != nullptr) {
						// Buffer increasement is successful, now recalculate all pointers.
						//_buf_beg point to the new begin and `s` is the size of the `actual buffer` located at the beginning of `full buffer`.

						this->_buf_get = this->_buf_beg;
						this->_buf_put = this->_buf_beg + s;
						this->_buf_end = this->_buf_beg + 2 * s;
					}
					else {
						// `_buf_get` was equal  to `_buf_beg`, so reassign `_buf_beg`.
						this->_buf_beg = this->_buf_get;
						return traits_type::eof ();
					}
				}
			}

			*this->_buf_put = __c;
			++this->_buf_put;
			return __c;
		}
	};
}

#endif /* _LIBGPSFILE2_UTILS_IOBUFBASE_ */