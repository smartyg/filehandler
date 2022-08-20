#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERROUTEREADERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERROUTEREADERBASE_

#include <string>
#include <gpsdata/types/ObjectTime.hpp>

#include <libgpsfile2/provider/ProviderWriterBase.hpp>

namespace libgpsfile2::provider {

	// Data providers used for data handlers from plugins.
	class ProviderRouteReaderBase : public ProviderReaderBase {
	public:
		virtual ~ProviderRouteReaderBase (void) = default;

		/* TODO: expand this base class to something workable */

		//virtual GpsRoute_ptr_const getRoute (void) const = 0;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERROUTEREADERBASE_ */
