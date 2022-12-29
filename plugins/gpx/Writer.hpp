#ifndef _PLUGIN_GPX_WRITER_
#define _PLUGIN_GPX_WRITER_

#include <memory>
#include <list>
#include <istream>
#include <gpx/Parser.h>
#include <gpsdata/types/ObjectTime.hpp>

#include <filehandler/handler/HandlerWriterBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"

namespace plugin::gpx {
	class Writer final : public filehandler::handler::HandlerWriterBase {
	public:
		Writer (const std::shared_ptr<Plugin>, std::unique_ptr<filehandler::provider::ProviderRouteWriterBase>, const std::string&);
		~Writer (void);

	private:
		bool write (std::istream *, const bool&) override;

	private:
		bool parseData (::gpx::GPX *);

		std::shared_ptr<Plugin> _base_instance;
		filehandler::provider::ProviderRouteWriterBase *_provider;
		::gpx::Report *_reporter;
		::gpx::Parser *_parser;
		std::map<const std::string, const std::string> _namespace_map;

		void readExtensions (const std::list<::gpx::Node *>&, const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, unsigned short = 0) const noexcept;
		const std::string_view getNamespaceUri (const std::string_view ns);
		filehandler::provider::ProviderRouteWriterBase::RouteData getGuessedType (const std::string_view name);

		template<class T>
		void addExtensionData (T&, ::gpx::Extensions&);

		static void addData (auto&, const filehandler::provider::ProviderRouteWriterBase::RouteData&, const ::gpx::Node&);
		static std::tuple<std::string_view, std::string_view> getNamespace (const std::string&);
	};
}

#endif /* _PLUGIN_GPX_WRITER_ */
