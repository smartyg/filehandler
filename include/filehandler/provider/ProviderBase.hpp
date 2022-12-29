#ifndef _FILEHANDLER_PROVIDER_PROVIDERBASE_
#define _FILEHANDLER_PROVIDER_PROVIDERBASE_

#include <filehandler/types/BasePlugin.hpp>

namespace filehandler::provider {

	// Base data handler class
	class ProviderBase : public filehandler::BasePlugin {
	public:
		virtual ~ProviderBase (void) = default;
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERBASE_ */
