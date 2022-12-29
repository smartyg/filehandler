#ifndef _FILEHANDLER_PROVIDER_PROVIDERREADERBASE_
#define _FILEHANDLER_PROVIDER_PROVIDERREADERBASE_

#include <string>

#include <filehandler/provider/ProviderBase.hpp>

namespace filehandler::provider {

	// Base data handler class
	class ProviderReaderBase : public ProviderBase {
	public:
		virtual ~ProviderReaderBase (void) = default;
		virtual const std::string getData (void) const = 0;
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERREADERBASE_ */