#ifndef _X_GPSREADER_HPP_
#define _X_GPSREADER_HPP_

#include <gpx/Writer.h>

#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>

#include "gpxplugin.hpp"

class GpxReader : public libgpsfile2::handler::HandlerReaderBase {
public:
	GpxReader (const std::shared_ptr<GpxPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderRouteReaderBase>, const std::string&);
	~GpxReader (void);

protected:
	bool read (std::ostream *, const std::size_t&) override;

private:
	std::shared_ptr<GpxPlugin> _base_instance;
	libgpsfile2::provider::ProviderRouteReaderBase *_provider;
	gpx::Writer *_parser;
	gpx::GPX *_root;
};

#endif /* _X_GPSREADER_HPP_ */