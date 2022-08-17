#ifndef _X_GPSFILE2_GPSFILEPLUGIN_
#define _X_GPSFILE2_GPSFILEPLUGIN_

#include <stddef.h>
#include <dirent.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>

#include <iostream>

#include "Datahandler.hpp"
//#include "provider/provider.hpp"
#include "traits/provider.hpp"
#include "traits/Datahandler.hpp"
//#include "traits/HandlerProviderPair.hpp"
#include "ConstValue.hpp"

namespace libgpsfile2 {/*
	enum DatahandlerType : uint8_t {
		PLUGIN_TYPE_DATA_FILE_NONE = 0,
		PLUGIN_TYPE_DATA_FILE_READ = 1,
		PLUGIN_TYPE_DATA_FILE_WRITE = 1 << 1,
		PLUGIN_TYPE_DATA_FILE_WAYPOINT = 1 << 2,
		PLUGIN_TYPE_DATA_FILE_TABLE = 1 << 3,
	} ;
*/
	typedef ConstValue<uint8_t> DatahandlerType;

	struct pluginDetails {
		const char *name;
		const char *author;
		const char *license;
		uint8_t major_version;
		uint8_t minor_version;
	};

	class GpsfilePlugin final : std::enable_shared_from_this<GpsfilePlugin> {
		struct pluginDataFileEntry {
			std::string code;
			int handler_types;
			std::shared_ptr<pluginDetails> details;
			std::shared_ptr<pluginDatahandler> handlers;

			~pluginDataFileEntry (void) {
				this->code.clear ();
			}
		};

		typedef std::unique_ptr<HandlerWriterBase> (*WriterConvFunc) (const std::shared_ptr<pluginDatahandler>, std::unique_ptr<provider::ProviderWriterBase>, const DatahandlerType&, const std::string&);
		typedef std::unique_ptr<HandlerReaderBase> (*ReaderConvFunc) (const std::shared_ptr<pluginDatahandler>, std::unique_ptr<provider::ProviderReaderBase>, const DatahandlerType&, const std::string&);

		static std::map<std::size_t, WriterConvFunc> _writer_func_map;
		static std::map<std::size_t, ReaderConvFunc> _reader_func_map;
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

		std::unique_ptr<HandlerReaderBase> createReader (std::unique_ptr<provider::ProviderReaderBase>, const DatahandlerType&, const std::string&, const std::string&);
		std::unique_ptr<HandlerWriterBase> createWriter (std::unique_ptr<provider::ProviderWriterBase>, const DatahandlerType&, const std::string&, const std::string&);
/*
		template<provider::ProviderReaderTrait T>
		std::unique_ptr<HandlerReaderBase> createReader (std::shared_ptr<T> provider, const DatahandlerType& t, const std::string& s1, const std::string& s2) {
			//static_assert (std::is_base_of<libgpsfile2::provider::ProviderReaderBase, T>::value);
			std::shared_ptr<provider::ProviderReaderBase> provider_base = std::static_pointer_cast<provider::ProviderReaderBase> (provider);
			return this->createReader (provider_base, t, s1, s2);
		}

		template<provider::ProviderWriterTrait T>
		std::unique_ptr<HandlerWriterBase> createWriter (std::shared_ptr<T> provider, const DatahandlerType& t, const std::string& s1, const std::string& s2) {
			//static_assert (std::is_base_of<libgpsfile2::provider::ProviderWriterBase, T>::value);
			std::shared_ptr<provider::ProviderWriterBase> provider_base = std::static_pointer_cast<provider::ProviderWriterBase> (provider);
			return this->createWriter (provider_base, t, s1, s2);
		}
*/
		bool isExtentionMatch (const std::string&, const DatahandlerType&) const;
		bool isExtentionMatch (const char *, const DatahandlerType&) const;

		static void registerPlugin (const std::string, const std::shared_ptr<pluginDetails>, const std::shared_ptr<pluginDatahandler>);
		static void removePlugin (const std::string);

		static const std::vector<std::string> getDataFileExtentions (const DatahandlerType&);

		static const std::shared_ptr<pluginDatahandler> getDatahandlerFunctions (const std::string&);

		template<provider::ProviderReaderTrait T>
		static void registerReaderType (DatahandlerType& type) {
			std::size_t type_hash = typeid (T).hash_code ();
			if (GpsfilePlugin::_reader_func_map.contains (type_hash)) {
				auto it = std::find (GpsfilePlugin::_reader_type_map.begin (), GpsfilePlugin::_reader_type_map.end (), type_hash);
				if (it != GpsfilePlugin::_reader_type_map.end ()) {
					auto o = std::distance (GpsfilePlugin::_reader_type_map.begin (), it);
					type.setValue<GpsfilePlugin> (o);
					return;
				}
				return;
			}
			ReaderConvFunc func = [] (const std::shared_ptr<pluginDatahandler> dh, std::unique_ptr<provider::ProviderReaderBase> provider, const DatahandlerType& dht, const std::string& path) -> std::unique_ptr<HandlerReaderBase> {
				// Check that `provider` is of type P, otherwise throw exception
				provider::ProviderReaderBase *provider_base_ptr = provider.get ();
				if (dynamic_cast<T*>(provider_base_ptr) == nullptr) throw std::runtime_error ("Can not cast provider");

				auto it = dh->reader_creators.find (static_cast<int>(dht));
				if (it == dh->reader_creators.end ()) return nullptr;

				std::unique_ptr<HandlerReaderBase> handler = (*it).second (dh->instance, std::move (provider), path);

				return handler;
			};

			int32_t num = GpsfilePlugin::registerReader (type_hash, func);
			type.setValue<GpsfilePlugin> (num);
		}

		template<provider::ProviderWriterTrait T>
		static void registerWriterType (DatahandlerType& type) {
			const std::size_t type_hash = typeid (T).hash_code ();
std::cout << "register type with hash: " << std::to_string (type_hash) << std::endl;
			if (GpsfilePlugin::_writer_func_map.contains (type_hash)) {
				std::cout << "already found existing type" << std::endl;
				auto it = std::find (GpsfilePlugin::_writer_type_map.begin (), GpsfilePlugin::_writer_type_map.end (), type_hash);
				if (it != GpsfilePlugin::_writer_type_map.end ()) {
					auto o = std::distance (GpsfilePlugin::_writer_type_map.begin (), it);
					std::cout << "record num: " << std::to_string (o) << std::endl;
					std::cout << "size of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.size ()) << std::endl;
					std::cout << "capacity of _reader_type_map: " << std::to_string (GpsfilePlugin::_reader_type_map.capacity ()) << std::endl;
					type.setValue<GpsfilePlugin> (o);
					return;
				}
				return;
			}

			WriterConvFunc func = [] (const std::shared_ptr<pluginDatahandler> dh, std::unique_ptr<provider::ProviderWriterBase> provider, const DatahandlerType& dht, const std::string& path) -> std::unique_ptr<HandlerWriterBase> {

				// Check that `provider` is of type P, otherwise throw exception
				provider::ProviderWriterBase *provider_base_ptr = provider.get ();
				if (dynamic_cast<T*>(provider_base_ptr) == nullptr) throw std::runtime_error ("Can not cast provider");

				auto it = dh->writer_creators.find (static_cast<int>(dht));
				if (it == dh->writer_creators.end ()) return nullptr;

				std::unique_ptr<HandlerWriterBase> handler = (*it).second (dh->instance, std::move (provider), path);

				return handler;
			};

			int32_t num = GpsfilePlugin::registerWriter (type_hash, func);
			type.setValue<GpsfilePlugin> (num);
		}

		[[nodiscard]] static std::shared_ptr<GpsfilePlugin> create (const std::list<std::string>& scan_list = {}) {
			return std::shared_ptr<GpsfilePlugin>(new GpsfilePlugin (scan_list));
		}

	private:
		void loadPlugins (const std::list<std::string>&, bool = true);
		static int loadPluginFilter (const struct dirent *);
		//static bool addDataFileEntry (const std::string&, const pluginDetails&, const pluginDatahandler *);
		//static bool removeDataFileEntry (const std::string&);

		static int32_t registerWriter (const std::size_t&, WriterConvFunc);
		static int32_t registerReader (const std::size_t&, ReaderConvFunc);

		static WriterConvFunc getWriterFunc (const DatahandlerType&);
		static ReaderConvFunc getReaderFunc (const DatahandlerType&);
	};
}

#endif /* _X_GPSFILE2_GPSFILEPLUGIN_ */
