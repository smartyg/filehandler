#ifndef _X_FILEHANDLER_HANDLER_HANDLERPLAINWRITER_
#define _X_FILEHANDLER_HANDLER_HANDLERPLAINWRITER_

#include <memory>
#include <istream>

#include <filehandler/handler/HandlerWriterBase.hpp>
#include <filehandler/provider/ProviderWriterBase.hpp>

namespace filehandler::handler {
	class HandlerPlainWriter : public HandlerWriterBase {
	public:
		HandlerPlainWriter (std::unique_ptr<provider::ProviderWriterBase>, const std::string&);
		~HandlerPlainWriter (void);

		bool write (std::istream *, const bool&) override;
	};
}

#endif /* _X_FILEHANDLER_HANDLER_HANDLERPLAINWRITER_ */