#ifndef _LIBGPSFILE2_HANDLER_HANDLERPLAINREADER_
#define _LIBGPSFILE2_HANDLER_HANDLERPLAINREADER_

#include <memory>
#include <ostream>

#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>

namespace libgpsfile2::handler {
	class HandlerPlainReader : public HandlerReaderBase {
	public:
		HandlerPlainReader (std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
		~HandlerPlainReader (void);

		bool read (std::ostream *, const std::size_t&) override;
	};
}

#endif /* _LIBGPSFILE2_HANDLER_HANDLERPLAINREADER_ */