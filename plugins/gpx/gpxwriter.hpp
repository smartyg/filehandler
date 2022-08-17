#ifndef _X_GPSWRITER_HPP_
#define _X_GPSWRITER_HPP_

#include <list>
#include <memory>
#include <gpx/Parser.h>
#include <gpsdata/types/ObjectTime.hpp>

#include <libgpsfile2/DatahandlerRoute.hpp>

#include "gpxplugin.hpp"

class GpxWriter: public libgpsfile2::HandlerWriterBase {
public:
	GpxWriter (const std::shared_ptr<const GpxPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderWriterBase>, const std::string&);
	~GpxWriter (void);

private:
	bool write (std::istream *, const bool&) override;

private:
	bool parseData (gpx::GPX *);

	std::shared_ptr<const GpxPlugin> _base_instance;
	//std::unique_ptr<libgpsfile2::provider::ProviderRouteWriterBase> _dp;
	gpx::Report *_reporter;
	gpx::Parser *_parser;

	void readExtensions (const std::list<gpx::Node *>&, const unsigned short&, const unsigned short&, const gpsdata::ObjectTime&, unsigned short = 0) const noexcept;
};

#endif /* _X_GPSWRITER_HPP_ */
