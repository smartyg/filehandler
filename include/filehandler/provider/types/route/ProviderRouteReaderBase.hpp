#ifndef _FILEHANDLER_PROVIDER_PROVIDERROUTEREADERBASE_
#define _FILEHANDLER_PROVIDER_PROVIDERROUTEREADERBASE_

#include <memory>
#include <string>
#include <type_traits>

#include <filehandler/provider/ProviderReaderBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteBase.hpp>

namespace filehandler::provider {

	namespace internal {
		inline namespace traits {
			template<class T>
			concept CONCEPT_RETURN ComparableTrait = requires(const T t1, const T t2) {
				{ t1 == t2 } -> SAME_TYPE(bool);
				{ t1 != t2 } -> SAME_TYPE(bool);
			};

			template<class T>
			concept CONCEPT_RETURN ProviderRouteReaderBaseTrait = /*requires(const T t) {
				typename T::RouteData;
			} &&*/ requires(const T t, const int n) {
				{ t.hasData (n) } -> SAME_TYPE(bool);
				{ t.getDataType (n) } -> SAME_TYPE(ProviderRouteBase::RouteData);
				{ t.getData (n) } -> SAME_TYPE(const std::string);
				{ t.hasSummary (n) } -> SAME_TYPE(bool);
				{ t.getSummary (n) } -> SAME_TYPE(const std::string);
				{ t.getSummaryType (n) } -> SAME_TYPE(ProviderRouteBase::RouteData);
			};
		}

		template<class T, ComparableTrait B>
		class IteratorBase {
		protected:
			const std::shared_ptr<B> _base;
			int _n;

		public:
			IteratorBase (const std::shared_ptr<B> base, const int& n) : _base(base), _n(n) {
				if (this->_n > -1 && !(static_cast<T*>(this)->validate (this->_n))) this->_n = -1;
			}

			IteratorBase (void) = delete;
			~IteratorBase (void) = default;
			IteratorBase (const IteratorBase& other) : IteratorBase (other._base, other._n) { } // copy constructor
			IteratorBase (IteratorBase&& other) noexcept : IteratorBase (other._base, other._n) { } // move constructor

			IteratorBase& operator= (const IteratorBase& other) // copy assignment
			{
				return *this = IteratorBase (other);
			}

			IteratorBase& operator= (IteratorBase&& other) noexcept // move assignment
			{
				return *this = IteratorBase (other);
			}

			inline T& operator++ (void) {
				++this->_n;
				T* obj = static_cast<T*>(this);
				if (!(obj->validate (this->_n))) this->_n = -1;
				return *obj;
			}

			inline T& operator-- (void) {
				if (this->_n > -1) --this->_n;
				return *(static_cast<T*>(this));
			}

			inline bool operator== (const T& other) const noexcept {
				return (*(this->_base) == *(other._base) &&
				this->_n == other._n);
			}

			#if __cplusplus < 202002L
			inline bool operator!= (const T& other) const noexcept { return !(*this == other); }
			#endif

			inline const T& operator* (void) const {
				return *(static_cast<const T*>(this));
			}

		protected:
			inline bool validate (const int&) const noexcept { return false; }
		};

		template<class T, class B>
		class ContainerBase {
			static_assert (std::is_constructible_v<T, const std::shared_ptr<B>, int>);

		protected:
			const std::shared_ptr<B> _base;

		public:
			ContainerBase (const std::shared_ptr<B> base) : _base(base) { }

			ContainerBase (void) = delete;
			~ContainerBase (void) = default;
			ContainerBase (const ContainerBase& other) : ContainerBase (other._base) { } // copy constructor
			ContainerBase (ContainerBase&& other) noexcept : ContainerBase (other._base) { } // move constructor

			ContainerBase& operator= (const ContainerBase& other) // copy assignment
			{
				return *this = ContainerBase (other);
			}

			ContainerBase& operator= (ContainerBase&& other) noexcept // move assignment
			{
				return *this = ContainerBase (other);
			}

			inline T begin (void) const noexcept {
				return T (this->_base, 0);
			}

			inline T end (void) const noexcept {
				return T (this->_base, -1);
			}
		};

	}

	// Data providers used for data handlers from plugins.
	class ProviderRouteReaderBase : public ProviderReaderBase, public internal::ProviderRouteBase {
	public:
		virtual ~ProviderRouteReaderBase (void) = default;

		//virtual GpsRoute_ptr_const getRoute (void) const = 0;

		virtual bool hasRoute (const int& n_route) noexcept = 0;
		virtual bool hasSegment (const int& n_route, const int& n_segment) noexcept = 0;
		virtual bool hasPoint (const int& n_route, const int& n_segment, const int& n_point) noexcept = 0;

		virtual bool hasData (const int& n) noexcept = 0;
		virtual bool hasData (const int& route, const int& n) noexcept = 0;
		virtual bool hasData (const int& route, const int& segment, const int& n) noexcept = 0;
		virtual bool hasData (const int& route, const int& segment, const int& point, const int& n) noexcept = 0;

		virtual const std::string getData (const int& n) = 0;
		virtual const std::string getData (const int& route, const int& n) = 0;
		virtual const std::string getData (const int& route, const int& segment, const int& n) = 0;
		virtual const std::string getData (const int& route, const int& segment, const int& point, const int& n) = 0;

		virtual RouteData getDataType (const int& n) = 0;
		virtual RouteData getDataType (const int& route, const int& n) = 0;
		virtual RouteData getDataType (const int& route, const int& segment, const int& n) = 0;
		virtual RouteData getDataType (const int& route, const int& segment, const int& point, const int& n) = 0;

		virtual bool hasSummary (const int& n) noexcept = 0;
		virtual bool hasSummary (const int& route, const int& n) noexcept = 0;
		virtual bool hasSummary (const int& route, const int& segment, const int& n) noexcept = 0;
		virtual bool hasSummary (const int& route, const int& segment, const int& point, const int& n) noexcept = 0;

		virtual const std::string getSummary (const int& n) = 0;
		virtual const std::string getSummary (const int& route, const int& n) = 0;
		virtual const std::string getSummary (const int& route, const int& segment, const int& n) = 0;
		virtual const std::string getSummary (const int& route, const int& segment, const int& point, const int& n) = 0;

		virtual RouteData getSummaryType (const int& n) = 0;
		virtual RouteData getSummaryType (const int& route, const int& n) = 0;
		virtual RouteData getSummaryType (const int& route, const int& segment, const int& n) = 0;
		virtual RouteData getSummaryType (const int& route, const int& segment, const int& point, const int& n) = 0;

	private:
		class RouteBase {
		public:
			ProviderRouteReaderBase* const _provider;

			RouteBase (ProviderRouteReaderBase* provider) noexcept : _provider(provider) { }

			inline bool hasRoute (const int& n) const noexcept {
				return this->_provider->hasRoute (n);
			}

			inline bool hasSummary (const int& n) const noexcept {
				return this->_provider->hasSummary (n);
			}

			inline const std::string getSummary (const int& n) const {
				return this->_provider->getSummary (n);
			}

			inline RouteData getSummaryType (const int& n) const {
				return this->_provider->getSummaryType (n);
			}

			inline bool hasData (const int& n) const noexcept {
				return this->_provider->hasData (n);
			}

			inline const std::string getData (const int& n) const {
				return this->_provider->getData (n);
			}

			inline RouteData getDataType (const int& n) const {
				return this->_provider->getDataType (n);
			}

			inline bool operator== (const RouteBase &other) const noexcept {
				return (this->_provider == other._provider);
			}

			#if __cplusplus < 202002L
			inline bool operator!= (const RouteBase& other) const noexcept { return !(*this == other); }
			#endif
		};

		class SegmentBase {
		public:
			ProviderRouteReaderBase* const _provider;
			const int _n_route;

			SegmentBase (ProviderRouteReaderBase* provider, const int& route) noexcept : _provider(provider), _n_route(route) { }
			SegmentBase (const std::shared_ptr<RouteBase> base, const int& route) noexcept : _provider(base->_provider), _n_route(route) {}

			inline bool hasSegment (const int& n) const noexcept {
				return this->_provider->hasSegment (this->_n_route, n);
			}

			inline bool hasSummary (const int& n) const noexcept {
				return this->_provider->hasSummary (this->_n_route, n);
			}

			inline const std::string getSummary (const int& n) const {
				return this->_provider->getSummary (this->_n_route, n);
			}

			inline RouteData getSummaryType (const int& n) const {
				return this->_provider->getSummaryType (this->_n_route, n);
			}

			inline bool hasData (const int& n) const noexcept {
				return this->_provider->hasData (this->_n_route, n);
			}

			inline const std::string getData (const int& n) const {
				return this->_provider->getData (this->_n_route, n);
			}

			inline RouteData getDataType (const int& n) const {
				return this->_provider->getDataType (this->_n_route, n);
			}

			inline bool operator== (const SegmentBase &other) const noexcept {
				return (this->_provider == other._provider &&
				this->_n_route == other._n_route);
			}

			#if __cplusplus < 202002L
			inline bool operator!= (const SegmentBase& other) const noexcept { return !(*this == other); }
			#endif
		};

		class PointBase {
		public:
			ProviderRouteReaderBase* const _provider;
			const int _n_route;
			const int _n_segment;

			PointBase (ProviderRouteReaderBase* provider, const int& route, const int& segment) noexcept : _provider(provider), _n_route(route), _n_segment(segment) { }
			PointBase (const std::shared_ptr<SegmentBase> base, const int& segment) noexcept : _provider(base->_provider), _n_route(base->_n_route), _n_segment(segment) {}

			inline bool hasPoint (const int& n) const noexcept {
				return this->_provider->hasPoint (this->_n_route, this->_n_segment, n);
			}

			inline bool hasSummary (const int& n) const noexcept {
				return this->_provider->hasSummary (this->_n_route, this->_n_segment, n);
			}

			inline const std::string getSummary (const int& n) const {
				return this->_provider->getSummary (this->_n_route, this->_n_segment, n);
			}

			inline RouteData getSummaryType (const int& n) const {
				return this->_provider->getSummaryType (this->_n_route, this->_n_segment, n);
			}

			inline bool hasData (const int& n) const noexcept {
				return this->_provider->hasData (this->_n_route, this->_n_segment, n);
			}

			inline const std::string getData (const int& n) const {
				return this->_provider->getData (this->_n_route, this->_n_segment, n);
			}

			inline RouteData getDataType (const int& n) const {
				return this->_provider->getDataType (this->_n_route, this->_n_segment, n);
			}

			inline bool operator== (const PointBase &other) const noexcept {
				return (this->_provider == other._provider &&
				this->_n_route == other._n_route &&
				this->_n_segment == other._n_segment);
			}

			#if __cplusplus < 202002L
			inline bool operator!= (const PointBase& other) const noexcept { return !(*this == other); }
			#endif
		};

		class Point {
		public:
			ProviderRouteReaderBase* const _provider;
			const int _n_route;
			const int _n_segment;
			const int _n_point;

			Point (ProviderRouteReaderBase* provider, const int& route, const int& segment, const int& point) noexcept : _provider(provider), _n_route(route), _n_segment(segment), _n_point(point) { }
			Point (const std::shared_ptr<PointBase> base, const int& point) noexcept : _provider(base->_provider), _n_route(base->_n_route), _n_segment(base->_n_segment), _n_point(point) {}

			inline bool hasData (const int& n) const noexcept {
				return this->_provider->hasData (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline bool hasSummary (const int& n) const noexcept {
				return this->_provider->hasSummary (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline const std::string getSummary (const int& n) const {
				return this->_provider->getSummary (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline RouteData getSummaryType (const int& n) const {
				return this->_provider->getSummaryType (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline const std::string getData (const int& n) const {
				return this->_provider->getData (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline RouteData getDataType (const int& n) const {
				return this->_provider->getDataType (this->_n_route, this->_n_segment, this->_n_point, n);
			}

			inline bool operator== (const Point &other) const noexcept {
				return (this->_provider == other._provider &&
				this->_n_route == other._n_route &&
				this->_n_segment == other._n_segment &&
				this->_n_point == other._n_point);
			}

			#if __cplusplus < 202002L
			inline bool operator!= (const Point& other) const { return !(*this == other); }
			#endif
		};

		template<internal::ProviderRouteReaderBaseTrait B>
		class IteratorData : public internal::IteratorBase<IteratorData<B>, B> {
		public:
			using internal::IteratorBase<IteratorData<B>, B>::IteratorBase;

			inline bool validate (const int& n) const noexcept {
				return this->_base->hasData (n);
			}

			inline const std::pair<RouteData, const std::string> operator* (void) const {
				return std::make_pair<RouteData, const std::string>(this->getDataType (), this->getData ());
			}

			inline const std::string getData (void) const {
				return this->_base->getData (this->_n);
			}

			inline RouteData getDataType (void) const {
				return this->_base->getDataType (this->_n);
			}

			inline bool hasData (void) const noexcept {
				return this->_base->hasData (this->_n);
			}
		};

		template<class B>
		using DataContainer = internal::ContainerBase<IteratorData<B>, B>;

		template<internal::ProviderRouteReaderBaseTrait B>
		class IteratorSummary : public internal::IteratorBase<IteratorSummary<B>, B> {
		public:
			using internal::IteratorBase<IteratorSummary<B>, B>::IteratorBase;

			inline bool validate (const int& n) const noexcept {
				return this->_base->hasSummary (n);
			}

			inline const std::pair<RouteData, const std::string> operator* (void) const {
				return std::make_pair<RouteData, const std::string>(this->getSummaryType (), this->getSummary ());
			}

			inline const std::string getSummary (void) const {
				return this->_base->getSummary (this->_n);
			}

			inline RouteData getSummaryType (void) const {
				return this->_base->getSummaryType (this->_n);
			}

			inline bool hasSummary (void) const noexcept {
				return this->_base->hasSummary (this->_n);
			}
		};

		template<class B>
		using SummaryContainer = internal::ContainerBase<IteratorSummary<B>, B>;

		class IteratorPoint : public internal::IteratorBase<IteratorPoint, PointBase> {
		public:
			using IteratorBase<IteratorPoint, PointBase>::IteratorBase;

			inline bool validate (const int& n) const noexcept {
				return this->_base->hasPoint (n);
			}

			inline DataContainer<Point> getData (void) const {
				return DataContainer<Point> (std::make_shared<Point>(this->_base, this->_n));
			}

			inline SummaryContainer<Point> getSummary (void) const {
				return SummaryContainer<Point> (std::make_shared<Point>(this->_base, this->_n));
			}
		};

		using PointContainer = internal::ContainerBase<IteratorPoint, PointBase>;

		class IteratorSegment : public internal::IteratorBase<IteratorSegment, SegmentBase> {
		public:
			using IteratorBase<IteratorSegment, SegmentBase>::IteratorBase;

			inline bool validate (const int& n) const noexcept {
				return this->_base->hasSegment (n);
			}

			inline PointContainer getPoints (void) const {
				return PointContainer (std::make_shared<PointBase>(this->_base, this->_n));
			}

			inline DataContainer<PointBase> getData (void) const {
				return DataContainer<PointBase> (std::make_shared<PointBase>(this->_base, this->_n));
			}

			inline SummaryContainer<PointBase> getSummary (void) const {
				return SummaryContainer<PointBase> (std::make_shared<PointBase>(this->_base, this->_n));
			}
		};

		using SegmentContainer = internal::ContainerBase<IteratorSegment, SegmentBase>;

		class IteratorRoute : public internal::IteratorBase<IteratorRoute, RouteBase> {
		public:
			using IteratorBase<IteratorRoute, RouteBase>::IteratorBase;

			inline bool validate (const int& n) const noexcept {
				return this->_base->hasRoute (n);
			}

			inline SegmentContainer getSegments (void) const {
				return SegmentContainer (std::make_shared<SegmentBase>(this->_base, this->_n));
			}

			inline DataContainer<SegmentBase> getData (void) const {
				return DataContainer<SegmentBase> (std::make_shared<SegmentBase>(this->_base, this->_n));
			}

			inline SummaryContainer<SegmentBase> getSummary (void) const {
				return SummaryContainer<SegmentBase> (std::make_shared<SegmentBase>(this->_base, this->_n));
			}
		};

		using RouteContainer = internal::ContainerBase<IteratorRoute, RouteBase>;

	public:
		virtual RouteContainer getRoutes (void) {
			return RouteContainer (std::make_shared<RouteBase>(this));
		}

		virtual IteratorRoute getRoute (const int& n_route) {
			return IteratorRoute (std::make_shared<RouteBase>(this), n_route);
		}

		virtual SegmentContainer getSegments (const int& n_route) {
			return SegmentContainer (std::make_shared<SegmentBase>(this, n_route));
		}

		virtual IteratorSegment getSegment (const int& n_route, const int& n_segment) {
			return IteratorSegment (std::make_shared<SegmentBase>(this, n_route), n_segment);
		}

		virtual PointContainer getPoints (const int& n_route, const int& n_segment) {
			return PointContainer (std::make_shared<PointBase>(this, n_route, n_segment));
		}

		virtual IteratorPoint getPoint (const int& n_route, const int& n_segment, const int& n_point) {
			return IteratorPoint (std::make_shared<PointBase>(this, n_route, n_segment), n_point);
		}

		const std::string getData (void) const final {
			throw std::runtime_error ("MethodNotAvalitibleInSpecializedProvider");
		}
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERROUTEREADERBASE_ */
