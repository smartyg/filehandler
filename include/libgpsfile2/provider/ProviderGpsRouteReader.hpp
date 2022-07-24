#ifndef _X_PRIVATE_DATAPROVIDERROUTEREADERPRIVATE_HPP_
#define _X_PRIVATE_DATAPROVIDERROUTEREADERPRIVATE_HPP_

#include <list>
#include <memory>

namespace libsad {
	namespace dataprovider {
		class DataProviderRouteReaderPrivate;
	}

	inline namespace types {
		typedef std::shared_ptr<libsad::dataprovider::DataProviderRouteReaderPrivate> DataProviderRouteReaderPrivate_ptr;
	}
}

#include "libsad.hpp"
#include "../libsadprivate.hpp"
#include "../db/databasetransaction.hpp"
#include "../gpsdata/gpsdatafactoryprivate.hpp"
#include "../gpsdata/gpsrouteprivate.hpp"
#include "../utils/asyncfactory.hpp"

namespace libsad::dataprovider {
	using namespace libsad::types;
	using libsad::db::DatabaseTransaction;
	using libsad::gpsdata::GpsDataFactoryPrivate;
	using libsad::utils::AsyncFactory;

	namespace internal {
		using namespace libsad::types;
		using libsad::gpsdata::GpsRoutePrivate;
		using libsad::db::DatabaseTransaction;

		class GpsRouteLoader {
		private:
			GpsRoutePrivate_ptr _route;
		public:
			GpsRouteLoader (DatabaseTransaction_ptr db, const FilterInfoType& fi) {
				(void)db;
				(void)fi;
				// TODO: write function
			}

			GpsRoutePrivate_ptr_const getRoute (void) const {
				return this->_route;
			}
		};
	}

	class DataProviderRouteReaderPrivate final : public DataProviderRouteReader {
	public:
		DataProviderRouteReaderPrivate (LibsadPrivate_ptr, const FilterInfoType&);
		~DataProviderRouteReaderPrivate (void) override;

		GpsRoute_ptr_const getRoute (void) const override;
		const std::string getData (void) const override;

	private:
		LibsadPrivate_ptr _lib;
		const FilterInfoType _fi;
		DatabaseTransaction_ptr _db;
		AsyncFactory<internal::GpsRouteLoader, DatabaseTransaction_ptr, const FilterInfoType&> *_route;
	};
}

#endif /* _X_PRIVATE_DATAPROVIDERROUTEREADERPRIVATE_HPP_ */
