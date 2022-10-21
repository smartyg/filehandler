#ifndef _X_GPSWRITER_HPP_
#define _X_GPSWRITER_HPP_

#include <memory>
#include <list>
#include <istream>
#include <gpx/Parser.h>
#include <gpsdata/types/ObjectTime.hpp>

#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>

#include "gpxplugin.hpp"

class GpxWriter final : public libgpsfile2::handler::HandlerWriterBase {
public:
	GpxWriter (const std::shared_ptr<GpxPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderRouteWriterBase>, const std::string&);
	~GpxWriter (void);

private:
	bool write (std::istream *, const bool&) override;

private:
	bool parseData (gpx::GPX *);

	std::shared_ptr<GpxPlugin> _base_instance;
	libgpsfile2::provider::ProviderRouteWriterBase *_provider;
	gpx::Report *_reporter;
	gpx::Parser *_parser;
	std::map<const std::string, const std::string> _namespace_map;

	void readExtensions (const std::list<gpx::Node *>&, const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, unsigned short = 0) const noexcept;
	const std::string_view getNamespaceUri (const std::string_view ns);
	libgpsfile2::provider::ProviderRouteWriterBase::RouteData getGuessedType (const std::string_view name);

	template<class T>
	void addExtensionData (T&, gpx::Extensions&);

	static void addData (auto&, const libgpsfile2::provider::ProviderRouteWriterBase::RouteData&, const gpx::Node&);
	static std::tuple<std::string_view, std::string_view> getNamespace (const std::string&);
};

#endif /* _X_GPSWRITER_HPP_ */
