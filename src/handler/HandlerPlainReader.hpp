#ifndef _X_FILEHANDLER_HANDLER_HANDLERPLAINREADER_
#define _X_FILEHANDLER_HANDLER_HANDLERPLAINREADER_

#include <memory>
#include <ostream>

#include <filehandler/handler/HandlerReaderBase.hpp>
#include <filehandler/provider/ProviderReaderBase.hpp>

namespace filehandler::handler {
	class HandlerPlainReader : public HandlerReaderBase {
	public:
		HandlerPlainReader (std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
		~HandlerPlainReader (void);

		bool read (std::ostream *, const std::size_t&) override;
	};
}

#endif /* _X_FILEHANDLER_HANDLER_HANDLERPLAINREADER_ */