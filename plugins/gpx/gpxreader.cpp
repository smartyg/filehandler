#include "config.h"
#include <features.h>

#include "gpxreader.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <ostream>
#include <gpx/Parser.h>
#include <gpx/Writer.h>
#include <gpsdata/utils/Logger.hpp>
#include <libgpsfile2/provider/ProviderRouteBase.hpp>
#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>

#include "gpxplugin.hpp"
#include "gpxreport.hpp"

using libgpsfile2::provider::ProviderRouteReaderBase;
//using libgpsfile2::provider::ProviderRouteBaseTypes;
using libgpsfile2::provider::internal::ProviderRouteBase;

GpxReader::GpxReader (const std::shared_ptr<GpxPlugin> base, std::unique_ptr<ProviderRouteReaderBase> dp, const std::string& path) : HandlerBase (path), HandlerReaderBase (std::move (dp), path) {
	this->_base_instance = base;
	//this->_dp = dp;
	this->_reporter = new GpxReport ();
	this->_parser = new gpx::Writer ();
	this->_root = new gpx::GPX ();
	this->_provider = dynamic_cast<ProviderRouteReaderBase*> (this->_dp.get ());
	this->_init = false;
	//this->_extension_namespaces.reserve (4);
}

GpxReader::~GpxReader (void) {
	this->_base_instance = nullptr;
	this->_dp.reset ();
	//this->_extension_namespaces.reset ();
	delete this->_parser;
	delete this->_root;
}

void GpxReader::addExtension (gpx::Extensions& extensions, gpx::Report* report, const ProviderRouteBase::RouteData& t, const std::string& data) {
	DEBUG_MSG("GpxReader::%s (..., %p, %d, %s)\n", __func__, report, t, data.c_str ());
	std::string_view name;
	std::string_view prefix;
	// Loop over registered extension namespaces
	for (const auto& [tag, uri, url] : this->_extension_namespaces) {
		//DEBUG_MSG("check if type %d is in namespace %s\n", t, std::string (uri).c_str ());
		name = this->_base_instance->namespaceNameLookup (uri, t);
		//DEBUG_MSG("  value of name: %s\n", std::string (name).c_str ());
		prefix = tag;
		if (!name.empty ()) break;
	}
	// check if this returns a valid (non-empty) name
	if (name.empty ()) {
		//DEBUG_MSG("No match found for type %d, check other namespaces\n", t);
		// itterate over all namespaces till a match is found and add namespace to namespace list
		for (const auto& [tag, uri, url] : this->_base_instance->allNamespaces ()) {
			//DEBUG_MSG("check if type %d is in namespace %s\n", t, std::string (uri).c_str ());
			name = this->_base_instance->namespaceNameLookup (uri, t);
			//DEBUG_MSG("  value of name: %s\n", std::string (name).c_str ());
			if (!name.empty ()) {
				//DEBUG_MSG("  found a match with tag: %s; name: %s\n", std::string (tag).c_str (), std::string (name).c_str ());
				prefix = tag;
				this->_extension_namespaces.push_back (std::tuple (tag, uri, url));
				break;
			}
		}
	}

	if (name.empty ()) return;

	std::ostringstream full_name;
	full_name << prefix << ":" << name << '\0';

	DEBUG_MSG("add tag: %s; data: %s\n", full_name.view ().data (), data.c_str ());
	//gpx::Node* node = extensions.add (full_name.view ().data (), gpx::Node::ELEMENT, report);
	//node->setValue (data);
	extensions.add (full_name.view ().data (), gpx::Node::ELEMENT, report)->setValue (data);
}

void GpxReader::loadRoutes (void) {
	this->_init = true;
	this->_root->add ("xmlns", gpx::Node::ATTRIBUTE)->setValue ("http://www.topografix.com/GPX/1/1"); // Some tools need this
	this->_root->add ("xmlns:xsi", gpx::Node::ATTRIBUTE)->setValue ("http://www.w3.org/2001/XMLSchema-instance");
	gpx::Node* schema_location = this->_root->add ("xsi:schemaLocation", gpx::Node::ATTRIBUTE);
	schema_location->setValue ("http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
	this->_root->version ().add (this->_reporter)->setValue ("1.1");
	this->_root->creator ().add (this->_reporter)->setValue ("libgpx");
	for (const auto& route : this->_provider->getRoutes ()) {
		gpx::TRK *trk = dynamic_cast<gpx::TRK*>(this->_root->trks ().add (this->_reporter));

		for (const auto& route_data : route.getData ()) {
			const ProviderRouteBase::RouteData& t = std::get<0>(route_data);
			const std::string& data = std::get<1>(route_data);
			std::cout << "    data type: " << std::to_string (t) << " value: " << data << std::endl;
			switch (t) {
				case ProviderRouteReaderBase::TYPE_NAME:
					trk->name ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_COMMENT:
					trk->cmt ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_DESCRIPTION:
					trk->desc ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_SRC:
					trk->src ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_LINK: {
					gpx::Link* link = dynamic_cast<gpx::Link*>(trk->links ().add (this->_reporter));
					link->href ().add (this->_reporter)->setValue (data);
					link->text ().add (this->_reporter)->setValue (data);
					break; }
				case ProviderRouteReaderBase::TYPE_ID:
					trk->number ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_TYPE:
					trk->type ().add (this->_reporter)->setValue (data);
					break;
				default:
					this->addExtension (trk->extensions (), this->_reporter, t, data);
					break;
			}
		}

		for (const auto& segment : route.getSegments ()) {
			gpx::TRKSeg *trkseg = dynamic_cast<gpx::TRKSeg *>(trk->trksegs ().add (this->_reporter));

			for (const auto& segment_data : segment.getData ()) {
				const auto& t = std::get<0>(segment_data);
				const std::string& data = std::get<1>(segment_data);
				std::cout << "    data type: " << std::to_string (t) << " value: " << data << std::endl;
				this->addExtension (trkseg->extensions (), this->_reporter, t, data);
			}

			for (const auto& point : segment.getPoints ()) {
				gpx::WPT *trkpt = dynamic_cast<gpx::WPT*>(trkseg->trkpts ().add (this->_reporter));

				for (const auto& point_data : point.getData ()) {
					const auto& t = std::get<0>(point_data);
					const std::string& data = std::get<1>(point_data);
					std::cout << "    data type: " << std::to_string (t) << " value: " << data << std::endl;
					switch (t) {
						case ProviderRouteReaderBase::TYPE_LAT:
							trkpt->lat ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_LON:
							trkpt->lon ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_ELE:
							trkpt->ele ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_TIME:
							trkpt->time ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_MAGVAR:
							trkpt->magvar ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_GEOIDHEIGHT:
							trkpt->geoidheight ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_NAME:
							trkpt->name ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_COMMENT:
							trkpt->cmt ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_DESCRIPTION:
							trkpt->desc ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_SRC:
							trkpt->src ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_TYPE:
							trkpt->type ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_FIX:
							trkpt->fix ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_SAT:
							trkpt->sat ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_HDOP:
							trkpt->hdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_VDOP:
							trkpt->vdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_PDOP:
							trkpt->pdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_AGEOFDGPSDATA:
							trkpt->ageofdgpsdata ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_DGPSID:
							trkpt->dgpsid ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_LINK: {
							gpx::Link* link = dynamic_cast<gpx::Link*>(trkpt->links ().add (this->_reporter));
							link->href ().add (this->_reporter)->setValue (data);
							link->text ().add (this->_reporter)->setValue (data);
							break; }
						default:
							this->addExtension (trkpt->extensions (), this->_reporter, t, data);
							break;
					}
				}
			}
		}
	}

	for (const auto& [name, uri, location] : this->_extension_namespaces) {
		std::ostringstream ss_name;
		ss_name << "xmlns:" << name << '\0';
		this->_root->add (ss_name.view ().data (), gpx::Node::ATTRIBUTE)->setValue (std::move (std::string (uri)));
		//this->_root->add ("xsi:schemaLocation", gpx::Node::ATTRIBUTE)
		std::ostringstream ss_schema;
		ss_schema << " " << uri << " " << location << '\0';
		schema_location->appendValue (ss_schema.view ().data ());
	}
}

bool GpxReader::read (std::ostream *s, const std::size_t&) {
	if (!this->_init) this->loadRoutes ();



	//GpsRoute_ptr_const route = this->_dp->getRoute ();
	//(void) route;
/*
	gpx::Node *attribute = this->_root->version().add();
	gpx::WPT  *wpt       = this->_root->wpts().add(&cerr);
	gpx::Node *extension = this->_root->add("xmlns",gpx::Node::ATTRIBUTE);

	attribute->setValue("1.1");
	wpt->lat().add()->setValue("1.60000");
	extension->setValue("http://www.topografix.com/GPX/1/1");
*/
	this->_parser->write (*s, this->_root, true);

	return true;
}
