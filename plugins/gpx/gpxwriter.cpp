#include "config.h"

#include "gpxwriter.hpp"

#include <string>
#include <list>
#include <gpx/Parser.h>
#include <gpsdata/utils/Logger.hpp>
#include <gpsdata/utils/PointDate.hpp>

#include "libgpsfile2.hpp"
#include "libgpsfile2/utils/Iobuf.hpp"


#include "gpxplugin.hpp"

#include "gpxreport.hpp"

#define GPX_TIME_FORMATS { "%Y-%m-%dT%TZ" }

using libgpsfile2::provider::ProviderWriterBase;
using libgpsfile2::provider::ProviderRouteWriterBase;
using libgpsfile2::utils::Iobuf;
using gpsdata::ObjectTime;
using gpsdata::utils::PointDate;

GpxWriter::GpxWriter (const std::shared_ptr<const GpxPlugin> base, std::unique_ptr<ProviderWriterBase> dp, const std::string& path) : HandlerBase (path), HandlerWriterBase (std::move (dp), path) {
	DEBUG_MSG("GpxWriter::%s ()\n", __func__);
	this->_base_instance = base;
	//this->_dp = dp;
	this->_reporter = new GpxReport ();
	this->_parser = new gpx::Parser (this->_reporter);
}

GpxWriter::~GpxWriter (void) {
	DEBUG_MSG("GpxWriter::%s ()\n", __func__);
	this->_base_instance = nullptr;
	//this->_dp = nullptr;
	if (this->_reporter != nullptr) delete this->_reporter;
	if (this->_parser != nullptr) delete this->_parser;
}

bool GpxWriter::write (std::istream *s, const bool& is_final) {
	DEBUG_MSG("GpxWriter::%s (%p, %d)\n", __func__, s, is_final);
	Iobuf *sbuf = reinterpret_cast<Iobuf *>(s->rdbuf ());
	const char *b = sbuf->getBuffer ();
	size_t l = sbuf->in_avail ();

	bool res = this->_parser->parse (b, l, is_final);
	if (!res) ERROR_MSG("Parsing of %s failed due to %s on line %s and column %s\n", this->getPath(), this->_parser->errorText(), this->_parser->errorLineNumber(), this->_parser->errorColumnNumber());

	sbuf->consumeGet (l);

	if (is_final && res) {
		res = this->parseData (this->_parser->root ());
		delete this->_parser;
		this->_parser = nullptr;
		this->_dp->finished ();
	}
	return res;
}

bool GpxWriter::parseData (gpx::GPX *root) {
	DEBUG_MSG("GpxWriter::%s ()\n", __func__);
	if (root->trks().list().size() < 1) {
		NOTICE_MSG("No tracks have been found in file %s.\n", this->getPath ());
		return false;
	}

	assert (this->_dp);
	this->_dp->setNumTracks (root->trks ().list ().size ());

	unsigned short route = 0;
	for (gpx::TRK *track : root->trks().list()) {
		this->_dp->setTrackTitle (route, track->name ().getValue ().c_str ());
		this->_dp->setTrackSummary (route, track->cmt ().getValue ().c_str ());
		this->_dp->setTrackDetails (route, track->desc ().getValue ().c_str ());
		//this->_dp->setTrackActivityType (route, track->type());
		NOTICE_MSG("track title: %s.\n", track->name ().getValue ().c_str ());
		NOTICE_MSG("track summary: %s.\n", track->cmt ().getValue ().c_str ());
		NOTICE_MSG("track details: %s.\n", track->desc ().getValue ().c_str ());
		NOTICE_MSG("track type: %s.\n", track->type ().getValue ().c_str ());
		unsigned short segment_nr = 0;
		for (gpx::TRKSeg *segment : track->trksegs ().list ()) {
			for (gpx::WPT *point : segment->trkpts ().list ()) {

				//this->_dp->addTrackPoint (route, GpxWriter::convertToPoint (point));
				ObjectTime time = PointDate::parseTime (point->time ().getValue ().c_str (), GPX_TIME_FORMATS);
				this->_dp->addTrackPoint (route, segment_nr, time);
				//this->_dp->addTrackPointDataLong (route, segment_nr, time);
				//NOTICE("track point time: %s (%ld).\n", point->time ().getValue ().c_str (), time);
				//NOTICE("track point lat: %s.\n", point->lat ().getValue ().c_str ());
				//NOTICE("track point lon: %s.\n", point->lon ().getValue ().c_str ());
				//NOTICE("track point ele: %s.\n", point->ele ().getValue ().c_str ());
				this->_dp->addTrackPointData (route, segment_nr, time, "LAT", point->lat ().getValue ());
				this->_dp->addTrackPointData (route, segment_nr, time, "LON", point->lon ().getValue ());
				this->_dp->addTrackPointData (route, segment_nr, time, "ALT", point->ele ().getValue ());
				this->readExtensions (point->extensions ().getElements(), route, segment_nr, time);
			}
			segment_nr++;
		}
		route++;
	}
	return true;
}

void GpxWriter::readExtensions (const std::list<gpx::Node *>& extensions, const unsigned short& route, const unsigned short& segment, const ObjectTime& time, unsigned short ns_len) const noexcept {
	for (std::list<gpx::Node *>::const_iterator iter = extensions.begin(); iter != extensions.end(); ++iter) {
		std::string name = (*iter)->getName ();
		std::string value = (*iter)->getValue ();
		if (ns_len == 0) {
			if (name.compare (0, 7, "gpxtpx:", 7) == 0) ns_len = 7;
			else if (name.compare (0, 8, "gpxdata:", 8) == 0) ns_len = 8;
		}
		if (name.compare (ns_len, std::string::npos, "temp") == 0) this->_dp->addTrackPointData (route, segment, time, "TEMPERATURE", value);
		else if (name.compare (ns_len, std::string::npos, "atemp") == 0) this->_dp->addTrackPointData (route, segment, time, "TEMPERATURE", value);
		//else if (name.compare (ns_len, std::string::npos, "wtemp") == 0) this->_dp->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		//else if (name.compare (ns_len, std::string::npos, "depth") == 0) this->_dp->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		else if (name.compare (ns_len, std::string::npos, "hr") == 0) this->_dp->addTrackPointData (route, segment, time, "HEARTRATE", value);
		else if (name.compare (ns_len, std::string::npos, "cad") == 0) this->_dp->addTrackPointData (route, segment, time, "CADANCE", value);
		else if (name.compare (ns_len, std::string::npos, "speed") == 0) this->_dp->addTrackPointData (route, segment, time, "SPEED", value);
		//else if (name.compare (ns_len, std::string::npos, "course") == 0) this->_dp->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		//else if (name.compare (ns_len, std::string::npos, "bearing") == 0) this->_dp->addTrackPointDataString (route, segment, time, gpsType::NONE, value);
		else if (name.compare (ns_len, std::string::npos, "SeaLevelPressure") == 0) this->_dp->addTrackPointData (route, segment, time, "AIR_PRESSURE", value);
		else if (name.compare (ns_len, std::string::npos, "TrackPointExtension") == 0) this->readExtensions ((*iter)->getElements (), route, segment, time, ns_len);
	}
}
