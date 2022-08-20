#ifndef _LIBGPSFILE2_GPSFILEPLUGIN_
#define _LIBGPSFILE2_GPSFILEPLUGIN_

#include <memory>
#include <iterator>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

#include <libgpsfile2/PluginHandler.hpp>
#include <libgpsfile2/types/HandlerType.hpp>
#include <libgpsfile2/types/PluginDetails.hpp>
#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/traits/ProviderReaderTrait.hpp>
#include <libgpsfile2/traits/ProviderWriterTrait.hpp>

namespace libgpsfile2 {

	class GpsfilePlugin final : std::enable_shared_from_this<GpsfilePlugin> {
		struct pluginDataFileEntry {
			std::string code;
			int handler_types;
			std::shared_ptr<PluginDetails> details;
			std::shared_ptr<PluginHandler> handlers;

			~pluginDataFileEntry (void) {
				this->code.clear ();
			}
		};

		static std::vector<std::size_t> _writer_type_map;
		static std::vector<std::size_t> _reader_type_map;

		static std::vector<pluginDataFileEntry> data_file_table;
		static int instance_counter;

		GpsfilePlugin (const std::list<std::string>& = {});
		GpsfilePlugin (const GpsfilePlugin&) = delete;                // copy constructor
		GpsfilePlugin (GpsfilePlugin&&) noexcept = delete;            // move constructor
		GpsfilePlugin& operator= (const GpsfilePlugin&) = delete;     // copy assignment
		GpsfilePlugin& operator= (GpsfilePlugin&&) noexcept = delete; // move assignment

	public:
		~GpsfilePlugin (void);

		std::shared_ptr<GpsfilePlugin> getptr (void) {
			return this->shared_from_this ();
		}

		std::unique_ptr<handler::HandlerReaderBase> createReader (std::unique_ptr<provider::ProviderReaderBase>, const HandlerType&, const std::string&, const std::string&);
		std::unique_ptr<handler::HandlerWriterBase> createWriter (std::unique_ptr<provider::ProviderWriterBase>, const HandlerType&, const std::string&, const std::string&);

		bool isExtentionMatch (const std::string&, const HandlerType&) const;
		bool isExtentionMatch (const char *, const HandlerType&) const;

		static void registerPlugin (const std::string, const std::shared_ptr<PluginDetails>, const std::shared_ptr<PluginHandler>);
		static void removePlugin (const std::string);

		const std::vector<std::string> getDataFileExtentions (const HandlerType&);

		const std::shared_ptr<PluginHandler> getDatahandlerFunctions (const std::string&);

		template<provider::ProviderReaderTrait T>
		static void registerReaderType (HandlerType& type) {
			assert (!type);
			std::size_t type_hash = typeid (T).hash_code ();
			auto it = std::find (GpsfilePlugin::_reader_type_map.begin (), GpsfilePlugin::_reader_type_map.end (), type_hash);
			if (it != GpsfilePlugin::_reader_type_map.end ()) {
				auto o = std::distance (GpsfilePlugin::_reader_type_map.begin (), it);
				type.setValue (o, 1, type_hash);
				return;
			}

			int32_t num = GpsfilePlugin::registerReader (type_hash);
			type.setValue (num, 1, type_hash);
		}

		template<provider::ProviderWriterTrait T>
		static void registerWriterType (HandlerType& type) {
			assert (!type);
			const std::size_t type_hash = typeid (T).hash_code ();
			auto it = std::find (GpsfilePlugin::_writer_type_map.begin (), GpsfilePlugin::_writer_type_map.end (), type_hash);
			if (it != GpsfilePlugin::_writer_type_map.end ()) {
				auto o = std::distance (GpsfilePlugin::_writer_type_map.begin (), it);
				//std::cout << "record num: " << std::to_string (o) << std::endl;
				//std::cout << "size of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.size ()) << std::endl;
				//std::cout << "capacity of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.capacity ()) << std::endl;
				type.setValue (o, 2, type_hash);
				return;
			}

			int32_t num = GpsfilePlugin::registerWriter (type_hash);
			type.setValue (num, 2, type_hash);
		}

		[[nodiscard]] static std::shared_ptr<GpsfilePlugin> create (const std::list<std::string>& scan_list = {}) {
			return std::shared_ptr<GpsfilePlugin>(new GpsfilePlugin (scan_list));
		}

	private:
		void loadPlugins (const std::list<std::string>&, bool = true);

		static int32_t registerWriter (const std::size_t&);
		static int32_t registerReader (const std::size_t&);
	};
}

#endif /* _LIBGPSFILE2_GPSFILEPLUGIN_ */