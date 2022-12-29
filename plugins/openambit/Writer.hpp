#ifndef _PLUGIN_OPENAMBIT_WRITER_
#define _PLUGIN_OPENAMBIT_WRITER_

#include <memory>
#include <istream>
#include <future>
#include <libxml/xmlreader.h>

#include <filehandler/handler/HandlerWriterBase.hpp>
#include <filehandler/provider/types/route/ProviderRouteWriterBase.hpp>

#include "Plugin.hpp"

namespace plugin::openambit {
	class Writer final : public filehandler::handler::HandlerWriterBase {
	public:
		Writer (const std::shared_ptr<Plugin>, std::unique_ptr<filehandler::provider::ProviderRouteWriterBase>, const std::string&);
		~Writer (void);

	private:
		bool write (std::istream *, const bool&) override;

		std::shared_ptr<Plugin> _base_instance;
		filehandler::provider::ProviderRouteWriterBase* _provider;
		std::future<bool> _reader;
	};
}

#endif /* _PLUGIN_OPENAMBIT_WRITER_ */