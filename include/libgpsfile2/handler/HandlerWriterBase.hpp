#ifndef _LIBGPSFILE2_HANDLER_HANDLERWRITERBASE_
#define _LIBGPSFILE2_HANDLER_HANDLERWRITERBASE_

#include <memory>
#include <string>
#include <istream>
#include <utility>

#include <libgpsfile2/handler/HandlerBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/traits/ProviderWriterTrait.hpp>
#include <libgpsfile2/utils/dynamic_unique_ptr_cast.hpp>

namespace libgpsfile2::handler {

	class HandlerWriterBase : virtual public HandlerBase {

	protected:
		std::unique_ptr<libgpsfile2::provider::ProviderWriterBase> _dp;

		HandlerWriterBase (std::unique_ptr<libgpsfile2::provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase (path) {
			this->_dp = std::move (dp);
			this->_s = new std::istream (this->_buf);
			this->_write_finished = false;
		}

	public:
		virtual ~HandlerWriterBase (void);

		virtual std::size_t streamWrite (const char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Read the content of a file to the provider. If no file is given, the value of path (passed to the constructor is used).
		virtual bool readFile (const std::string& = {}) final;

		template<libgpsfile2::provider::ProviderWriterTrait T>
		std::unique_ptr<T> getProvider (void) {
			if (this->_write_finished)
				return libgpsfile2::utils::dynamic_unique_ptr_cast<T> (std::move (this->_dp));
			return std::unique_ptr<T>();
		}

	protected:
		/**
		 * This function is called after each write request. The writen data is contained in the provided stream.
		 * When finished is true, this is the last and final call (there will be no more data provided after that final call).
		 * returns true for each successfull write, if returned false this means there occured an error in the write and the action will be terminated.
		 */
		virtual bool write (std::istream *stream, const bool& finished) = 0;

	private:
		std::istream *_s;
		bool _write_finished;
	};
}

#endif /* _LIBGPSFILE2_HANDLER_HANDLERWRITERBASE_ */
