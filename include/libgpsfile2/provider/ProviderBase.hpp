#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERBASE_

#include <libgpsfile2/types/BasePlugin.hpp>

namespace libgpsfile2::provider {

	// Base data handler class
	class ProviderBase : public libgpsfile2::BasePlugin {
	public:
		virtual ~ProviderBase (void) = default;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERBASE_ */
