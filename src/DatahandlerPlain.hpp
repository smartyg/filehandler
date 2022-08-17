#ifndef _X_LIBGPSFILE2_DATAHANDLERPLAIN_
#define _X_LIBGPSFILE2_DATAHANDLERPLAIN_

#include <cstddef>
#include <memory>
#include <ostream>
#include <istream>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"

namespace libgpsfile2 {
	class HandlerPlainReader : public HandlerReaderBase {
	public:
		HandlerPlainReader (std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
		~HandlerPlainReader (void);

		bool read (std::ostream *, const std::size_t&) override;
	};

	class HandlerPlainWriter : public HandlerWriterBase {
	public:
		HandlerPlainWriter (std::unique_ptr<provider::ProviderWriterBase>, const std::string&);
		~HandlerPlainWriter (void);

		bool write (std::istream *, const bool&) override;
	};
}

#endif /* _X_LIBGPSFILE2_DATAHANDLERPLAIN_ */