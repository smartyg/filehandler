#include "config.h"

#include "Parser.hpp"

#include <memory>
#include <thread>
#include <chrono>
#include <string_view>
#include <string>
#include <tuple>
#include <utility>
#include <cassert>
#include <libxml/xmlreader.h>
#include <Logger.hpp>
#include <gpsdata/utils/Convert.hpp>

#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>
#include <libgpsfile2/utils/Strings.hpp>

#include "OpenambitWriter.hpp"

using plugin::openambit::Parser;
using libgpsfile2::provider::ProviderRouteWriterBase;

bool Parser::parse (void) {
	DEBUG_MSG ("Parser::{:s} ()\n", __func__);

	xmlTextReaderPtr reader;

	if ((reader = xmlReaderForIO (readerCallback, closeCallback, const_cast<OpenambitWriter*>(this->_writer), this->_writer->getPath ().c_str (), "utf-8", XML_PARSE_PEDANTIC | XML_PARSE_NONET | XML_PARSE_HUGE)) == nullptr)
		ERROR_MSG ("can not load libxml2 reader for file {:s}\n", this->_writer->getPath ());

	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		const xmlChar* element_name = xmlTextReaderConstName (reader);
		//const xmlStringView element_name_sv (element_name);
		DEBUG_2_MSG (2, "process node: {:s} ({:d})\n", reinterpret_cast<const char*>(element_name), type);
		switch (type) {
			case XML_READER_TYPE_ELEMENT: {
				if (Parser::xmlStringCompare ("openambitlog", element_name)) {
					this->_route = this->_provider->newRoute ();
					ret = xmlTextReaderRead (reader);
				} else if (Parser::xmlStringCompare ("DeviceInfo", element_name))
					this->parseDeviceInfo (reader);
				else if (Parser::xmlStringCompare ("Log", element_name))
					this->parseLog (reader);
				else
					ret = xmlTextReaderNext (reader);
				break;
			}
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_TEXT:
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ELEMENT:
			case XML_READER_TYPE_END_ENTITY:
			case XML_READER_TYPE_XML_DECLARATION:
			default:
				ret = xmlTextReaderRead (reader);
				break;
		}
	}

	if (ret == -1) {
		ERROR_MSG ("{:s} : failed to parse ({:d})\n", this->_writer->getPath (), ret);
	}

	DEBUG_2_MSG (1, "done processing input, close reader ({:p})\n", fmt::ptr (reader));
	this->_provider->finishRoute (this->_route);
	xmlFreeTextReader (reader);

	return true;
}

void Parser::parseDeviceInfo (xmlTextReaderPtr reader) {
	DEBUG_MSG ("Parser::{:s} ({:p})\n", __func__, fmt::ptr (reader));

	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		const xmlChar* element_name = xmlTextReaderConstName (reader);
		//const xmlStringView element_name_sv (element_name);
		DEBUG_MSG ("process node: {:s} ({:d})\n", reinterpret_cast<const char*>(element_name), type);
		switch (type) {
			case XML_READER_TYPE_ELEMENT: {
				if (Parser::xmlStringCompare ("Serial", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DEVICESERIAL);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Name", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DEVICENAME);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("FWVersion", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DEVICEFWVERSION);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("HWVersion", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DEVICEHWVERSION);
					ret = xmlTextReaderRead (reader);
					break;
				}
				ret = xmlTextReaderNext (reader);
				break;
			}
			case XML_READER_TYPE_END_ELEMENT:
				if (Parser::xmlStringCompare ("DeviceInfo", element_name))
					return;
				[[fallthrough]];
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_TEXT:
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ENTITY:
			case XML_READER_TYPE_XML_DECLARATION:
			default:
				ret = xmlTextReaderRead (reader);
				break;
		}
	}

	if (ret == -1) {
		ERROR_MSG ("{:s} : failed to parse ({:d})\n", this->_writer->getPath (), ret);
	}
}

void Parser::parseLog (xmlTextReaderPtr reader) {
	DEBUG_MSG ("Parser::{:s} ({:p})\n", __func__, fmt::ptr (reader));

	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		const xmlChar* element_name = xmlTextReaderConstName (reader);
		//const xmlStringView element_name_sv (element_name);
		DEBUG_MSG ("process node: {:s} ({:d})\n", reinterpret_cast<const char*>(element_name), type);
		switch (type) {
			case XML_READER_TYPE_ELEMENT: {
				if (Parser::xmlStringCompare ("Header", element_name)) {
					this->parseHeader (reader);
				} else if (Parser::xmlStringCompare ("Samples", element_name)) {
					//this->_segment = this->_provider->newSegment (this->_route);
					ret = xmlTextReaderRead (reader);
				} else if (Parser::xmlStringCompare ("Sample", element_name)) {
					this->parseSample (reader);
					ret = xmlTextReaderNext (reader);
				} else
					ret = xmlTextReaderNext (reader);
				break;
			}
			case XML_READER_TYPE_END_ELEMENT:
				if (Parser::xmlStringCompare ("Log", element_name))
					return;
				[[fallthrough]];
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_TEXT:
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ENTITY:
			case XML_READER_TYPE_XML_DECLARATION:
			default:
				ret = xmlTextReaderRead (reader);
				break;
		}
	}

	if (ret == -1) {
		ERROR_MSG ("{:s} : failed to parse ({:d})\n", this->_writer->getPath (), ret);
	}

	//this->_provider->finishRoute (this->_route);
}

void Parser::parseHeader (xmlTextReaderPtr reader) {
	DEBUG_MSG ("Parser::{:s} ({:p})\n", __func__, fmt::ptr (reader));

	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		const xmlChar* element_name = xmlTextReaderConstName (reader);
		//const xmlStringView element_name_sv (element_name);
		DEBUG_MSG ("process node: {:s} ({:d})\n", reinterpret_cast<const char*>(element_name), type);
		switch (type) {
			case XML_READER_TYPE_ELEMENT: {
				if (Parser::xmlStringCompare ("Ascent", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_ASCENT);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Descent", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DESCENT);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("AscentTime", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_ASCENTTIME);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("DescentTime", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DESCENTTIME);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("RecoveryTime", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_RECOVERYTIME);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Speed", element_name)) {
					const xmlString name = std::basic_string (element_name);
					const auto min_avg_max = Parser::getMinAvgMax (reader, name.c_str ());
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_SPEEDAVERAGE, std::get<1>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_SPEEDMAX, std::get<2>(min_avg_max));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Cadence", element_name)) {
					const xmlString name = std::basic_string (element_name);
					const auto min_avg_max = Parser::getMinAvgMax (reader, name.c_str ());
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_CADENCEAVERAGE, std::get<1>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_CADENCEMAX, std::get<2>(min_avg_max));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Altitude", element_name)) {
					const xmlString name = std::basic_string (element_name);
					const auto min_avg_max = Parser::getMinAvgMax (reader, name.c_str ());
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_ALTITUDEMIN, std::get<0>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_ALTITUDEMAX, std::get<2>(min_avg_max));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("HR", element_name)) {
					const xmlString name = std::basic_string (element_name);
					const auto min_avg_max = Parser::getMinAvgMax (reader, name.c_str ());
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_HEARTRATEMIN, std::get<0>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_HEARTRATEAVERAGE, std::get<1>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_HEARTRATEMAX, std::get<2>(min_avg_max));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("PeakTrainingEffect", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_PEAKTRAININGEFFECT);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("ActivityTypeName", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_TYPE);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Temperature", element_name)) {
					const xmlString name = std::basic_string (element_name);
					const auto min_avg_max = Parser::getMinAvgMax (reader, name.c_str ());
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_TEMPERATUREMIN, std::get<0>(min_avg_max));
					this->_provider->addSummary (this->_route, ProviderRouteWriterBase::TYPE_TEMPERATUREMAX, std::get<2>(min_avg_max));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Distance", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_DISTANCE);
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Energy", element_name)) {
					this->addTextAsSummary (reader, ProviderRouteWriterBase::TYPE_ENERGY);
					ret = xmlTextReaderRead (reader);
					break;
				}
				ret = xmlTextReaderNext (reader);
				break;
			}
			case XML_READER_TYPE_END_ELEMENT:
				if (Parser::xmlStringCompare ("Header", element_name))
					return;
				[[fallthrough]];
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_TEXT:
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ENTITY:
			case XML_READER_TYPE_XML_DECLARATION:
			default:
				ret = xmlTextReaderRead (reader);
				break;
		}
	}

	if (ret == -1) {
		ERROR_MSG ("{:s} : failed to parse ({:d})\n", this->_writer->getPath (), ret);
	}
}

void Parser::parseSample (xmlTextReaderPtr reader) {
	DEBUG_MSG ("Parser::{:s} ({:p})\n", __func__, fmt::ptr (reader));

	const xmlNodePtr sample_node = xmlTextReaderExpand (reader)->children;
	//print_element_names (sample_node->children);

	const xmlStringView type_str = Parser::getType (sample_node);
	const xmlStringView time_str = Parser::getTime (sample_node);
	long int time = -1;
	//Parser::SampleType type = Parser::SAMPLE_INVALID;
	int type = 0;
	gpsdata::utils::Convert::convertValue (type, type_str, true);
	gpsdata::utils::Convert::convertValue (time, time_str, true);

	switch (static_cast<Parser::SampleType>(type)) {
		case SAMPLE_PERIODIC:
		case SAMPLE_GPS_BASE:
		case SAMPLE_GPS_SMALL:
		case SAMPLE_GPS_TINY: {
			DEBUG_MSG ("Gps/Periodic sample ({:d}), time: {:d}.\n", type, time);
			if (this->_prev_time != time) {
				if (this->_point >= 0)
					this->_provider->finishPoint (this->_route, this->_segment, this->_point);

				if (this->_pause) break;

				//new point
				this->_point = this->_provider->newPoint (this->_route, this->_segment);
				this->_provider->addData (this->_route, this->_segment, this->_point, ProviderRouteWriterBase::TYPE_TIME, Parser::convertString (Parser::getGpsTime (sample_node)));
			}
			this->_prev_time = time;

			xmlNodePtr cur_node;
			for (cur_node = sample_node; cur_node; cur_node = cur_node->next) {
				const auto type_value_pair = Parser::getTypeValue (cur_node);
				//if (std::get<xmlStringView>(type_value_pair).empty ()) continue;
				if (std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair) == ProviderRouteWriterBase::TYPE_NO_TYPE) continue;
				if (std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair) == ProviderRouteWriterBase::TYPE_TIME) continue;

				std::string value = Parser::convertString (std::get<const xmlStringView>(type_value_pair));

				switch (std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair)) {
					case ProviderRouteWriterBase::TYPE_LAT:
					case ProviderRouteWriterBase::TYPE_LON:
						libgpsfile2::utils::Strings::padLeft (value, '0', 9, libgpsfile2::utils::Strings::isSigned (value));
						libgpsfile2::utils::Strings::insertChar (value, '.', -8);
						break;
					case ProviderRouteWriterBase::TYPE_SPEED:
						libgpsfile2::utils::Strings::padLeft (value, '0', 3);
						libgpsfile2::utils::Strings::insertChar (value, '.', -2);
						break;
					case ProviderRouteWriterBase::TYPE_TEMPERATURE:
						libgpsfile2::utils::Strings::padLeft (value, '0', 2);
						libgpsfile2::utils::Strings::insertChar (value, '.', -1);
					default:
						break;
				}
				DEBUG_MSG ("add data with type: {:d}; and value: {:s}.\n", std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair), value);
				this->_provider->addData (this->_route, this->_segment, this->_point, std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair), value);
			}
			break;
		}
		case SAMPLE_LAPINFO: {
			DEBUG_MSG ("lap-info sample ({:d}).\n", type);
			// Manual
			// Start
			// Pauze
			// Autolap
			// High Interval
			// Low Interval
			// Interval
			const xmlNodePtr lap = Parser::getLap (sample_node);

			const xmlStringView lapinfo_type_str = Parser::getType (lap);
			int lapinfo_type = -1;
			gpsdata::utils::Convert::convertValue (lapinfo_type, lapinfo_type_str, true);
			switch (static_cast<Parser::LapinfoType>(lapinfo_type)) {
				case LAPINFO_MANUAL:
				case LAPINFO_START: {
					DEBUG_MSG ("lap-info type Start/Manual ({:d}), finish old segment (if any) and start new segment.\n", lapinfo_type);
					if (this->_segment >= 0) {
						xmlNodePtr cur_node;
						for (cur_node = lap; cur_node; cur_node = cur_node->next) {
							const auto type_value_pair = Parser::getTypeValue (lap);
							if (std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair) == ProviderRouteWriterBase::TYPE_DISTANCE)
								this->_provider->addSummary (this->_route, this->_segment, std::get<const ProviderRouteWriterBase::RouteData>(type_value_pair), Parser::convertString (std::get<const xmlStringView>(type_value_pair)));
						}
						//TODO: check for return value (false if not a valid segment)
						this->_provider->finishSegment (this->_route, this->_segment);
					}
					this->_segment = this->_provider->newSegment (this->_route);
					std::string segment_number_str;
					gpsdata::utils::Convert::convertValue (segment_number_str, this->_segment, true);
					this->_provider->addData (this->_route, this->_segment, ProviderRouteWriterBase::TYPE_ID, segment_number_str);
					this->_pause = false;
					break;
				}
				case LAPINFO_PAUSE: {
					DEBUG_MSG ("lap-info type Pause ({:d}), finish old segment (if any).\n", lapinfo_type);
					this->_pause = true;
					if (this->_segment >= 0 && (this->_prev_time - time > 1000)) {
						//TODO: check for return value (false if not a valid segment)
						this->_provider->finishSegment (this->_route, this->_segment);
						this->_segment = -1;
					}
					break;
				}
				case LAPINFO_AUTOLAP:
				case LAPINFO_HIGH_INTERVAL:
				case LAPINFO_LOW_INTERVAL:
				case LAPINFO_INTERVAL:
					INFO_MSG ("lap-info type {:d} is not supported yet.\n", lapinfo_type);
					break;
				default:
					WARNING_MSG ("lap-info type {:d} is unknown.\n", lapinfo_type);
					break;
			}

			break;
		}
				case SAMPLE_SWIMMING_TURN:
				case SAMPLE_SWIMMING_STROKE:

				case SAMPLE_CADENCE_SOURCE:
				case SAMPLE_ALTITUDE_SOURCE:
				case SAMPLE_DISTANCE_SOURCE:

				case SAMPLE_LOGRESTART:
				case SAMPLE_LOGPAUSE:
				case SAMPLE_TIME:
				case SAMPLE_IBI:
				case SAMPLE_TTFF:
				case SAMPLE_ACTIVITY:
				case SAMPLE_POSITION:
				case SAMPLE_FWINFO:
				case SAMPLE_UNKNOWN:
				case SAMPLE_INVALID:
				default:
					DEBUG_MSG ("other sample type: {:d} ({:s}).\n", type, reinterpret_cast<const char*>(type_str.data ()));
					return;
	}
}

const std::tuple<std::string, std::string, std::string> Parser::getMinAvgMax (xmlTextReaderPtr reader, const xmlChar* element) {
	DEBUG_MSG ("Parser::{:s} ({:p}, {:s})\n", __func__, fmt::ptr (reader), reinterpret_cast<const char*>(element));
	if (element == nullptr) return {};
	std::string min_str, avg_str, max_str;
	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		const xmlChar* element_name = xmlTextReaderConstName (reader);
		//const xmlStringView element_name_sv (element_name);
		DEBUG_MSG ("process node: {:s} ({:d})\n", reinterpret_cast<const char*>(element_name), type);
		switch (type) {
			case XML_READER_TYPE_ELEMENT: {
				if (Parser::xmlStringCompare ("Min", element_name)) {
					min_str = Parser::convertString (Parser::getTextValue (reader));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Avg", element_name)) {
					avg_str = Parser::convertString (Parser::getTextValue (reader));
					ret = xmlTextReaderRead (reader);
					break;
				} else if (Parser::xmlStringCompare ("Max", element_name)) {
					max_str = Parser::convertString (Parser::getTextValue (reader));
					ret = xmlTextReaderRead (reader);
					break;
				}
				ret = xmlTextReaderNext (reader);
				break;
			}
			case XML_READER_TYPE_END_ELEMENT:
				if (xmlStringView (element_name).compare (element) == 0)
					return std::make_tuple (min_str, avg_str, max_str);
				[[fallthrough]];
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_TEXT:
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ENTITY:
			case XML_READER_TYPE_XML_DECLARATION:
			default:
				ret = xmlTextReaderRead (reader);
				break;
		}
	}

	if (ret == -1) {
		ERROR_MSG ("failed to parse ({:d})\n", ret);
	}
	return {};
}

const Parser::xmlString Parser::getTextValue (xmlTextReaderPtr reader) {
	DEBUG_MSG ("Parser::{:s} ({:p})\n", __func__, fmt::ptr (reader));
	xmlString value;
	int ret = xmlTextReaderRead (reader);
	while (ret == 1) {
		int type = xmlTextReaderNodeType (reader);
		switch (type) {
			case XML_READER_TYPE_TEXT: {
				value = xmlString(xmlTextReaderConstValue (reader));
				ret = xmlTextReaderRead (reader);
				break;
			}
			case XML_READER_TYPE_END_ELEMENT:
				return value;
			case XML_READER_TYPE_NONE:
			case XML_READER_TYPE_ATTRIBUTE:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_ENTITY:
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_NOTATION:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			case XML_READER_TYPE_END_ENTITY:
				ret = xmlTextReaderRead (reader);
				break;
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_PROCESSING_INSTRUCTION:
			case XML_READER_TYPE_XML_DECLARATION:
			case XML_READER_TYPE_DOCUMENT:
			case XML_READER_TYPE_DOCUMENT_TYPE:
			case XML_READER_TYPE_DOCUMENT_FRAGMENT:
			default:
				const xmlChar* name = xmlTextReaderConstName (reader);
				ERROR_MSG ("Unexpected element encountered {:d} with name: {:s}\n", type, reinterpret_cast<const char*>(name));
				throw std::runtime_error ("Unexpected element encountered.");
		}
	}

	if (ret == -1) {
		ERROR_MSG ("failed to parse ({:d})\n", ret);
	}
	throw std::runtime_error ("No text element found.");
}

void Parser::addTextAsSummary (xmlTextReaderPtr reader, const ProviderRouteWriterBase::RouteData& t) {
	DEBUG_MSG ("Parser::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (reader), t);
	try {
		const auto value = Parser::getTextValue (reader);
		const std::string value_str = Parser::convertString (value);
		this->_provider->addSummary (this->_route, t, value_str);
	} catch (const std::runtime_error& e) {
		EXCEPTION_WARNING_MSG (e);
	} /*catch (const std::exception& e) {
	EXCEPTION_ERROR_MSG (e);
}*/
}

const Parser::xmlStringView Parser::getType (const xmlNodePtr node) {
	const xmlNodePtr type_node = Parser::getNode (node, "Type");
	if (type_node) {
		xmlAttrPtr attr = nullptr;
		for (attr = type_node->properties; attr; attr = attr->next) {
			if (Parser::xmlStringCompare ("id", attr->name)) return xmlStringView (attr->children->content);
		}
	}
	return {};
}

const Parser::xmlStringView Parser::getTime (const xmlNodePtr node) {
	const xmlNodePtr time_node = Parser::getNode (node, "Time");
	if (time_node) return Parser::getNodeContent (time_node);
	return {};
}

const Parser::xmlStringView Parser::getGpsTime (const xmlNodePtr node) {
	const xmlNodePtr utc_node = Parser::getNode (node, "UTC");
	if (utc_node) return Parser::getNodeContent (utc_node);
	return {};
}

xmlNodePtr Parser::getLap (const xmlNodePtr node) {
	const xmlNodePtr lap = Parser::getNode (node, "Lap");
	if (lap) return lap->children;
	return nullptr;
}

xmlNodePtr Parser::getNode (const xmlNodePtr node, const char* name) {
	xmlNodePtr cur_node = nullptr;
	for (cur_node = node; cur_node; cur_node = cur_node->next) {
		if (Parser::xmlStringCompare (name, cur_node->name)) {
			//DEBUG_2_MSG (2, "found match for node named '{:s}'; content pointer: {:p}; child pointer: {:p}; attribute pointer: {:p}\n", name, fmt::ptr (cur_node->content), fmt::ptr (cur_node->children), fmt::ptr (cur_node->properties));
			return cur_node;
		}
	}
	return nullptr;
}

const Parser::xmlStringView Parser::getNodeContent (const xmlNodePtr node) {
	if (node->type == XML_TEXT_NODE && node->content) return xmlStringView (node->content);
	else if (node->type == XML_ELEMENT_NODE && node->children) {
		const xmlNodePtr child = node->children;
		if (child->type == XML_TEXT_NODE && child->content) return xmlStringView (child->content);
	}
	return {};
}

const std::pair<const ProviderRouteWriterBase::RouteData, const Parser::xmlStringView> Parser::getTypeValue (const xmlNodePtr node) {
	DEBUG_2_MSG (1, "Parser::{:s} ({:p})\n", __func__, fmt::ptr (node));
	for (const auto& entry : Parser::_sample_map) {
		if (Parser::xmlStringCompare (std::get<const std::string_view>(entry).data (), node->name)) {
			const xmlStringView value = Parser::getNodeContent (node);
			//DEBUG_2_MSG (1, "found tag: {:s} ({:d}); value: {:s}\n", reinterpret_cast<const char*>(node->name), std::get<const ProviderRouteWriterBase::RouteData>(entry), reinterpret_cast<const char*>(value.data ()));
			return std::make_pair (std::get<const ProviderRouteWriterBase::RouteData>(entry), value);
		}
	}
	return {ProviderRouteWriterBase::TYPE_NO_TYPE, {}};
}

int Parser::readerCallback (void* context, char* out_buffer, int len) {
	DEBUG_MSG ("Parser::{:s} ({:p}, {:p}, {:d})\n", __func__, fmt::ptr (context), fmt::ptr (out_buffer), len);

	OpenambitWriter* ptr = static_cast<OpenambitWriter*>(context);
	libgpsfile2::utils::Iobuf *sbuf = reinterpret_cast<libgpsfile2::utils::Iobuf *>(ptr->getStream ()->rdbuf ());

	while (!ptr->isFinished () && sbuf->in_avail () == 0) {
		std::this_thread::sleep_for (std::chrono::milliseconds (100));
	}

	ptr->lock ();
	// Return upto `availible` bytes, but not more than requested.
	const std::size_t read = std::min (static_cast<std::streamsize>(len), sbuf->in_avail ());
	DEBUG_2_MSG (4, "will copy {:d} bytes into xml reader buffer.\n", read);
	const void* ret = std::memcpy (out_buffer, sbuf->getBuffer (), read);
	assert (ret == out_buffer);
	DEBUG_2_MSG (3, "copied {:d} bytes into xml reader buffer.\n", read);
	sbuf->consumeGet (read);
	ptr->unlock ();

	DEBUG_2_MSG (2, "return: {:d}\n", read);

	return read;
}

int Parser::closeCallback (void* context) {
	DEBUG_MSG ("OpenambitWriter::{:s} ({:p})\n", __func__, fmt::ptr (context));

	OpenambitWriter* ptr = static_cast<OpenambitWriter*>(context);

	assert (ptr->isFinished());

	return 0;
}