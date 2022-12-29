#include "config.h"

#include "Writer.hpp"

#include <memory>
#include <string>
#include <list>
#include <istream>
#include <utility>
#include <gpx/Parser.h>
#include <Logger.hpp>
#include <gpsdata/utils/PointDate.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>
#include <filehandler/utils/Iobuf.hpp>

#include "Plugin.hpp"
#include "Report.hpp"

#define GPX_TIME_FORMATS { "%Y-%m-{:d}T%TZ" }

using plugin::gpx::Plugin;
using plugin::gpx::Report;
using plugin::gpx::Writer;
using filehandler::provider::ProviderRouteWriterBase;
using filehandler::utils::Iobuf;

Writer::Writer (const std::shared_ptr<Plugin> base, std::unique_ptr<ProviderRouteWriterBase> dp, const std::string& path) : HandlerBase (path), HandlerWriterBase (std::move (dp), path) {
	DEBUG_MSG ("Writer::{:s} ()\n", __func__);
	this->_base_instance = base;
	this->_reporter = new Report ();
	this->_parser = new ::gpx::Parser (this->_reporter);
	this->_provider = dynamic_cast<ProviderRouteWriterBase*> (this->_dp.get ());
}

Writer::~Writer (void) {
	DEBUG_MSG ("Writer::{:s} ()\n", __func__);
	this->_base_instance = nullptr;
	this->_provider = nullptr;
	//this->_dp = nullptr;
	if (this->_reporter != nullptr) delete this->_reporter;
	if (this->_parser != nullptr) delete this->_parser;
}

bool Writer::write (std::istream *s, const bool& is_final) {
	DEBUG_MSG ("Writer::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (s), is_final);
	Iobuf *sbuf = reinterpret_cast<Iobuf *>(s->rdbuf ());
	const char *b = sbuf->getBuffer ();
	size_t l = sbuf->in_avail ();

	bool res = this->_parser->parse (b, l, is_final);
	if (!res) ERROR_MSG ("Parsing of {:s} failed due to {:s} on line {:d} and column {:d}\n", this->getPath (), this->_parser->errorText (), this->_parser->errorLineNumber (), this->_parser->errorColumnNumber ());

	sbuf->consumeGet (l);

	if (is_final && res) {
		res = this->parseData (this->_parser->root ());
		delete this->_parser;
		this->_parser = nullptr;
		this->_provider->finished ();
	}
	return res;
}

void Writer::addData (auto& obj, const ProviderRouteWriterBase::RouteData& t, const ::gpx::Node& data) {
	DEBUG_MSG ("Writer::{:s} ({:d}, {:p})\n", __func__, t, fmt::ptr (&data));
	if (!data.getValue ().empty ()) {
		if (obj.addData (t, data.getValue ()))
			DEBUG_2_MSG (1, "Writer::{:s}: success setting type {:d} to: {:s}\n", __func__, t, data.getValue ());
		else
			INFO_MSG("Writer::{:s}: failed setting type {:d} to: {:s}\n", __func__, t, data.getValue ());
	}
}

std::tuple<std::string_view, std::string_view> Writer::getNamespace (const std::string& full_name) {
	DEBUG_MSG ("Writer::{:s} ({:s})\n", __func__, full_name);
	auto pos = full_name.find_first_of (':');
	return {
		std::string_view (full_name).substr (0, (pos == std::string::npos ? 0 : pos)),
		std::string_view (full_name).substr (pos + 1)
	};
}

const std::string_view Writer::getNamespaceUri (const std::string_view ns) {
	DEBUG_MSG ("Writer::{:s} ({:s})\n", __func__, ns);
	for (const auto& [key, value] : this->_namespace_map) {
		if (key.compare (ns) == 0) return std::string_view (value);
	}
	return std::string_view ();
}

ProviderRouteWriterBase::RouteData Writer::getGuessedType (const std::string_view name) {
	DEBUG_MSG ("Writer::{:s} ({:s})\n", __func__, name);
	ProviderRouteWriterBase::RouteData t = ProviderRouteWriterBase::TYPE_NO_TYPE;
	if ((t = ProviderRouteWriterBase::getType (name)) != ProviderRouteWriterBase::TYPE_NO_TYPE) return t;
	return t;
}

template<class T>
void Writer::addExtensionData (T& route, ::gpx::Extensions& extensions) {
	DEBUG_MSG ("Writer::{:s} ({:p}, {:p})\n", __func__, fmt::ptr (&route), fmt::ptr (&extensions));

	for (const ::gpx::Node* extension : extensions.getElements ()) {
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
		Writer::addData (route, t, *extension);

		NOTICE_MSG ("track point extension namespace: {:s}; name: {:s}; value: {:s}.\n", ns, name, value);
	}
}

bool Writer::parseData (::gpx::GPX *root) {
	DEBUG_MSG("Writer::{:s} ({:p})\n", __func__, fmt::ptr (root));
	if (root->trks().list().size() < 1) {
		NOTICE_MSG ("No tracks have been found in file {:s}.\n", this->getPath ());
		return false;
	}

	assert (this->_provider);

	for (const ::gpx::Node* e : root->getAttributes ()) {
		if (e->getName ().compare (0, 6, "xmlns:") == 0) {
			NOTICE_MSG ("namespace add: {:s}; value: {:s}.\n", e->getName (). substr (6), e->getValue ());
			this->_namespace_map.insert (std::pair (e->getName (). substr (6), e->getValue ()));
		}
	}

	for (::gpx::TRK *track : root->trks().list()) {
		auto route = this->_provider->addRoute ();
		Writer::addData (route, ProviderRouteWriterBase::TYPE_NAME, track->name ());
		Writer::addData (route, ProviderRouteWriterBase::TYPE_COMMENT, track->cmt ());
		Writer::addData (route, ProviderRouteWriterBase::TYPE_DESCRIPTION, track->desc ());
		Writer::addData (route, ProviderRouteWriterBase::TYPE_SRC, track->src ());
		for (::gpx::Link *link : track->links ().list ())
			Writer::addData (route, ProviderRouteWriterBase::TYPE_LINK, link->href ());
		Writer::addData (route, ProviderRouteWriterBase::TYPE_ID, track->number ());
		Writer::addData (route, ProviderRouteWriterBase::TYPE_TYPE, track->type ());
		this->addExtensionData (route, track->extensions ());

		for (::gpx::TRKSeg *track_segment : track->trksegs ().list ()) {
			auto segment = route.addSegment ();
			this->addExtensionData (segment, track_segment->extensions ());

			for (::gpx::WPT *segment_point : track_segment->trkpts ().list ()) {
				auto point = segment.addPoint ();
				Writer::addData (point, ProviderRouteWriterBase::TYPE_LAT, segment_point->lat ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_LON, segment_point->lon ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_ELE, segment_point->ele ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_TIME, segment_point->time ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_MAGVAR, segment_point->magvar ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_GEOIDHEIGHT, segment_point->geoidheight ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_NAME, segment_point->name ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_COMMENT, segment_point->cmt ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_DESCRIPTION, segment_point->desc ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_SRC, segment_point->src ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_TYPE, segment_point->type ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_FIX, segment_point->fix ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_SAT, segment_point->sat ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_HDOP, segment_point->hdop ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_VDOP, segment_point->vdop ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_PDOP, segment_point->pdop ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_AGEOFDGPSDATA, segment_point->ageofdgpsdata ());
				Writer::addData (point, ProviderRouteWriterBase::TYPE_DGPSID, segment_point->dgpsid ());
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
void Writer::readExtensions (const std::list<gpx::Node *>& extensions, const unsigned short& route, const unsigned short& segment, const ObjectTime& time, unsigned short ns_len) const noexcept {
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
