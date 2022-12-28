#ifndef _LIBGPSFILE2_HANDLER_HANDLERPLAINWRITER_
#define _LIBGPSFILE2_HANDLER_HANDLERPLAINWRITER_

#include <memory>
#include <istream>

#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>

namespace libgpsfile2::handler {
	class HandlerPlainWriter : public HandlerWriterBase {
	public:
		HandlerPlainWriter (std::unique_ptr<provider::ProviderWriterBase>, const std::string&);
		~HandlerPlainWriter (void);

		bool write (std::istream *, const bool&) override;
	};
}

#endif /* _LIBGPSFILE2_HANDLER_HANDLERPLAINWRITER_ */