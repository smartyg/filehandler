#ifndef _X_LIBGPSFILE2_DATAHANDLER_ROUTE_
#define _X_LIBGPSFILE2_DATAHANDLER_ROUTE_

//#include <cstddef>
//#include <ostream>
//#include <istream>
#include <memory>
#include <string>
//#include <map>

//#include "libgpsfile2/BasePlugin.hpp"
#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"

namespace libgpsfile2 {
#if 0
	class HandlerRouteReaderBase : virtual public HandlerReaderBase {
	public:
		//HandlerRouteReaderBase (std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
		HandlerRouteReaderBase (std::unique_ptr<provider::ProviderReaderBase> provider, const std::string& path) : HandlerReaderBase (std::move (provider), path) {}

		virtual ~HandlerRouteReaderBase (void) = default;
	};

	class HandlerRouteWriterBase : virtual public HandlerWriterBase {
	public:
		//HandlerRouteWriterBase (std::unique_ptr<provider::ProviderWriterBase>, const std::string&);
		HandlerRouteWriterBase (std::unique_ptr<provider::ProviderWriterBase> provider, const std::string& path) : HandlerWriterBase (std::move (provider), path) {}
		virtual ~HandlerRouteWriterBase (void) = default;
	};

	class BaseDatahandlerWaypointReaderPlugin : virtual public BaseDatahandlerReaderPlugin {
	public:
		BaseDatahandlerWaypointReaderPlugin (const std::string&);
		virtual ~BaseDatahandlerWaypointReaderPlugin (void) = default;
	};

	class BaseDatahandlerTableReaderPlugin : virtual public BaseDatahandlerReaderPlugin {
	public:
		BaseDatahandlerTableReaderPlugin (const std::string&);
		virtual ~BaseDatahandlerTableReaderPlugin (void) = default;
	};
#endif
}

#endif /* _X_LIBGPSFILE2_DATAHANDLER_ROUTE_ */
