#include "config.h"

#include "gpxwriter.hpp"

#include <memory>
#include <string>
#include <list>
#include <istream>
#include <utility>
#include <gpx/Parser.h>
#include <Logger.hpp>
#include <gpsdata/utils/PointDate.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>
#include <libgpsfile2/utils/Iobuf.hpp>

#include "gpxplugin.hpp"
#include "gpxreport.hpp"

#define GPX_TIME_FORMATS { "%Y-%m-{:d}T%TZ" }

using libgpsfile2::provider::ProviderRouteWriterBase;
using libgpsfile2::utils::Iobuf;

GpxWriter::GpxWriter (const std::shared_ptr<GpxPlugin> base, std::unique_ptr<ProviderRouteWriterBase> dp, const std::string& path) : HandlerBase (path), HandlerWriterBase (std::move (dp), path) {
	DEBUG_MSG ("GpxWriter::{:s} ()\n", __func__);
	this->_base_instance = base;
	this->_reporter = new GpxReport ();
	this->_parser = new gpx::Parser (this->_reporter);
	this->_provider = dynamic_cast<ProviderRouteWriterBase*> (this->_dp.get ());
}

GpxWriter::~GpxWriter (void) {
	DEBUG_MSG ("GpxWriter::{:s} ()\n", __func__);
	this->_base_instance = nullptr;
	this->_provider = nullptr;
	//this->_dp = nullptr;
	if (this->_reporter != nullptr) delete this->_reporter;
	if (this->_parser != nullptr) delete this->_parser;
}

bool GpxWriter::write (std::istream *s, const bool& is_final) {
	DEBUG_MSG ("GpxWriter::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (s), is_final);
	Iobuf *sbuf = reinterpret_cast<Iobuf *>(s->rdbuf ());
	const char *b = sbuf->getBuffer ();
	size_t l = sbuf->in_avail ();

	bool res = this->_parser->parse (b, l, is_final);
	if (!res) ERROR_MSG("Parsing of {:s} failed due to {:s} on line {:s} and column {:s}\n", this->getPath (), this->_parser->errorText (), this->_parser->errorLineNumber (), this->_parser->errorColumnNumber ());

	sbuf->consumeGet (l);

	if (is_final && res) {
		res = this->parseData (this->_parser->root ());
		delete this->_parser;
		this->_parser = nullptr;
		this->_provider->finished ();
	}
	return res;
}

void GpxWriter::addData (auto& obj, const ProviderRouteWriterBase::RouteData& t, const gpx::Node& data) {
	DEBUG_MSG ("GpxWriter::{:s} ({:d}, {:p})\n", __func__, t, fmt::ptr (&data));
	if (!data.getValue ().empty ()) {
		if (obj.addData (t, data.getValue ()))
			DEBUG_2_MSG (1, "GpxWriter::{:s}: success setting type {:d} to: {:s}\n", __func__, t, data.getValue ());
		else
			INFO_MSG("GpxWriter::{:s}: failed setting type {:d} to: {:s}\n", __func__, t, data.getValue ());
	}
}

std::tuple<std::string_view, std::string_view> GpxWriter::getNamespace (const std::string& full_name) {
	DEBUG_MSG ("GpxWriter::{:s} ({:s})\n", __func__, full_name);
	auto pos = full_name.find_first_of (':');
	return {
		std::string_view (full_name).substr (0, (pos == std::string::npos ? 0 : pos)),
		std::string_view (full_name).substr (pos + 1)
	};
}

const std::string_view GpxWriter::getNamespaceUri (const std::string_view ns) {
	DEBUG_MSG ("GpxWriter::{:s} ({:s})\n", __func__, ns);
	for (const auto& [key, value] : this->_namespace_map) {
		if (key.compare (ns) == 0) return std::string_view (value);
	}
	return std::string_view ();
}

ProviderRouteWriterBase::RouteData GpxWriter::getGuessedType (const std::string_view name) {
	DEBUG_MSG ("GpxWriter::{:s} ({:s})\n", __func__, name);
	ProviderRouteWriterBase::RouteData t = ProviderRouteWriterBase::TYPE_NO_TYPE;
	if ((t = ProviderRouteWriterBase::getType (name)) != ProviderRouteWriterBase::TYPE_NO_TYPE) return t;
	return t;
}

template<class T>
void GpxWriter::addExtensionData (T& route, gpx::Extensions& extensions) {
	DEBUG_MSG ("GpxWriter::{:s} ({:p}, {:p})\n", __func__, fmt::ptr (&route), fmt::ptr (&extensions));

	for (const gpx::Node* extension : extensions.getElements ()) {
		const std::string& full_name = extension->getName ();
		const std::string& value = extension->getValue ();

		const auto [ns, name] = getNamespace (full_name);

		const std::string_view ns_uri = this->getNamespaceUri (ns);
		ProviderRouteWriterBase::RouteData t = this->_base_instance->namespaceTypeLookup (ns_uri, name);
		if (t == ProviderRouteWriterBase::TYPE_NO_TYPE)
			t = this->getGuessedType (name);

		NOTICE_MSG ("track point extension namespace: {:s} ({:s}); name: {:s}; type: {:d}; value: {:s}.\n", ns,
				   ns_uri,
				   name,
				   t, value);
		GpxWriter::addData (route, t, *extension);

		NOTICE_MSG ("track point extension namespace: {:s}; name: {:s}; value: {:s}.\n", ns, name, value);
	}
}

bool GpxWriter::parseData (gpx::GPX *root) {
	DEBUG_MSG("GpxWriter::{:s} ({:p})\n", __func__, fmt::ptr (root));
	if (root->trks().list().size() < 1) {
		NOTICE_MSG ("No tracks have been found in file {:s}.\n", this->getPath ());
		return false;
	}

	assert (this->_provider);

	for (const gpx::Node* e : root->getAttributes ()) {
		if (e->getName ().compare (0, 6, "xmlns:") == 0) {
			NOTICE_MSG ("namespace add: {:s}; value: {:s}.\n", e->getName (). substr (6), e->getValue ());
			this->_namespace_map.insert (std::pair (e->getName (). substr (6), e->getValue ()));
		}
	}

	for (gpx::TRK *track : root->trks().list()) {
		auto route = this->_provider->addRoute ();
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_NAME, track->name ());
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_COMMENT, track->cmt ());
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_DESCRIPTION, track->desc ());
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_SRC, track->src ());
		for (gpx::Link *link : track->links ().list ())
			GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_LINK, link->href ());
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_ID, track->number ());
		GpxWriter::addData (route, ProviderRouteWriterBase::TYPE_TYPE, track->type ());
		this->addExtensionData (route, track->extensions ());

		for (gpx::TRKSeg *track_segment : track->trksegs ().list ()) {
			auto segment = route.addSegment ();
			this->addExtensionData (segment, track_segment->extensions ());

			for (gpx::WPT *segment_point : track_segment->trkpts ().list ()) {
				auto point = segment.addPoint ();
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_LAT, segment_point->lat ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_LON, segment_point->lon ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_ELE, segment_point->ele ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_TIME, segment_point->time ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_MAGVAR, segment_point->magvar ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_GEOIDHEIGHT, segment_point->geoidheight ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_NAME, segment_point->name ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_COMMENT, segment_point->cmt ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_DESCRIPTION, segment_point->desc ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_SRC, segment_point->src ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_TYPE, segment_point->type ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_FIX, segment_point->fix ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_SAT, segment_point->sat ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_HDOP, segment_point->hdop ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_VDOP, segment_point->vdop ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_PDOP, segment_point->pdop ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_AGEOFDGPSDATA, segment_point->ageofdgpsdata ());
				GpxWriter::addData (point, ProviderRouteWriterBase::TYPE_DGPSID, segment_point->dgpsid ());
				this->addExtensionData (point, segment_point->extensions ());
				point.finish ();
			}
			segment.finish ();
		}
		route.finish ();
	}
	return true;
}
#if 0
void GpxWriter::readExtensions (const std::list<gpx::Node *>& extensions, const unsigned short& route, const unsigned short& segment, const ObjectTime& time, unsigned short ns_len) const noexcept {
	for (std::list<gpx::Node *>::const_iterator iter = extensions.begin(); iter != extensions.end(); ++iter) {
		std::string name = (*iter)->getName ();
		std::string value = (*iter)->getValue ();

		if (ns_len == 0) {
			if (name.compare (0, 7, "gpxtpx:", 7) == 0) ns_len = 7;
			else if (name.compare (0, 8, "gpxdata:", 8) == 0) ns_len = 8;
		}

		NOTICE_MSG ("track point extension type: {:s}; value: {:s}.\n", name.data () + ns_len, value);

		if (name.compare (ns_len, std::string::npos, "temp") == 0) this->_provider->addTrackPointData (route, segment, time, "TEMPERATURE", value);
		else if (name.compare (ns_len, std::string::npos, "atemp") == 0) this->_provider->addTrackPointData (route, segment, time, "TEMPERATURE", value);
		//else if (name.compare (ns_len, std::string::npos, "wtemp") == 0) this->_provider->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		//else if (name.compare (ns_len, std::string::npos, "depth") == 0) this->_provider->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		else if (name.compare (ns_len, std::string::npos, "hr") == 0) this->_provider->addTrackPointData (route, segment, time, "HEARTRATE", value);
		else if (name.compare (ns_len, std::string::npos, "cad") == 0) this->_provider->addTrackPointData (route, segment, time, "CADANCE", value);
		else if (name.compare (ns_len, std::string::npos, "speed") == 0) this->_provider->addTrackPointData (route, segment, time, "SPEED", value);
		//else if (name.compare (ns_len, std::string::npos, "course") == 0) this->_provider->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		//else if (name.compare (ns_len, std::string::npos, "bearing") == 0) this->_provider->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		else if (name.compare (ns_len, std::string::npos, "SeaLevelPressure") == 0) this->_provider->addTrackPointData (route, segment, time, "AIR_PRESSURE", value);
		else if (name.compare (ns_len, std::string::npos, "TrackPointExtension") == 0) this->readExtensions ((*iter)->getElements (), route, segment, time, ns_len);
	}
}
#endif
