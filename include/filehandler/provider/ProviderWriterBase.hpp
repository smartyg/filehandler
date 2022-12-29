#ifndef _FILEHANDLER_PROVIDER_PROVIDERWRITERBASE_
#define _FILEHANDLER_PROVIDER_PROVIDERWRITERBASE_

#include <string>

#include <filehandler/provider/ProviderBase.hpp>

namespace filehandler::provider {

	class ProviderWriterBase : public ProviderBase {
	public:
		virtual ~ProviderWriterBase (void) = default;

		virtual bool setData (const std::string&) const = 0;
		virtual void finished (void) = 0;
	};
}

#endif /* _FILEHANDLER_PROVIDER_PROVIDERWRITERBASE_ */