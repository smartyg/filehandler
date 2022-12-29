#ifndef _FILEHANDLER_TYPES_HANDLERCREATORFUNC_
#define _FILEHANDLER_TYPES_HANDLERCREATORFUNC_

#include <memory>
#include <string>
#include <functional>

#include "filehandler/handler/HandlerBase.hpp"
#include "filehandler/provider/ProviderBase.hpp"

namespace filehandler {
	class FileHandlerManager;

	inline namespace types {
		typedef std::function<std::unique_ptr<handler::HandlerBase>(const std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path)> HandlerCreatorFunc;
	}
}

#endif /* _FILEHANDLER_TYPES_HANDLERCREATORFUNC_ */