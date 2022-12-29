#ifndef _PLUGIN_OPENAMBIT_PARSER_
#define _PLUGIN_OPENAMBIT_PARSER_

#include <memory>
#include <array>
#include <string_view>
#include <string>
#include <utility>
#include <libxml/xmlreader.h>
#include <Logger.hpp>

#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"
#include "Writer.hpp"

namespace plugin::openambit {

class Parser final {
	typedef std::basic_string<xmlChar> xmlString;
	typedef std::basic_string_view<xmlChar> xmlStringView;

	enum SampleType : uint16_t {
		SAMPLE_INVALID = 0,
		SAMPLE_PERIODIC = 0x0200,
		SAMPLE_LOGPAUSE = 0x0304,
		SAMPLE_LOGRESTART = 0x0305,
		SAMPLE_IBI = 0x0306,
		SAMPLE_TTFF = 0x0307,
		SAMPLE_DISTANCE_SOURCE = 0x0308,
		SAMPLE_LAPINFO = 0x0309,
		SAMPLE_ALTITUDE_SOURCE = 0x030d,
		SAMPLE_GPS_BASE = 0x030f,
		SAMPLE_GPS_SMALL = 0x0310,
		SAMPLE_GPS_TINY = 0x0311,
		SAMPLE_TIME = 0x0312,
		SAMPLE_SWIMMING_TURN = 0x0314,
		SAMPLE_SWIMMING_STROKE = 0x0315,
		SAMPLE_ACTIVITY = 0x0318,
		SAMPLE_CADENCE_SOURCE = 0x031a,
		SAMPLE_POSITION = 0x031b,
		SAMPLE_FWINFO = 0x031c,
		SAMPLE_UNKNOWN = 0xf000,
	};

	enum LapinfoType : uint8_t {
		LAPINFO_AUTOLAP = 0x00,
		LAPINFO_MANUAL = 0x01,
		LAPINFO_HIGH_INTERVAL = 0x14,
		LAPINFO_LOW_INTERVAL = 0x15,
		LAPINFO_INTERVAL = 0x16,
		LAPINFO_PAUSE = 0x1e,
		LAPINFO_START = 0x1f,
	};

	constexpr static const auto _sample_map = std::to_array<const std::pair<const filehandler::provider::ProviderRouteWriterBase::RouteData, const std::string_view>> ({
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_LAT, "Latitude" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_LON, "Longitude" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_DISTANCE, "Distance" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_SPEED, "Speed" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_HEARTRATE, "HR" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_TIME, "UTC" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_SPEED, "Gpsspeed" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_SPEED, "Bikepodspeed" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_EHPE, "EHPE" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_EVPE, "EVPE" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_ELE, "Altitude" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_ABSPRESSURE, "AbsPressure" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_PRESSURE, "SeaLevelPressure" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_ENERGY, "Energy" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_TEMPERATURE, "Temperature" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_ELE, "GPSAltitude" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_HEADING, "GPSHeading" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_HDOP, "GpsHDOP" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_VDOP, "GpsVDOP" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_FIX, "NumberOfSatellites" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_VERTICALSPEED, "VerticalSpeed" },
		{ filehandler::provider::ProviderRouteWriterBase::TYPE_POWER, "BikePower" },
	});

	filehandler::provider::ProviderRouteWriterBase* const _provider;
	const Writer* const _writer;
	const std::shared_ptr<Plugin> _base;

	long int _route = -1;
	long int _segment = -1;
	long int _point = -1;
	bool _pause = true;
	long int _prev_time;
	long int _start_time;

	Parser(const Parser& other)             = delete; // copy constructor
	Parser(Parser&& other) noexcept         = delete; // move constructor
	void operator=(const Parser& other)     = delete; // copy assignment
	void operator=(Parser&& other) noexcept = delete; // move assignment

public:
	Parser (filehandler::provider::ProviderRouteWriterBase* const provider, const Writer* const writer, const std::shared_ptr<Plugin> base) : _provider (provider), _writer (writer), _base (base) {
		DEBUG_MSG ("Parser::{:s} ({:p}, {:p}, {:p})\n", __func__, fmt::ptr (provider), fmt::ptr (writer), fmt::ptr (base));
		this->_prev_time = 0;
	}

	~Parser (void) {
		DEBUG_MSG ("Parser::{:s} ()\n", __func__);
	}

	bool parse (void);

private:
	void parseDeviceInfo (xmlTextReaderPtr reader);
	void parseLog (xmlTextReaderPtr reader);
	void parseHeader (xmlTextReaderPtr reader);
	void parseSample (xmlTextReaderPtr reader);
	void addTextAsSummary (xmlTextReaderPtr reader, const filehandler::provider::ProviderRouteWriterBase::RouteData&);

	/**
	 * print_element_names:
	 * @a_node: the initial xml node to consider.
	 *
	 * Prints the names of the all the xml elements
	 * that are siblings or children of a given xml node.
	 */
	static void	print_element_names (xmlNode* a_node) {
		xmlNode *cur_node = nullptr;

		for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				INFO_MSG ("node type: Element, name: {:s}; content: {:s} ({:d})\n", reinterpret_cast<const char*>(cur_node->name), reinterpret_cast<const char*>(xmlNodeGetContent (cur_node)), xmlNodeIsText (cur_node));
			}

			print_element_names (cur_node->children);
		}
	}

	template<typename CharT>
	static std::string convertString (CharT* in_string) {
		DEBUG_2_MSG (1, "Parser::{:s} ({:s})\n", __func__, reinterpret_cast<const char*>(in_string));
		std::string out_string;
		while (*in_string != '\0') {
			out_string.push_back (static_cast<char>(*in_string));
			++in_string;
		}
		return out_string;
	}

	template<typename CharT>
	static std::string convertString (const std::basic_string_view<CharT> in_string) {
		DEBUG_2_MSG (1, "Parser::{:s} ({:s})\n", __func__, reinterpret_cast<const char*>(in_string.data ()));
		std::string out_string;
		for (auto it = in_string.cbegin (); it != in_string.cend (); ++it) {
			out_string.push_back (static_cast<char>(*it));
		}
		return out_string;
	}

	template<typename CharT>
	static std::string convertString (const std::basic_string<CharT> in_string) {
		DEBUG_2_MSG (1, "Parser::{:s} ({:s})\n", __func__, reinterpret_cast<const char*>(in_string.c_str ()));
		std::string out_string;
		for (auto it = in_string.cbegin (); it != in_string.cend (); ++it) {
			out_string.push_back (static_cast<char>(*it));
		}
		return out_string;
	}

	constexpr static bool xmlStringCompare (const char* a, const unsigned char* b) {
		if (static_cast<const void*>(a) == static_cast<const void*>(b)) return true;
		char* ptr_a = const_cast<char*>(a);
		unsigned char* ptr_b = const_cast<unsigned char*>(b);
		while (*ptr_a != 0 || *ptr_b != 0) {
			if (static_cast<int16_t>(*ptr_a) != static_cast<int16_t>(*ptr_b)) return false;
			++ptr_a;
			++ptr_b;
		}
		return (*ptr_a == 0 && *ptr_b == 0);
	}

	static const std::tuple<std::string, std::string, std::string> getMinAvgMax (xmlTextReaderPtr reader, const xmlChar* element);
	static const xmlString getTextValue (xmlTextReaderPtr reader);
	static xmlNodePtr getLap (const xmlNodePtr);
	static const xmlStringView getType (const xmlNodePtr);
	static const xmlStringView getTime (const xmlNodePtr);
	static const xmlStringView getGpsTime (const xmlNodePtr);
	static const std::pair<const filehandler::provider::ProviderRouteWriterBase::RouteData, const xmlStringView> getTypeValue (const xmlNodePtr);
	//static LapinfoType getLapInfoType (const xmlNodePtr);
	static xmlNodePtr getNode (const xmlNodePtr node, const char* name);
	static const xmlStringView getNodeContent (const xmlNodePtr node);

	static int readerCallback (void* context, char* out_buffer, int len);
	static int closeCallback (void* context);
};
}

#endif /* _PLUGIN_OPENAMBIT_PARSER_ */