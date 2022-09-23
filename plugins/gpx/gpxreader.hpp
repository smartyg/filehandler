#ifndef _X_GPSREADER_HPP_
#define _X_GPSREADER_HPP_

#include <memory>
#include <string>
#include <ostream>
#include <gpx/GPX.h>
#include <gpx/Writer.h>
#include <gpx/Extensions.h>

#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>

#include "gpxplugin.hpp"

class GpxReader : public libgpsfile2::handler::HandlerReaderBase {
private:
	std::shared_ptr<GpxPlugin> _base_instance;
	libgpsfile2::provider::ProviderRouteReaderBase *_provider;
	gpx::Report *_reporter;
	gpx::Writer *_parser;
	gpx::GPX *_root;
	bool _init;
	std::list<std::tuple<const std::string_view, const std::string_view, const std::string_view>> _extension_namespaces;

public:
	GpxReader (const std::shared_ptr<GpxPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderRouteReaderBase>, const std::string&);
	~GpxReader (void);

	void loadRoutes (void);

protected:
	bool read (std::ostream *, const std::size_t&) override;

private:
	void addExtension (gpx::Extensions&, gpx::Report*, const libgpsfile2::provider::internal::ProviderRouteBase::RouteData&, const std::string&);
};

#endif /* _X_GPSREADER_HPP_ */