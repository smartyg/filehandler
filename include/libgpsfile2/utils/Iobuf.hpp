#ifndef _LIBGPSFILE2_UTILS_IOBUF_
#define _LIBGPSFILE2_UTILS_IOBUF_

#include <libgpsfile2/utils/IobufBase.hpp>

namespace libgpsfile2::utils {
	extern template class IobufBase<char>;
	extern template class IobufBase<uint8_t>;
	typedef IobufBase<char> Iobuf;
	typedef IobufBase<uint8_t> IobufInt;
#if 0
	template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Alloc = std::allocator<_CharT> >
	class basic_iobuf : public std::basic_streambuf<_CharT, _Traits> {
	private:
		typedef _CharT char_type;
		typedef _Traits traits_type;
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::pos_type pos_type;
		typedef typename traits_type::off_type off_type;
		typedef std::basic_string<char_type, _Traits, _Alloc> __string_type;

		std::streamsize _base_num_buffer;

	public:
		explicit basic_iobuf (void);
		~basic_iobuf (void);

		std::streamsize getNumPut (void) const;
		std::streamsize getNumGet (void) const;
		const _CharT *getBuffer (void) const;
		void consumeAllGet (void);
		void consumeGet (std::streamsize);

		#ifdef _DEBUG
		void printDebug (void) const;
		#endif

		// Get and set:
		/**
		 *  @brief  Copying out the string buffer.
		 *  @return  A copy of one of the underlying sequences.
		 *
		 *  <em>If the buffer is only created in input mode, the underlying
		 *  character sequence is equal to the input sequence; otherwise, it
		 *  is equal to the output sequence.</em> [27.7.1.2]/1
		 */
		__string_type str (void) const;

		/**
		 *  @brief  Setting a new buffer.
		 *  @param  __s  The string to use as a new sequence.
		 *
		 *  Deallocates any previous stored sequence, then copies @a s to
		 *  use as a new one.
		 */
		void str (const __string_type& __s);

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
		std::basic_streambuf<char_type,_Traits> *setbuf (char_type *b, std::streamsize s);

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		pos_type seekoff (off_type, std::ios_base::seekdir, std::ios_base::openmode /*__mode*/ = std::ios_base::in | std::ios_base::out);

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		pos_type seekpos(pos_type, std::ios_base::openmode /*__mode*/ = std::ios_base::in | std::ios_base::out);

		/**
		 *  @brief  Synchronizes the buffer arrays with the controlled sequences.
		 *  @return  -1 on failure.
		 *
		 *  Each derived class provides its own appropriate behavior,
		 *  including the definition of @a failure.
		 *  @note  Base class version does nothing, returns zero.
		 */
		int sync (void) override;

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
		std::streamsize showmanyc (void) override;

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
		std::streamsize xsgetn(char_type* __s, std::streamsize __n) override;

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
		int_type underflow (void) override;

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
		int_type pbackfail (int_type __c  = traits_type::eof()) override;

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
		std::streamsize xsputn (const char_type* __s, std::streamsize __n) override;

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
		int_type overflow (int_type __c = traits_type::eof()) override;
	};

	typedef basic_iobuf<char> Iobuf;
#endif
}

#endif /* _LIBGPSFILE2_UTILS_IOBUF_ */
