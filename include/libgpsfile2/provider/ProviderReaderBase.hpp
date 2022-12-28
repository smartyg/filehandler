#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERREADERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERREADERBASE_

#include <string>

#include <libgpsfile2/provider/ProviderBase.hpp>

namespace libgpsfile2::provider {

	// Base data handler class
	class ProviderReaderBase : public ProviderBase {
	public:
		virtual ~ProviderReaderBase (void) = default;
		virtual const std::string getData (void) const = 0;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERREADERBASE_ */