#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_

#include <string>
#include <type_traits>

#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/provider/ProviderRouteBase.hpp>

namespace libgpsfile2::provider {
	namespace internal {
		template<class T>
		class Base {
		public:
			bool addData (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return (static_cast<T*>(this))->addDataImpl (t, data);
			}

			bool addSummary (const internal::ProviderRouteBase::RouteData& t , const std::string& data) {
				return (static_cast<T*>(this))->addSummaryImpl (t, data);
			}

			bool finish (void) {
				return (static_cast<T*>(this))->finishImpl ();
			}
		};
	}

	class ProviderRouteWriterBase : public ProviderWriterBase, public internal::ProviderRouteBase {
	public:
		virtual ~ProviderRouteWriterBase (void) = default;

		virtual std::size_t newRoute (void) = 0;
		virtual std::size_t newSegment (const std::size_t& route) = 0;
		virtual std::size_t newPoint (const std::size_t& route, const std::size_t& segment) = 0;

		virtual bool finishRoute (const std::size_t& route) = 0;
		virtual bool finishSegment (const std::size_t& route, const std::size_t& segment) = 0;
		virtual bool finishPoint (const std::size_t& route, const std::size_t& segment, const std::size_t& point) = 0;

		virtual bool addData (const RouteData& t, const std::string& data) = 0;
		virtual bool addData (const std::size_t& route, const RouteData& t, const std::string& data) = 0;
		virtual bool addData (const std::size_t& route, const std::size_t& segment, const RouteData& t, const std::string& data) = 0;
		virtual bool addData (const std::size_t& route, const std::size_t& segment, const std::size_t& point, const RouteData& t, const std::string& data) = 0;

		virtual bool addSummary (const RouteData& t, const std::string& data) = 0;
		virtual bool addSummary (const std::size_t& route, const RouteData& t, const std::string& data) = 0;
		virtual bool addSummary (const std::size_t& route, const std::size_t& segment, const RouteData& t, const std::string& data) = 0;
		virtual bool addSummary (const std::size_t& route, const std::size_t& segment, const std::size_t& point, const RouteData& t, const std::string& data) = 0;

	private:
		class PointBase : public internal::Base<PointBase> {
			ProviderRouteWriterBase* _base;
			const int _route;
			const int _segment;
			const int _point;

		public:
			PointBase (ProviderRouteWriterBase* base, const std::size_t& route, const std::size_t& segment, const std::size_t& point) : _base(base), _route(route), _segment(segment), _point(point) { }

			bool addDataImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addData (this->_route, this->_segment, this->_point, t, data);
			}

			bool addSummaryImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addSummary (this->_route, this->_segment, this->_point, t, data);
			}

			bool finishImpl (void) {
				return this->_base->finishPoint (this->_route, this->_segment, this->_point);
			}
		};

		class SegmentBase : public internal::Base<SegmentBase> {
			ProviderRouteWriterBase* _base;
			int _route;
			int _segment;

		public:
			SegmentBase (ProviderRouteWriterBase* base, const std::size_t& route, const std::size_t& segment) : _base(base), _route(route), _segment(segment) { }

			bool addDataImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addData (this->_route, this->_segment, t, data);
			}

			bool addSummaryImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addSummary (this->_route, this->_segment, t, data);
			}

			PointBase addPoint (void) {
				return PointBase (this->_base, this->_route, this->_segment, this->_base->newPoint (this->_route, this->_segment));
			}

			bool finishImpl (void) {
				return this->_base->finishSegment (this->_route, this->_segment);
			}
		};

		class RouteBase : public internal::Base<RouteBase> {
			ProviderRouteWriterBase* _base;
			int _route;

		public:
			RouteBase (ProviderRouteWriterBase* base, const std::size_t& route) : _base(base), _route(route) { }

			bool addDataImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addData (this->_route, t, data);
			}

			bool addSummaryImpl (const internal::ProviderRouteBase::RouteData& t, const std::string& data) {
				return this->_base->addSummary (this->_route, t, data);
			}

			SegmentBase addSegment (void) {
				return SegmentBase (this->_base, this->_route, this->_base->newSegment (this->_route));
			}

			bool finishImpl (void) {
				return this->_base->finishRoute (this->_route);
			}
		};

	public:
		RouteBase addRoute (void) {
			return RouteBase (this, this->newRoute ());
		}

		bool setData (const std::string&) const override final {
			throw std::runtime_error ("MethodNotAvalitibleInSpecializedProvider");
			return false;
		}
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERROUTEWRITERBASE_ */
