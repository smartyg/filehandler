#ifndef _LIBGPSFILE2_TYPES_HANDLERCREATORFUNC_
#define _LIBGPSFILE2_TYPES_HANDLERCREATORFUNC_

#include <memory>
#include <string>
#include <functional>

#include "libgpsfile2/handler/HandlerBase.hpp"
#include "libgpsfile2/provider/ProviderBase.hpp"

namespace libgpsfile2 {
	class GpsfileManager;

	inline namespace types {
		typedef std::function<std::unique_ptr<handler::HandlerBase>(const std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path)> HandlerCreatorFunc;
	}
}

#endif /* _LIBGPSFILE2_TYPES_HANDLERCREATORFUNC_ */