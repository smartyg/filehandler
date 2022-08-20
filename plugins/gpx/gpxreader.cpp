#include "config.h"
#include <features.h>

#include "gpxreader.hpp"

#include <memory>
#include <ostream>
#include <gpx/Writer.h>
#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>

#include "gpxplugin.hpp"

using libgpsfile2::provider::ProviderRouteReaderBase;

GpxReader::GpxReader (const std::shared_ptr<GpxPlugin> base, std::unique_ptr<ProviderRouteReaderBase> dp, const std::string& path) : HandlerBase (path), HandlerReaderBase (std::move (dp), path) {
	this->_base_instance = base;
	//this->_dp = dp;
	this->_parser = new gpx::Writer ();
	this->_root = new gpx::GPX ();
	this->_provider = dynamic_cast<ProviderRouteReaderBase*> (this->_dp.get ());
}

GpxReader::~GpxReader (void) {
	this->_base_instance = nullptr;
	this->_dp.reset ();
	delete this->_parser;
	delete this->_root;
}

bool GpxReader::read (std::ostream *s, const std::size_t&) {

	//GpsRoute_ptr_const route = this->_dp->getRoute ();
	//(void) route;
/*
	gpx::Node *attribute = root->version().add();
	gpx::WPT  *wpt       = root->wpts().add(&cerr);
	gpx::Node *extension = root->add("xmlns",gpx::Node::ATTRIBUTE);

	attribute->setValue("1.1");
	wpt->lat().add()->setValue("1.60000");
	extension->setValue("http://www.topografix.com/GPX/1/1");
*/
	this->_parser->write(*s, this->_root, true);

	return true;
}
