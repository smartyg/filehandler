#ifndef _X_GPSREADER_HPP_
#define _X_GPSREADER_HPP_

#include <gpx/Writer.h>

#include <libgpsfile2/DatahandlerRoute.hpp>

#include "gpxplugin.hpp"

class GpxReader : public libgpsfile2::HandlerReaderBase {
public:
	GpxReader (const std::shared_ptr<const GpxPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderReaderBase>, const std::string&);
	~GpxReader (void);

protected:
	bool read (std::ostream *, const std::size_t&) override;

private:
	std::shared_ptr<const GpxPlugin> _base_instance;
	//std::unique_ptr<const libgpsfile2::provider::ProviderRouteReaderBase> _dp;
	gpx::Writer *_parser;
	gpx::GPX *_root;
};

#endif /* _X_GPSREADER_HPP_ */