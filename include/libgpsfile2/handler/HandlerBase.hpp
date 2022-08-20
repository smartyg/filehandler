#ifndef _X_LIBGPSFILE2_HANDLER_HANDLERBASE_
#define _X_LIBGPSFILE2_HANDLER_HANDLERBASE_

#include <string>
#include <streambuf>

#include <libgpsfile2/types/BasePlugin.hpp>
#include <libgpsfile2/utils/Iobuf.hpp>

namespace libgpsfile2::handler {
	// Base data handler class
	class HandlerBase : public libgpsfile2::BasePlugin {
	protected:
		HandlerBase (const std::string& path) {
			this->_path = path;
			this->_buf = new libgpsfile2::utils::Iobuf ();
		}

	public:
		virtual ~HandlerBase (void);
		virtual const std::string getPath (void) const final;
		virtual bool release (void) = 0;

	protected:
		std::streambuf *_buf;

	private:
		std::string _path;
	};
}

#endif /* _X_LIBGPSFILE2_HANDLER_HANDLERBASE_ */
