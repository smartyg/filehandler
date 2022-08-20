#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEREADER_
#define _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEREADER_

#include <memory>
#include <vector>
#include <string>
#include <gpsdata.hpp>
#include <gpsdata/traits/GpsRoute.hpp>

#include <libgpsfile2/provider/ProviderRouteReaderBase.hpp>

namespace libsad::dataprovider {

	template<gpsdata::GpsRouteTrait R>
	class ProviderGpsRouteReader final : public ProviderRouteReaderBase {

		std::vector<std::shared_ptr<R>> _routes;
		bool _finished;

		ProviderGpsRouteWriter (const std::vector<std::shared_ptr<R>>& routes) : _route(routes), _finished(false) { }

		ProviderGpsRouteWriter (void) = delete;
		ProviderGpsRouteWriter (const ProviderGpsRouteWriter&) = delete;                // copy constructor
		ProviderGpsRouteWriter (ProviderGpsRouteWriter&&) noexcept = delete;            // move constructor
		ProviderGpsRouteWriter& operator= (const ProviderGpsRouteWriter&) = delete;     // copy assignment
		ProviderGpsRouteWriter& operator= (ProviderGpsRouteWriter&&) noexcept = delete; // move assignment

		GpsRoute_ptr_const getRoute (void) const override;
		const std::string getData (void) const override;

		[[nodiscard]] static std::unique_ptr<ProviderGpsRouteReader<R>> create (const std::vector<std::shared_ptr<R>>& routes) {
			return std::unique_ptr<ProviderGpsRouteReader<R>>(new ProviderGpsRouteReader (routes));
		}
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERGPSROUTEREADER_ */
