#ifndef _LIBGPSFILE2_GPSFILEMANAGER_
#define _LIBGPSFILE2_GPSFILEMANAGER_

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

	class GpsfileManager final {
		struct pluginDataFileEntry {
			std::string code;
			int handler_types;
			std::shared_ptr<PluginDetails> details;
			std::shared_ptr<PluginHandler> handlers;

			~pluginDataFileEntry (void) {
				this->code.clear ();
			}
		};

		std::vector<std::size_t> _writer_type_map;
		std::vector<std::size_t> _reader_type_map;

		std::vector<pluginDataFileEntry> _data_file_table;

		GpsfileManager (void);
		GpsfileManager (const GpsfileManager&) = delete;                // copy constructor
		GpsfileManager (GpsfileManager&&) noexcept = delete;            // move constructor
		GpsfileManager& operator= (const GpsfileManager&) = delete;     // copy assignment
		GpsfileManager& operator= (GpsfileManager&&) noexcept = delete; // move assignment

	public:
		constexpr static const std::string_view TAG = "GpsfileManager";

		~GpsfileManager (void);

		std::unique_ptr<handler::HandlerReaderBase> createReader (std::unique_ptr<provider::ProviderReaderBase>, const HandlerType&, const std::string&, const std::string&);
		std::unique_ptr<handler::HandlerWriterBase> createWriter (std::unique_ptr<provider::ProviderWriterBase>, const HandlerType&, const std::string&, const std::string&);

		bool isExtentionMatch (const std::string&, const HandlerType&) const;
		bool isExtentionMatch (const char *, const HandlerType&) const;

		void registar (const char* id, void* data_pointer) {
			(void)id;
			(void)data_pointer;
		}
		void deregistar (const char* id) {
			(void)id;
		}

		const std::vector<std::string> getDataFileExtentions (const HandlerType&);
		const std::shared_ptr<PluginHandler> getDatahandlerFunctions (const std::string&);

		template<provider::ProviderReaderTrait T>
		void registerReaderType (HandlerType& type) {
			assert (!type);
			std::size_t type_hash = typeid (T).hash_code ();
			auto it = std::find (this->_reader_type_map.begin (), this->_reader_type_map.end (), type_hash);
			if (it != this->_reader_type_map.end ()) {
				auto o = std::distance (this->_reader_type_map.begin (), it);
				type.setValue (o, 1, type_hash);
				return;
			}

			int32_t num = this->registerReader (type_hash);
			type.setValue (num, 1, type_hash);
		}

		template<provider::ProviderWriterTrait T>
		void registerWriterType (HandlerType& type) {
			assert (!type);
			const std::size_t type_hash = typeid (T).hash_code ();
			auto it = std::find (this->_writer_type_map.begin (), this->_writer_type_map.end (), type_hash);
			if (it != this->_writer_type_map.end ()) {
				auto o = std::distance (this->_writer_type_map.begin (), it);
				//std::cout << "record num: " << std::to_string (o) << std::endl;
				//std::cout << "size of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.size ()) << std::endl;
				//std::cout << "capacity of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.capacity ()) << std::endl;
				type.setValue (o, 2, type_hash);
				return;
			}

			int32_t num = this->registerWriter (type_hash);
			type.setValue (num, 2, type_hash);
		}

		[[nodiscard]] static const std::shared_ptr<GpsfileManager> getPtr (void) {
			static std::shared_ptr<GpsfileManager> instance;

			if (instance) return instance;
			else return (instance = std::shared_ptr<GpsfileManager>(new GpsfileManager ()));
		}

	private:
		int32_t registerWriter (const std::size_t&);
		int32_t registerReader (const std::size_t&);
	};
}

#endif /* _LIBGPSFILE2_GPSFILEMANAGER_ */