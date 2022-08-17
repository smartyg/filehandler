#ifndef _X_PRIVATE_DATAPROVIDERROUTEWRITERPRIVATE_HPP_
#define _X_PRIVATE_DATAPROVIDERROUTEWRITERPRIVATE_HPP_

#include <memory>
#include <string>
#include <memory>

#include "provider.hpp"

#include <gpsdata.hpp>
#include <gpsdata/traits/GpsRoute.hpp>

namespace libgpsfile2::provider {
	template<gpsdata::GpsRouteTrait R>
	class ProviderGpsRouteWriter final : public ProviderRouteWriterBase {//, std::enable_shared_from_this<ProviderGpsRouteWriter<R>> {
		using GpsFactory = typename R::GpsFactory;

		const std::shared_ptr<const GpsFactory> _factory;
		std::vector<std::shared_ptr<R>> _routes;
		bool _finished;

		ProviderGpsRouteWriter (const std::shared_ptr<const GpsFactory>& factory) : _factory(factory), _finished(false) { }

		ProviderGpsRouteWriter (void) = delete;
		ProviderGpsRouteWriter (const ProviderGpsRouteWriter&) = delete;                // copy constructor
		ProviderGpsRouteWriter (ProviderGpsRouteWriter&&) noexcept = delete;            // move constructor
		ProviderGpsRouteWriter& operator= (const ProviderGpsRouteWriter&) = delete;     // copy assignment
		ProviderGpsRouteWriter& operator= (ProviderGpsRouteWriter&&) noexcept = delete; // move assignment

	public:
		~ProviderGpsRouteWriter (void) override {
			this->_finished = true;
			this->_routes.clear ();
		}
/*
		std::shared_ptr<ProviderGpsRouteWriter<R>> getptr (void) {
			return this->shared_from_this ();
		}
*/
		bool setNumTracks (const unsigned short& num) override {
			if (this->_finished) return false;
			if (num < this->_routes.size ()) return false;
			this->_routes.resize (num);
			for (auto& route : this->_routes) {
				if (!route) route = R::template create<R> (this->_factory);
			}
			return true;
		}

		bool setTrackTitle (const unsigned short& route_num, const std::string& title) override {
			if (this->_finished) return false;
			return this->_routes.at (route_num)->setTitle (title);
		}

		bool setTrackSummary (const unsigned short& route_num, const std::string& summary) override {
			if (this->_finished) return false;
			return this->_routes.at (route_num)->setSummary (summary);
		}

		bool setTrackDetails (const unsigned short& route_num, const std::string& details) override {
			if (this->_finished) return false;
			return this->_routes.at (route_num)->setDetails (details);
		}

		bool setTrackActivityType (const unsigned short& route_num, const std::string& type) override {
			if (this->_finished) return false;
			return this->_routes.at (route_num)->setActivityType (type);
		}

		bool setTrackOriginalHash (const unsigned short& route_num, const std::string& hash) override {
			if (this->_finished) return false;
			(void) route_num;
			(void) hash;
			return true;
		}

		bool addTrackPoint (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time) override {
			if (this->_finished) return false;
			if (this->_routes.at (route_num)->hasPoint (time)) return false;
			return this->_routes.at (route_num)->addPoint (segment_num, time);
		}

		bool addTrackPointData (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time, const std::string& type, const std::string& value) override {
			if (this->_finished) return false;
			if (!this->_routes.at (route_num)->hasPoint (time)) this->addTrackPoint (route_num, segment_num, time);

			return this->_routes.at (route_num)->addPointData (segment_num, time, type, value, true);
		}

		bool addTrackPointData (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time, const std::string& type, int value) override {
			if (this->_finished) return false;
			if (!this->_routes.at (route_num)->hasPoint (time)) this->addTrackPoint (route_num, segment_num, time);

			return this->_routes.at (route_num)->addPointData (segment_num, time, type, value);
		}

		bool addTrackPointData (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time, const std::string& type, long long value) override {
			if (this->_finished) return false;
			if (!this->_routes.at (route_num)->hasPoint (time)) this->addTrackPoint (route_num, segment_num, time);

			return this->_routes.at (route_num)->addPointData (segment_num, time, type, value);
		}

		bool addTrackPointData (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time, const std::string& type, float value) override {
			if (this->_finished) return false;
			if (!this->_routes.at (route_num)->hasPoint (time)) this->addTrackPoint (route_num, segment_num, time);

			return this->_routes.at (route_num)->addPointData (segment_num, time, type, value);
		}

		bool addTrackPointData (const unsigned short& route_num, const unsigned short& segment_num, const gpsdata::ObjectTime& time, const std::string& type, double value) override {
			if (this->_finished) return false;
			if (!this->_routes.at (route_num)->hasPoint (time)) this->addTrackPoint (route_num, segment_num, time);

			return this->_routes.at (route_num)->addPointData (segment_num, time, type, value);
		}

		bool setData (const std::string&) const override {
			//throw libsad::exception::MethodNotAvalitibleInSpecializedProvider ();
			throw std::runtime_error ("MethodNotAvalitibleInSpecializedProvider");
			return false;
		}

		void finished (void) override {
			if (this->_finished) return;

			this->_finished = true;
			return;
		}

		std::vector<std::shared_ptr<R>> getRoutes (void) const noexcept {
			return this->_routes;
		}
/*
		[[nodiscard]] static std::shared_ptr<ProviderGpsRouteWriter<R>> create (const std::shared_ptr<const GpsFactory>& factory) {
			return std::shared_ptr<ProviderGpsRouteWriter<R>>(new ProviderGpsRouteWriter (factory));
		}*/
		[[nodiscard]] static std::unique_ptr<ProviderGpsRouteWriter<R>> create (const std::shared_ptr<const GpsFactory>& factory) {
			return std::unique_ptr<ProviderGpsRouteWriter<R>>(new ProviderGpsRouteWriter (factory));
		}
	};
}

#endif /* _X_PRIVATE_DATAPROVIDERROUTEWRITERPRIVATE_HPP_ */
