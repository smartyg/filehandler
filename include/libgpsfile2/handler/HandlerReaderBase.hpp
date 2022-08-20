#ifndef _LIBGPSFILE2_HANDLER_HANDLERREADERBASE_
#define _LIBGPSFILE2_HANDLER_HANDLERREADERBASE_

#include <memory>
#include <string>
#include <istream>
#include <utility>

#include <libgpsfile2/handler/HandlerBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/traits/ProviderReaderTrait.hpp>

namespace libgpsfile2::handler {

	class HandlerReaderBase : virtual public HandlerBase {

	protected:
		std::unique_ptr<libgpsfile2::provider::ProviderReaderBase> _dp;

		HandlerReaderBase (std::unique_ptr<libgpsfile2::provider::ProviderReaderBase> dp, const std::string& path) : HandlerBase (path) {
			this->_dp = std::move (dp);
			this->_s = new std::ostream (this->_buf);
			this->_read_finished = false;
		}

	public:
		virtual ~HandlerReaderBase (void);

		virtual std::size_t streamRead (char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Write the content as provided by the provider to a file. If no file is given, the value of path (passed to the constructor is used).
		virtual bool writeFile (const std::string& = {}) final;

		template<libgpsfile2::provider::ProviderReaderTrait T>
		std::unique_ptr<T> getProvider (void) {
			if (this->_read_finished)
				return dynamic_unique_ptr_cast<T> (std::move (this->_dp));
			return std::unique_ptr<T>();
		}

	protected:
		virtual ssize_t streamAvailible (const off_t& = 0) const final;

		/**
		 * This is where the actual reading happens. When this method is called there is a request to read at least n bytes of data.
		 * The method should write at least n bytes to the provided stream. If writen less than requested bytes or if true is returned,
		 * it will be interperated as EOF (end of file) and no more calls to this method will be made.
		 */
		virtual bool read (std::ostream *stream, const std::size_t& len) = 0;

	private:
		std::ostream *_s;
		bool _read_finished;
	};
}

#endif /* _LIBGPSFILE2_HANDLER_HANDLERREADERBASE_ */
