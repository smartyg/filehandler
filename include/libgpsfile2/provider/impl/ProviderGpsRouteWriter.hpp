#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEWRITER_
#define _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEWRITER_

#include <memory>
#include <vector>
#include <string>
#include <gpsdata.hpp>
#include <gpsdata/utils/PointDate.hpp>

#include <libgpsfile2/provider/impl/ProviderGpsRouteBase.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>

#define GPX_TIME_FORMATS { "%Y-%m-%dT%TZ" }

namespace libgpsfile2::provider {
	template<gpsdata::GpsRouteTrait R>
	class ProviderGpsRouteWriter final : public ProviderRouteWriterBase, protected internal::ProviderGpsRouteBase {

		using Factory = typename R::GpsFactory;
		using DataType = typename R::DataType;
		using ActivityType = typename R::ActivityType;

		const std::shared_ptr<Factory> _factory;
		bool _finished;
		std::vector<std::shared_ptr<R>> _routes;
		std::vector<std::shared_ptr<R>> _routeMap;
		std::vector<std::shared_ptr<typename R::Segment>> _segmentMap;
		std::vector<std::shared_ptr<typename R::Point>> _pointMap;
		std::map<RouteData, DataType> _convertionMap;

		ProviderGpsRouteWriter (const std::shared_ptr<Factory> factory) : _factory(factory), _finished(false) {
			this->_routes.reserve (4);
			this->_routeMap.reserve (4);
			this->_segmentMap.reserve (24);
			this->_pointMap.reserve (86400);

			this->initCovertionTable ();
		}

	public:
		~ProviderGpsRouteWriter (void) {
			this->_routeMap.clear ();
			this->_segmentMap.clear ();
			this->_pointMap.clear ();
			this->_convertionMap.clear ();
		}

	private:
		inline void initCovertionTable (void) {
			std::map<DataType, uint8_t> map;
			for (auto it = ProviderGpsRouteWriter::_type_pairs.cbegin (); it != ProviderGpsRouteWriter::_type_pairs.cend (); ++it) {
				this->_convertionMap[ProviderRouteBase::getType ((*it).first)] = this->_factory->getDataType ((*it).second);
			}
		}

		int newRoute (void) override {
			auto r = R::template create<R> (this->_factory);
			this->_routeMap.push_back (r);
			return this->_routeMap.size () - 1;
		}

		int newSegment (const int& route) override {
			(void)route;
			auto s = R::Segment::template create<typename R::Segment> (this->_factory);
			this->_segmentMap.push_back (s);
			return this->_routeMap.size () - 1;
		}

		int newPoint (const int& route, const int& segment) override {
			(void)route;
			(void)segment;
			auto p = R::Point::template create<typename R::Point> (this->_factory);
			this->_pointMap.push_back (p);
			return this->_pointMap.size () - 1;
		}

		bool finishRoute (const int& route) override {
			auto& r = this->_routeMap.at (route);
			this->_routes.push_back (r);
			return true;
		}

		bool finishSegment (const int& route, const int& segment) override {
			auto& s = this->_segmentMap.at (segment);
			const auto& r = this->_routeMap.at (route);
			return r->addSegment (s);
		}

		bool finishPoint (const int& route, const int& segment, const int& point) override {
			(void)route;
			auto& p = this->_pointMap.at (point);
			const auto& s = this->_segmentMap.at (segment);
			return s->addPoint (p);
		}

	public:
		constexpr
		bool addData (const RouteData&, const std::string&) override { return false; }

		bool addData (const int& route, const RouteData& t, const std::string& data) override {
			const auto& r = this->_routeMap.at (route);
			switch (t) {
				case ProviderRouteBase::TYPE_ID: {
					//gpsdata::ObjectId::Type id;
					uint32_t id;
					gpsdata::utils::Convert::convertValue (id, data, true);
					r->setId (id);
					break;
				}
				case ProviderRouteBase::TYPE_TYPE:
					r->setActivityType (data);
					return true;
				case ProviderRouteBase::TYPE_NAME:
					r->setTitle (data);
					return true;
				case ProviderRouteBase::TYPE_COMMENT:
					r->setSummary (data);
					return true;
				case ProviderRouteBase::TYPE_DESCRIPTION:
					r->setDetails (data);
					return true;
				default:
					return false;
			}
			return false;
		}

		bool addData (const int& route, const int& segment, const RouteData& t, const std::string& data) override {
			(void) route;
			const auto& s = this->_segmentMap.at (segment);
			if (t == ProviderRouteBase::TYPE_ID) {
				int id;
				gpsdata::utils::Convert::convertValue (id, data, true);
				s->setSegmentNumber (id);
			}
			return false;
		}

		bool addData (const int& route, const int& segment, const int& point, const RouteData& t, const std::string& data) override {
			(void) route;
			(void) segment;
			const auto& p = this->_pointMap.at (point);
			if (t == ProviderRouteBase::TYPE_TIME) {
				gpsdata::ObjectTime time = gpsdata::utils::PointDate::parseTime (data, GPX_TIME_FORMATS);
				if (time) {
					return p->setTime (time);
				}
				return false;
			} else {
				DataType t_gps;
				try {
					t_gps = this->_convertionMap.at (t);
				} catch (const std::out_of_range& e) {
					(void)e;
					return false;
				}
				if (t == ProviderRouteBase::TYPE_LAT || t == ProviderRouteBase::TYPE_LON) {
					long double value;
					gpsdata::utils::Convert::convertValue (value, data, true);
					value *= 100000000; // multiply by 10⁸8 to store value inernally
					return p->addData (t_gps, static_cast<int64_t>(value));
				} else if (t == ProviderRouteBase::TYPE_ELE) {
					long double value;
					gpsdata::utils::Convert::convertValue (value, data, true);
					value *= 1000; // multiply by 10⁸3 to store value inernally
					return p->addData (t_gps, static_cast<int>(value));
				} else
					return p->addData (t_gps, data, true);
			}
		}

		constexpr
		bool addSummary (const RouteData& t, const std::string& data) override {
			(void)t;
			(void)data;
			return false;
		}

		bool addSummary (const int& route, const RouteData& t, const std::string& data) override {
			(void)route;
			(void)t;
			(void)data;
			return true;
		}

		bool addSummary (const int& route, const int& segment, const RouteData& t, const std::string& data) override {
			(void)route;
			(void)segment;
			(void)t;
			(void)data;
			return true;
		}

		constexpr
		bool addSummary (const int& route, const int& segment, const int& point, const RouteData& t, const std::string& data) override {
			(void)route;
			(void)segment;
			(void)point;
			(void)t;
			(void)data;
			return false;
		}

		std::vector<std::shared_ptr<R>> getRoutes (void) const {
			if (this->_finished)
				return this->_routes;
			throw std::runtime_error ("Provider is not finished yet, can not export data.");
		}

		void finished (void) override {
			this->_finished = true;
		}

		[[nodiscard]] static std::unique_ptr<ProviderGpsRouteWriter<R>> create (const std::shared_ptr<Factory> factory) {
			return std::unique_ptr<ProviderGpsRouteWriter<R>>(new ProviderGpsRouteWriter (factory));
		}
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEWRITER_ */