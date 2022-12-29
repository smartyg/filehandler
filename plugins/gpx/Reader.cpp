#include "config.h"

#include "Reader.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <ostream>
#include <gpx/Parser.h>
#include <gpx/Writer.h>
#include <Logger.hpp>
#include <filehandler/provider/types/route/ProviderRouteBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteReaderBase.hpp>

#include "Plugin.hpp"
#include "Report.hpp"

using plugin::gpx::Reader;
using plugin::gpx::Plugin;
using plugin::gpx::Report;
using filehandler::provider::ProviderRouteReaderBase;
using filehandler::provider::internal::ProviderRouteBase;

Reader::Reader (const std::shared_ptr<Plugin> base, std::unique_ptr<ProviderRouteReaderBase> dp, const std::string& path) : HandlerBase (path), HandlerReaderBase (std::move (dp), path) {
	DEBUG_MSG ("Reader::{:s} ({:p}, {:p}, {:s})\n", __func__, fmt::ptr (base), fmt::ptr (dp), path);
	this->_base_instance = base;
	this->_reporter = new Report ();
	this->_parser = new ::gpx::Writer ();
	this->_root = new ::gpx::GPX ();
	this->_provider = dynamic_cast<ProviderRouteReaderBase*> (this->_dp.get ());
	this->_init = false;
}

Reader::~Reader (void) {
	DEBUG_MSG ("Reader::{:s} ()\n", __func__);
	this->_base_instance = nullptr;
	this->_dp.reset ();
	delete this->_parser;
	delete this->_root;
	delete this->_reporter;
}

void Reader::addExtension (::gpx::Extensions* extensions, ::gpx::Report* report, const ProviderRouteBase::RouteData& t, const std::string& data) {
	DEBUG_MSG ("Reader::{:s} ({:p}, {:p}, {:d}, {:s})\n", __func__, fmt::ptr (extensions), fmt::ptr (report), t, data);
	std::string_view name;
	std::string_view prefix;
	// Loop over registered extension namespaces
	for (const auto& [tag, uri, url] : this->_extension_namespaces) {
		DEBUG_2_MSG (1, "check if type {:d} is in namespace {:s}\n", t, uri);
		name = this->_base_instance->namespaceNameLookup (uri, t);
		DEBUG_2_MSG(1, "  value of name: {:s}\n", name);
		prefix = tag;
		if (!name.empty ()) break;
	}
	// check if this returns a valid (non-empty) name
	if (name.empty ()) {
		DEBUG_2_MSG (1, "No match found for type {:d}, check other namespaces\n", t);
		// itterate over all namespaces till a match is found and add namespace to namespace list
		for (const auto& [tag, uri, url] : this->_base_instance->allNamespaces ()) {
			DEBUG_2_MSG(1, "check if type {:d} is in namespace {:s}\n", t, uri);
			name = this->_base_instance->namespaceNameLookup (uri, t);
			DEBUG_2_MSG(1, "  value of name: {:s}\n", name);
			if (!name.empty ()) {
				DEBUG_2_MSG(1, "  found a match with tag: {:s}; name: {:s}\n", tag, name);
				prefix = tag;
				this->_extension_namespaces.push_back (std::tuple (tag, uri, url));
				break;
			}
		}
	}

	if (name.empty ()) { // None of the used nor availible namespaces have a field to cover this type. Now just store this type as a non-namespace extension.
		name = ProviderRouteBase::getTypeName (t);
		prefix = {};
	}

	if (name.empty ()) return;

	std::string_view full_name;
	if (prefix.size () > 0) {
		std::ostringstream full_name_stream;
		full_name_stream << prefix << ":" << name;
		full_name = full_name_stream.view ();
	} else {
		full_name = name;
	}

	DEBUG_2_MSG (2, "add tag: {:s}; data: {:s}\n", full_name, data);
	extensions->add (std::string (full_name).c_str (), ::gpx::Node::ELEMENT, report)->setValue (data);
}

void Reader::loadRoutes (void) {
	DEBUG_MSG ("Reader::{:s} ()\n", __func__);
	this->_init = true;
	this->_root->add ("xmlns", ::gpx::Node::ATTRIBUTE)->setValue ("http://www.topografix.com/GPX/1/1"); // Some tools need this
	this->_root->add ("xmlns:xsi", ::gpx::Node::ATTRIBUTE)->setValue ("http://www.w3.org/2001/XMLSchema-instance");
	::gpx::Node* schema_location = this->_root->add ("xsi:schemaLocation", ::gpx::Node::ATTRIBUTE);
	schema_location->setValue ("http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
	this->_root->version ().add (this->_reporter)->setValue ("1.1");
	this->_root->creator ().add (this->_reporter)->setValue ("libgpx");
	for (const auto& route : this->_provider->getRoutes ()) {
		::gpx::TRK *trk = dynamic_cast<::gpx::TRK*>(this->_root->trks ().add (this->_reporter));

		for (const auto& route_data : route.getData ()) {
			const ProviderRouteBase::RouteData& t = std::get<0>(route_data);
			const std::string& data = std::get<1>(route_data);
			DEBUG_2_MSG (2, "data type: {:d}; value: {:s}\n", static_cast<int>(t), data);
				//std::cout << "data type: " << std::to_string (t) << "; value: " << data << std::endl;
			switch (t) {
				case ProviderRouteReaderBase::TYPE_NAME:
					DEBUG_2_MSG (3, "  set name to: {:s}\n", data);
					trk->name ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_COMMENT:
					DEBUG_2_MSG (3, "  set comment to: {:s}\n", data);
					trk->cmt ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_DESCRIPTION:
					DEBUG_2_MSG (3, "  set description to: {:s}\n", data);
					trk->desc ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_SRC:
					DEBUG_2_MSG (3, "  set source type to: {:s}\n", data);
					trk->src ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_LINK: {
					DEBUG_2_MSG (3, "  set link to: {:s}\n", data);
					::gpx::Link* link = dynamic_cast<::gpx::Link*>(trk->links ().add (this->_reporter));
					link->href ().add (this->_reporter)->setValue (data);
					link->text ().add (this->_reporter)->setValue (data);
					break; }
				case ProviderRouteReaderBase::TYPE_ID:
					DEBUG_2_MSG (3, "  set id to: {:s}\n", data);
					trk->number ().add (this->_reporter)->setValue (data);
					break;
				case ProviderRouteReaderBase::TYPE_TYPE:
					DEBUG_2_MSG (3, "  set type to: {:s}\n", data);
					trk->type ().add (this->_reporter)->setValue (data);
					break;
				default:
					::gpx::Extensions* e =  dynamic_cast<::gpx::Extensions*>(trk->extensions ().add (this->_reporter));
					this->addExtension (e, this->_reporter, t, data);
					break;
			}
		}

		for (const auto& segment : route.getSegments ()) {
			::gpx::TRKSeg *trkseg = dynamic_cast<::gpx::TRKSeg *>(trk->trksegs ().add (this->_reporter));

			for (const auto& segment_data : segment.getData ()) {
				const auto& t = std::get<0>(segment_data);
				const std::string& data = std::get<1>(segment_data);
				DEBUG_2_MSG (2, "data type: {:d}; value: {:s}\n", static_cast<int>(t), data);
				::gpx::Extensions* e =  dynamic_cast<::gpx::Extensions*>(trkseg->extensions ().add (this->_reporter));
				this->addExtension (e, this->_reporter, t, data);
			}

			for (const auto& point : segment.getPoints ()) {
				::gpx::WPT *trkpt = dynamic_cast<::gpx::WPT*>(trkseg->trkpts ().add (this->_reporter));

				for (const auto& point_data : point.getData ()) {
					const auto& t = std::get<0>(point_data);
					const std::string& data = std::get<1>(point_data);
					//DEBUG_MSG ("data type: {:d}; value: {:s}\n", __func__, static_cast<int>(t), data.c_str ());
					DEBUG_MSG ("data type: {:d}; value: {:s}\n", static_cast<int>(t), data.c_str ());
					switch (t) {
						case ProviderRouteReaderBase::TYPE_LAT:
							DEBUG_2_MSG (3, "  set latitude to: {:s}\n", data);
							trkpt->lat ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_LON:
							DEBUG_2_MSG (3, "  set longitude to: {:s}\n", data);
							trkpt->lon ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_ELE:
							DEBUG_2_MSG (3, "  set elevation to: {:s}\n", data);
							trkpt->ele ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_TIME:
							DEBUG_2_MSG (3, "  set time to: {:s}\n", data);
							trkpt->time ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_MAGVAR:
							DEBUG_2_MSG (3, "  set magver to: {:s}\n", data);
							trkpt->magvar ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_GEOIDHEIGHT:
							DEBUG_2_MSG (3, "  set geoidheight to: {:s}\n", data);
							trkpt->geoidheight ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_NAME:
							DEBUG_2_MSG (3, "  set name to: {:s}\n", data);
							trkpt->name ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_COMMENT:
							DEBUG_2_MSG (3, "  set comment to: {:s}\n", data);
							trkpt->cmt ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_DESCRIPTION:
							DEBUG_2_MSG (3, "  set description to: {:s}\n", data);
							trkpt->desc ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_SRC:
							DEBUG_2_MSG (3, "  set source type to: {:s}\n", data);
							trkpt->src ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_TYPE:
							DEBUG_2_MSG (3, "  set type to: {:s}\n", data);
							trkpt->type ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_FIX:
							DEBUG_2_MSG (3, "  set fix to: {:s}\n", data);
							trkpt->fix ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_SAT:
							DEBUG_2_MSG (3, "  set sat to: {:s}\n", data);
							trkpt->sat ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_HDOP:
							DEBUG_2_MSG (3, "  set hdop to: {:s}\n", data);
							trkpt->hdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_VDOP:
							DEBUG_2_MSG (3, "  set vdop to: {:s}\n", data);
							trkpt->vdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_PDOP:
							DEBUG_2_MSG (3, "  set pdop to: {:s}\n", data);
							trkpt->pdop ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_AGEOFDGPSDATA:
							DEBUG_2_MSG (3, "  set ageofdgpsdata to: {:s}\n", data);
							trkpt->ageofdgpsdata ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_DGPSID:
							DEBUG_2_MSG (3, "  set dgpsid to: {:s}\n", data);
							trkpt->dgpsid ().add (this->_reporter)->setValue (data);
							break;
						case ProviderRouteReaderBase::TYPE_LINK: {
							DEBUG_2_MSG (3, "  set link to: {:s}\n", data);
							::gpx::Link* link = dynamic_cast<::gpx::Link*>(trkpt->links ().add (this->_reporter));
							link->href ().add (this->_reporter)->setValue (data);
							link->text ().add (this->_reporter)->setValue (data);
							break; }
						default:
							::gpx::Extensions* e =  dynamic_cast<::gpx::Extensions*>(trkpt->extensions ().add (this->_reporter));
							this->addExtension (e, this->_reporter, t, data);
							break;
					}
				}
			}
		}
	}

	for (const auto& [name, uri, location] : this->_extension_namespaces) {
		std::ostringstream ss_name;
		ss_name << "xmlns:" << name;
		this->_root->add (std::string (ss_name.view ()).c_str (), ::gpx::Node::ATTRIBUTE)->setValue (std::move (std::string (uri)));
		//this->_root->add ("xsi:schemaLocation", gpx::Node::ATTRIBUTE)
		std::ostringstream ss_schema;
		ss_schema << " " << uri << " " << location;
		schema_location->appendValue (std::string (ss_schema.view ()).c_str ());
	}
}

bool Reader::read (std::ostream *s, const std::size_t&) {
	if (!this->_init) this->loadRoutes ();

	this->_parser->write (*s, this->_root, true);

	return true;
}
