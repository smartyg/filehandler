#ifndef _FILEHANDLER_HANDLER_HANDLERBASE_
#define _FILEHANDLER_HANDLER_HANDLERBASE_

#include <string>
#include <streambuf>

#include <filehandler/types/BasePlugin.hpp>
#include <filehandler/utils/Iobuf.hpp>

namespace filehandler::handler {
	// Base data handler class
	class HandlerBase : public filehandler::BasePlugin {
	protected:
		HandlerBase (const std::string& path) : _path (path) {
			this->_buf = new filehandler::utils::Iobuf ();
		}

	public:
		virtual ~HandlerBase (void);
		virtual const std::string getPath (void) const final;
		virtual bool release (void) = 0;

	protected:
		std::streambuf *_buf;

	private:
		const std::string _path;
	};
}

#endif /* _FILEHANDLER_HANDLER_HANDLERBASE_ */
