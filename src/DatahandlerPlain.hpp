#ifndef _X_LIBGPSFILE2_DATAHANDLERPLAIN_
#define _X_LIBGPSFILE2_DATAHANDLERPLAIN_

#include <cstddef>
#include <memory>
#include <ostream>
#include <istream>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"

namespace libgpsfile2 {
	class DatahandlerPlainReader : public BaseDatahandlerReaderPlugin {
	public:
		DatahandlerPlainReader (std::shared_ptr<libgpsfile2::provider::ProviderReaderBase>, const std::string&);
		~DatahandlerPlainReader (void);

		bool read (std::ostream *, const std::size_t&) override;

	private:
		std::shared_ptr<libgpsfile2::provider::ProviderReaderBase> data_provider;
	};

	class DatahandlerPlainWriter : public BaseDatahandlerWriterPlugin {
	public:
		DatahandlerPlainWriter (std::shared_ptr<libgpsfile2::provider::ProviderWriterBase>, const std::string&);
		~DatahandlerPlainWriter (void);

		bool write (std::istream *, const bool&) override;

	private:
		std::shared_ptr<libgpsfile2::provider::ProviderWriterBase> data_provider;
	};
}

#endif /* _X_LIBGPSFILE2_DATAHANDLERPLAIN_ */