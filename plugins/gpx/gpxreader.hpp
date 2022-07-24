#ifndef _X_GPSREADER_HPP_
#define _X_GPSREADER_HPP_

#include <gpx/Writer.h>

#include "libgpsfile2.hpp"

#include "gpxplugin.hpp"

class GpxReader : public libgpsfile2::BaseDatahandlerRouteReaderPlugin {
public:
	GpxReader (const std::shared_ptr<const GpxPlugin>&, const std::shared_ptr<const libgpsfile2::provider::ProviderRouteReaderBase>&, const std::string&);
	~GpxReader (void);

protected:
	bool read (std::ostream *, const std::size_t&) override;

private:
	std::shared_ptr<const GpxPlugin> _base_instance;
	std::shared_ptr<const libgpsfile2::provider::ProviderRouteReaderBase> _dp;
	gpx::Writer *_parser;
	gpx::GPX *_root;
};

#endif /* _X_GPSREADER_HPP_ */