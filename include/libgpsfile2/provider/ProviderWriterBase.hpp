#ifndef _LIBGPSFILE2_PROVIDER_PROVIDERWRITERBASE_
#define _LIBGPSFILE2_PROVIDER_PROVIDERWRITERBASE_

#include <string>

#include <libgpsfile2/provider/ProviderBase.hpp>

namespace libgpsfile2::provider {

	class ProviderWriterBase : virtual public ProviderBase {
	public:
		virtual ~ProviderWriterBase (void) = default;

		virtual bool setData (const std::string&) const = 0;
		virtual void finished (void) = 0;
	};
}

#endif /* _LIBGPSFILE2_PROVIDER_PROVIDERWRITERBASE_ */