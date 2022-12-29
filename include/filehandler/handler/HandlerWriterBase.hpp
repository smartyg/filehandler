#ifndef _FILEHANDLER_HANDLER_HANDLERWRITERBASE_
#define _FILEHANDLER_HANDLER_HANDLERWRITERBASE_

#include <memory>
#include <string>
#include <istream>
#include <atomic>
#include <utility>

#include <filehandler/handler/HandlerBase.hpp>
#include <filehandler/provider/ProviderWriterBase.hpp>
#include <filehandler/traits/ProviderWriterTrait.hpp>
#include <filehandler/utils/dynamic_unique_ptr_cast.hpp>

namespace filehandler::handler {

	class HandlerWriterBase : virtual public HandlerBase {
		mutable std::shared_mutex _mutex;
		std::istream *_s;
		std::atomic<bool> _write_finished = false;

	protected:
		std::unique_ptr<filehandler::provider::ProviderWriterBase> _dp;

		HandlerWriterBase (std::unique_ptr<filehandler::provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase (path) {
			std::unique_lock lock (this->_mutex);
			this->_dp = std::move (dp);
			this->_s = new std::istream (this->_buf);
		}

	public:
		virtual ~HandlerWriterBase (void);

		virtual std::size_t streamWrite (const char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Read the content of a file to the provider. If no file is given, the value of path (passed to the constructor is used).
		virtual bool readFile (const std::string& = {}) final;

		template<filehandler::provider::ProviderWriterTrait T>
		const std::unique_ptr<T> getProvider (void) {
			std::unique_lock lock (this->_mutex);
			if (this->_write_finished)
				return filehandler::utils::dynamic_unique_ptr_cast<T> (std::move (this->_dp));
			return std::unique_ptr<T>();
		}

		virtual std::istream* getStream (void) const final;
		virtual bool isFinished (void) const final;

		virtual void lock (void) final;
		virtual void unlock (void) final;

	protected:
		/**
		 * This function is called after each write request. The writen data is contained in the provided stream.
		 * When finished is true, this is the last and final call (there will be no more data provided after that final call).
		 * returns true for each successfull write, if returned false this means there occured an error in the write and the action will be terminated.
		 */
		virtual bool write (std::istream *stream, const bool& finished) = 0;

	private:
	};
}

#endif /* _FILEHANDLER_HANDLER_HANDLERWRITERBASE_ */