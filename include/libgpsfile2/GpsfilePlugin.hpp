#ifndef _X_GPSFILE2_GPSFILEPLUGIN_
#define _X_GPSFILE2_GPSFILEPLUGIN_

#include <stddef.h>
#include <dirent.h>
#include <cstdint>
#include <vector>
#include <memory>

#include "Datahandler.hpp"
#include "provider/provider.hpp"

namespace libgpsfile2 {
	enum DatahandlerType : uint8_t {
		PLUGIN_TYPE_DATA_FILE_NONE = 0,
		PLUGIN_TYPE_DATA_FILE_READ = 1,
		PLUGIN_TYPE_DATA_FILE_WRITE = 1 << 1,
		PLUGIN_TYPE_DATA_FILE_WAYPOINT = 1 << 2,
		PLUGIN_TYPE_DATA_FILE_TABLE = 1 << 3,
	} ;

	struct pluginDetails {
		const char *name;
		const char *author;
		const char *license;
		uint8_t major_version:8;
		uint8_t minor_version:8;
	};

	class GpsfilePlugin final : std::enable_shared_from_this<GpsfilePlugin> {
		struct pluginDataFileEntry {
			std::string code;
			DatahandlerType handlers;
			pluginDetails details;
			pluginDatahandler data;

			~pluginDataFileEntry (void) {
				this->code.clear ();
			}
		};

		static std::vector<pluginDataFileEntry> data_file_table;
		static int instance_counter;

		GpsfilePlugin (void);
		GpsfilePlugin (const GpsfilePlugin&) = delete;                // copy constructor
		GpsfilePlugin (GpsfilePlugin&&) noexcept = delete;            // move constructor
		GpsfilePlugin& operator= (const GpsfilePlugin&) = delete;     // copy assignment
		GpsfilePlugin& operator= (GpsfilePlugin&&) noexcept = delete; // move assignment

	public:
		~GpsfilePlugin (void);

		std::shared_ptr<GpsfilePlugin> getptr (void) {
			return this->shared_from_this ();
		}

		BaseDatahandlerReaderPlugin *createReader (std::shared_ptr<libgpsfile2::provider::ProviderReaderBase>, const DatahandlerType&, const std::string&, const std::string&);
		BaseDatahandlerWriterPlugin *createWriter (std::shared_ptr<libgpsfile2::provider::ProviderWriterBase>, const DatahandlerType&, const std::string&, const std::string&);

		template<class T>
		BaseDatahandlerReaderPlugin *createReader (std::shared_ptr<T> provider, const DatahandlerType& t, const std::string& s1, const std::string& s2) {
			static_assert (std::is_base_of<libgpsfile2::provider::ProviderReaderBase, T>::value);
			std::shared_ptr<libgpsfile2::provider::ProviderReaderBase> provider_base = std::static_pointer_cast<libgpsfile2::provider::ProviderReaderBase> (provider);
			return this->createReader (provider_base, t, s1, s2);
		}

		template<class T>
		BaseDatahandlerWriterPlugin *createWriter (std::shared_ptr<T> provider, const DatahandlerType& t, const std::string& s1, const std::string& s2) {
			static_assert (std::is_base_of<libgpsfile2::provider::ProviderWriterBase, T>::value);
			std::shared_ptr<libgpsfile2::provider::ProviderWriterBase> provider_base = std::static_pointer_cast<libgpsfile2::provider::ProviderWriterBase> (provider);
			return this->createWriter (provider_base, t, s1, s2);
		}

		bool isExtentionMatch (const std::string&, const DatahandlerType&) const;
		bool isExtentionMatch (const char *, const DatahandlerType&) const;

		static void registerPlugin (const char *, const pluginDetails&, void *);
		static void removePlugin (const char *);

		static const std::vector<std::string> getDataFileExtentions (const DatahandlerType&);

		static const pluginDatahandler *getDatahandlerFunctions (const std::string&);

		[[nodiscard]] static std::shared_ptr<GpsfilePlugin> create (void) {
			return std::shared_ptr<GpsfilePlugin>(new GpsfilePlugin ());
		}

	private:
		static void loadPlugins (void);
		static int loadPluginFilter (const struct dirent *);
		static bool addDataFileEntry (const std::string&, const pluginDetails&, const pluginDatahandler *);
		static bool removeDataFileEntry (const std::string&);
	};
}

#endif /* _X_GPSFILE2_GPSFILEPLUGIN_ */
