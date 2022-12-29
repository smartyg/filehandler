#ifndef _FILEHANDLER_TYPES_PLUGINCREATORFUNC_
#define _FILEHANDLER_TYPES_PLUGINCREATORFUNC_

#include <memory>
#include <functional>

namespace filehandler {
	class FileHandlerManager;

	inline namespace types {
		typedef std::function<void(const std::shared_ptr<FileHandlerManager>&)> PluginCreatorFunc;
	}
}

#endif /* _FILEHANDLER_TYPES_PLUGINCREATORFUNC_ */