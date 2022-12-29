#ifndef _PLUGIN_GPX_READER_
#define _PLUGIN_GPX_READER_

#include <memory>
#include <string>
#include <ostream>
#include <gpx/GPX.h>
#include <gpx/Writer.h>
#include <gpx/Extensions.h>

#include <filehandler/handler/HandlerReaderBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteReaderBase.hpp>

#include "Plugin.hpp"
#include "Report.hpp"

namespace plugin::gpx {
	class Reader : public filehandler::handler::HandlerReaderBase {
	private:
		std::shared_ptr<Plugin> _base_instance;
		filehandler::provider::ProviderRouteReaderBase *_provider;
		::gpx::Report *_reporter;
		::gpx::Writer *_parser;
		::gpx::GPX *_root;
		bool _init;
		std::list<std::tuple<const std::string_view, const std::string_view, const std::string_view>> _extension_namespaces;

	public:
		Reader (const std::shared_ptr<Plugin>, std::unique_ptr<filehandler::provider::ProviderRouteReaderBase>, const std::string&);
		~Reader (void);

		void loadRoutes (void);

	protected:
		bool read (std::ostream *, const std::size_t&) override;

	private:
		void addExtension (::gpx::Extensions*, ::gpx::Report*, const filehandler::provider::internal::ProviderRouteBase::RouteData&, const std::string&);
	};
}

#endif /* _PLUGIN_GPX_READER_ */