#ifndef _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEREADER_
#define _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEREADER_

#include <memory>
#include <vector>
#include <string>
#include <gpsdata.hpp>
#include <gpsdata/traits/GpsRoute.hpp>

#include <filehandler/provider/types/route/ProviderRouteReaderBase.hpp>
#include <filehandler/provider/types/route/impl/ProviderGpsRouteBase.hpp>
#include <filehandler/utils/Strings.hpp>

namespace filehandler::provider {
	template<gpsdata::GpsRouteTrait R>
	class ProviderGpsRouteReader final : public ProviderRouteReaderBase, protected internal::ProviderGpsRouteBase {

		using Factory = typename R::GpsFactory;
		using DataType = typename R::DataType;
		using ActivityType = typename R::ActivityType;

		std::vector<std::shared_ptr<R>> _routes;
		bool _finished;
		std::vector<std::map<DataType, uint8_t>> _convertionMap;

		struct {
			std::pair<int, typename std::vector<std::shared_ptr<R>>::const_iterator> route;
			std::pair<int, typename std::vector<gpsdata::GpsValue<DataType>>::const_iterator> route_statistics;
			std::pair<int, typename R::const_iterator> segment;
			std::pair<int, typename std::vector<gpsdata::GpsValue<DataType>>::const_iterator> segment_statistics;
			std::pair<int, typename R::Segment::const_iterator> point;
			std::pair<int, typename R::Point::const_iterator> data;
		} _iterators;

		ProviderGpsRouteReader (const std::vector<std::shared_ptr<R>>& routes) : _routes(routes), _finished(false) {

			auto& it = this->_iterators;
			it.route.first = 0;
			it.route.second = this->_routes.cbegin ();
			it.route_statistics.first = 0;
			it.route_statistics.second = (*(it.route.second))->getStatistics ().cbegin ();
			it.segment.first = 0;
			it.segment.second = (*(it.route.second))->cbegin ();
			it.segment_statistics.first = 0;
			it.segment_statistics.second = (*(it.segment.second))->getStatistics ().cbegin ();
			it.point.first = 0;
			it.point.second = (*(it.segment.second))->cbegin ();
			it.data.first = 0;
			it.data.second = (*(it.point.second))->cbegin ();

			this->initCovertionTable ();
		}

		ProviderGpsRouteReader (void) = delete;
		ProviderGpsRouteReader (const ProviderGpsRouteReader&) = delete;                // copy constructor
		ProviderGpsRouteReader (ProviderGpsRouteReader&&) noexcept = delete;            // move constructor
		ProviderGpsRouteReader& operator= (const ProviderGpsRouteReader&) = delete;     // copy assignment
		ProviderGpsRouteReader& operator= (ProviderGpsRouteReader&&) noexcept = delete; // move assignment

	public:
		virtual ~ProviderGpsRouteReader (void) {
			this->_finished = true;
			this->_routes.clear ();
			this->_convertionMap.clear ();
		}

	private:
		template<class T>
		static int advance (typename T::const_iterator& it, const int& n, const T& container) {
			int counter = 0;
			if (n > 0) {
				for (; counter < n && it != container.cend (); ++counter) {
					++it;
				}

			} else if (n < 0) {
				for (;counter > n && it != container.cbegin (); --counter) {
					--it;
				}
				return counter;
			}
			return counter;
		}

		inline void updateRouteIterator (const int& n) {
			if (this->_iterators.route.first != n) {
				auto& it = this->_iterators;
				it.route.first += advance (it.route.second, n - it.route.first, this->_routes);
				it.route_statistics.first = 0;
				it.route_statistics.second = (*(it.route.second))->getStatistics ().cbegin ();
				it.segment.first = 0;
				it.segment.second = (*(it.route.second))->cbegin ();
				it.segment_statistics.first = 0;
				it.segment_statistics.second = (*(it.segment.second))->getStatistics ().cbegin ();
				it.point.first = 0;
				it.point.second = (*(it.segment.second))->cbegin ();
				it.data.first = 0;
				it.data.second = (*(it.point.second))->cbegin ();
				return;
			}
		}

		inline void updateSegmentIterator (const int& n) {
			if (this->_iterators.segment.first != n) {
				auto& it = this->_iterators;
				it.segment.first += advance (it.segment.second, n - it.segment.first, *(*(it.route.second)));
				it.segment_statistics.first = 0;
				it.segment_statistics.second = (*(it.segment.second))->getStatistics ().cbegin ();
				it.point.first = 0;
				it.point.second = (*(it.segment.second))->cbegin ();
				it.data.first = 0;
				it.data.second = (*(it.point.second))->cbegin ();
				return;
			}
		}

		inline void updatePointIterator (const int& n) {
			if (this->_iterators.point.first != n) {
				auto& it = this->_iterators;
				it.point.first += advance (it.point.second, n - it.point.first, *(*(it.segment.second)));
				it.data.first = 0;
				it.data.second = (*(it.point.second))->cbegin ();
				return;
			}
		}

		inline void updateRouteStatisticsIterator (const int& n) {
			if (this->_iterators.route_statistics.first != n) {
				auto& it = this->_iterators;
				it.route_statistics.first += advance (it.route_statistics.second, n - it.route_statistics.first, (*(it.route.second))->getStatistics ());
			}
		}

		inline void updateSegmentStatisticsIterator (const int& n) {
			if (this->_iterators.segment_statistics.first != n) {
				auto& it = this->_iterators;
				it.segment_statistics.first += advance (it.segment_statistics.second, n - it.segment_statistics.first, (*(it.segment.second))->getStatistics ());
			}
		}

		inline void updateDataIterator (const int& n) {
			if (this->_iterators.data.first != n) {
				auto& it = this->_iterators;
				it.data.first += advance (it.data.second, n - it.data.first, *(*(it.point.second)));
			}
		}

		inline bool validRoute (void) const noexcept {
			auto& it = this->_iterators;
			return (it.route.second != this->_routes.cend ());
		}

		inline bool validSegment (void) const noexcept {
			if (!this->validRoute ()) return false;
			auto& it = this->_iterators;
			return (it.segment.second != (*(it.route.second))->cend ());
		}

		inline bool validPoint (void) const noexcept {
			if (!this->validSegment ()) return false;
			auto& it = this->_iterators;
			return (it.point.second != (*(it.segment.second))->cend ());
		}

		inline bool validRouteStatistics (void) const noexcept {
			if (!this->validRoute ()) return false;
			auto& it = this->_iterators;
			return (it.route_statistics.second != (*(it.route.second))->getStatistics ().cend ());
		}

		inline bool validSegmentStatistics (void) const noexcept {
			if (!this->validSegment ()) return false;
			auto& it = this->_iterators;
			return (it.segment_statistics.second != (*(it.segment.second))->getStatistics ().cend ());
		}

		inline bool validData (void) const noexcept {
			if (!this->validPoint ()) return false;
			auto& it = this->_iterators;
			return (it.data.second != (*(it.point.second))->cend ());
		}

		inline const Factory* getFactory (const int& route) {
			this->updateRouteIterator (route);
			if (this->validRoute ())
				return (*(this->_iterators.route.second))->getFactory ().get ();
			return nullptr;
		}

		inline void initCovertionTable (void) {
			for (const auto& route : this->_routes) {
				const Factory* factory = route->getFactory ().get ();
				std::map<DataType, uint8_t> map;
				for (auto it = ProviderGpsRouteReader::_type_pairs.cbegin (); it != ProviderGpsRouteReader::_type_pairs.cend (); ++it) {
					map[factory->getDataType ((*it).second)] = ProviderRouteBase::getType ((*it).first);
				}
				this->_convertionMap.push_back (map);
			}
		}

		int convertDataType (const int& route, const DataType& type) const noexcept {
			try {
				return this->_convertionMap.at (route).at (type);
			} catch (std::out_of_range&) {
				return -1;
			}
		}

	public:
		using ProviderRouteReaderBase::getData;

		bool hasRoute (const int& route) noexcept override {
			return (route < static_cast<int>(this->_routes.size ()));
		}

		bool hasSegment (const int& route, const int& segment) noexcept override {
			try {
				return this->_routes.at (route)->hasSegment (segment);
			} catch (const std::out_of_range&) {
				return false;
			}
		}

		bool hasPoint (const int& route, const int& segment, const int& point) noexcept override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			if (!this->validSegment ()) return false;
			// We can not use updatePointIterator as we do not know if the point really exists.
			// Create a copy of the cached iterator
			auto it = this->_iterators.point.second;
			// advance the iterator as much as requested, or till we reach `end()`.
			advance (it, point - this->_iterators.point.first, *(*(this->_iterators.segment.second)));
			// Now test the copied iterator if it is still in the valid range.
			return (it != (*(this->_iterators.segment.second))->cend ());
		}

		constexpr
		bool hasData (const int&) noexcept override { return false; }
		bool hasData (const int&, const int& n) noexcept override { return (n < 7); }
		bool hasData (const int&, const int&, const int& n) noexcept override { return (n < 2); }

		bool hasData (const int& route, const int& segment, const int& point, const int& n) noexcept override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updatePointIterator (point);
			if (n < 1) {
				// entry 0 is a property of Point (getTime ())
				return this->validPoint ();
			} else {
				this->updateDataIterator (n - 1);
				return this->validData ();
			}
		}

		const std::string getData (const int&) noexcept override { return std::string (); }
		const std::string getData (const int& route, const int& n) noexcept override {
			this->updateRouteIterator (route);
			if (this->validRoute ()) {
				const auto& r = *(this->_iterators.route.second);
				std::string res;
				switch (n) {
					case 0: { // id
						const gpsdata::ObjectId& id = r->getId ();
						gpsdata::utils::Convert::convertValue (res, id.getId (), true);
						break; }
					case 1: { // time
						const gpsdata::ObjectTime& time = r->getTime ();
						gpsdata::utils::Convert::convertValue (res, time, true);
						break; }
					case 2: { // activity type
						res = r->getFactory ()->getActivityTypeString (r->getActivityType ());
						break; }
					case 3: { // title
						res = r->getTitle ();
						break; }
					case 4: { // summary
						res = r->getSummary ();
						break; }
					case 5: { // details
						res = r->getDetails ();
						break; }
					default:
						break;
				}
				return res;
			}
			return std::string ();
		}

		const std::string getData (const int& route, const int& segment, const int& n) noexcept override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			if (this->validSegment () && n == 0) {
				const auto& s = *(this->_iterators.segment.second);
				int n_s = s->getSegmentNumber ();
				std::string res;
				gpsdata::utils::Convert::convertValue (res, n_s, true);
				return res;
			}
			return std::string ();
		}

		const std::string getData (const int& route, const int& segment, const int& point, const int& n) override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updatePointIterator (point);
			if (n == 0) {
				// Get time of point an convert to a string representation of that time, including milliseconds.
				const auto& p = *(this->_iterators.point.second);
				const gpsdata::ObjectTime time = p->getTime ();
				std::tm tm{};
				tm.tm_isdst = -1;
				tm.tm_year = time.getYear () - 1900;
				tm.tm_mon = time.getMonth () - 1;
				tm.tm_mday = time.getDay ();
				tm.tm_hour = time.getHours ();
				tm.tm_min = time.getMinutes ();
				tm.tm_sec = time.getSeconds ();
				std::string msec = std::to_string (time.getMilliseconds ());
				std::string res;
				res.resize (25);
				std::size_t len = strftime (res.data (), res.size (), "%Y-%m-%dT%H:%M:%S", &tm);
				filehandler::utils::Strings::padLeft (msec, '0', 3);
				res[len++] = '.';
				res[len++] = msec[0];
				res[len++] = msec[1];
				res[len++] = msec[2];
				res[len++] = 'Z';
				res.resize (len);
				return res;
				//return std::string ("0000-00-00T00:00:00.000Z");
			} else {
				this->updateDataIterator (n - 1);
				if (this->validData ()) {
					std::string str;
					const auto& value = *(this->_iterators.data.second);
					 if (this->getFactory (route)->getValue (value, str, true)) {
						if (value.type == this->getFactory (route)->getDataType ("LAT") ||
							value.type == this->getFactory (route)->getDataType ("LON")) {
							filehandler::utils::Strings::padLeft (str, '0', 9);
							filehandler::utils::Strings::insertChar (str, '.', -8);
						} else if (value.type == this->getFactory (route)->getDataType ("ALT")) {
							filehandler::utils::Strings::padLeft (str, '0', 4);
							filehandler::utils::Strings::insertChar (str, '.', -3);
						}
						return str;
					}
				}
			}
			return std::string ();
		}

		constexpr
		RouteData getDataType (const int&) noexcept override { return -1; }
		RouteData getDataType (const int& route, const int& n) noexcept override {
			this->updateRouteIterator (route);
			if (this->validRoute ()) {
				//const auto& r = *(this->_iterators.route.second);
				std::string res;
				switch (n) {
					case 0: // id
						return ProviderRouteBase::TYPE_ID;
					case 1: // time
						return ProviderRouteBase::TYPE_TIME;
					case 2: // activity type
						return ProviderRouteBase::TYPE_TYPE;
					case 3: // title
						return ProviderRouteBase::TYPE_NAME;
					case 4: // summary
						return ProviderRouteBase::TYPE_COMMENT;
					case 5: // details
						return ProviderRouteBase::TYPE_DESCRIPTION;
					default:
						return ProviderRouteBase::TYPE_NO_TYPE;
				}
			}
			return ProviderRouteBase::TYPE_NO_TYPE;
		}
		constexpr
		RouteData getDataType (const int&, const int&, const int&) noexcept override { return ProviderRouteBase::TYPE_ID; }
		RouteData getDataType (const int& route, const int& segment, const int& point, const int& n) override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updatePointIterator (point);
			if (n == 0) {
				return ProviderRouteBase::TYPE_TIME;
			} else {
				this->updateDataIterator (n - 1);
				if (this->validData ()) {
					return this->convertDataType (route, (*(this->_iterators.data.second)).type);
				}
			}
			return -1;
		}

		constexpr
		bool hasSummary (const int&) noexcept override { return false; }
		bool hasSummary (const int& route, const int& n) noexcept override {
			this->updateRouteIterator (route);
			this->updateRouteStatisticsIterator (n);
			return this->validRouteStatistics ();
		}

		bool hasSummary (const int& route, const int& segment, const int& n) noexcept override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updateSegmentStatisticsIterator (n);
			return this->validSegmentStatistics ();
		}

		constexpr
		bool hasSummary (const int&, const int&, const int&, const int&) noexcept override { return false; }

		const std::string getSummary (const int&) noexcept override { return std::string (); }
		const std::string getSummary (const int& route, const int& n) override {
			this->updateRouteIterator (route);
			this->updateRouteStatisticsIterator (n);
			if (this->validRouteStatistics ()) {
				std::string str;
				if (this->getFactory (route)->getValue (*(this->_iterators.route_statistics.second), str, true))
					return str;
			}
			return std::string ();
		}

		const std::string getSummary (const int& route, const int& segment, const int& n) override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updateSegmentStatisticsIterator (n);
			if (this->validSegmentStatistics ()) {
				std::string str;
				if (this->getFactory (route)->getValue (*(this->_iterators.segment_statistics.second), str, true))
					return str;
			}
			return std::string ();
		}

		const std::string getSummary (const int&, const int&, const int&, const int&) noexcept override { return std::string (); }

		constexpr
		RouteData getSummaryType (const int&) noexcept override { return -1; }
		RouteData getSummaryType (const int& route, const int& n) override {
			this->updateRouteIterator (route);
			this->updateRouteStatisticsIterator (n);
			if (this->validRouteStatistics ()) {
				return this->convertDataType (route, (*(this->_iterators.route_statistics.second)).type);
			}
			return -1;
		}

		RouteData getSummaryType (const int& route, const int& segment, const int& n) override {
			this->updateRouteIterator (route);
			this->updateSegmentIterator (segment);
			this->updateSegmentStatisticsIterator (n);
			if (this->validSegmentStatistics ()) {
				return this->getDataType ((*(this->_iterators.segment_statistics.second)).type);
			}
			return -1;
		}

		constexpr
		RouteData getSummaryType (const int&, const int&, const int&, const int&) noexcept override { return -1; }

		[[nodiscard]] static std::unique_ptr<ProviderGpsRouteReader<R>> create (const std::vector<std::shared_ptr<R>>& routes) {
			return std::unique_ptr<ProviderGpsRouteReader<R>>(new ProviderGpsRouteReader (routes));
		}
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERGPSROUTEREADER_ */
