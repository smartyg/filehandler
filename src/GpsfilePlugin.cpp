#include "config.h"

#include "libgpsfile2/GpsfilePlugin.hpp"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <dirent.h>
#include <dlfcn.h>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"

#include "DatahandlerPlain.hpp"

#ifndef IS_SET
#define IS_SET(v, m) (((v) & (m)) == (m))
#endif

using libgpsfile2::GpsfilePlugin;
using libgpsfile2::BaseDatahandlerReaderPlugin;
using libgpsfile2::BaseDatahandlerWriterPlugin;
using libgpsfile2::pluginDatahandler;

std::vector<GpsfilePlugin::pluginDataFileEntry> GpsfilePlugin::data_file_table;

int GpsfilePlugin::instance_counter = 0;

GpsfilePlugin::GpsfilePlugin (void) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	/* TODO */
	// get exclusive lock (mutex)
	this->instance_counter++;
	GpsfilePlugin::loadPlugins ();
	// release exclusive lock
}

GpsfilePlugin::~GpsfilePlugin (void) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	/* TODO */
	// get exclusive lock (mutex)
	this->instance_counter--;
	if (this->instance_counter == 0) {
		GpsfilePlugin::data_file_table.clear ();
	}
	// release exclusive lock
}

BaseDatahandlerReaderPlugin *GpsfilePlugin::createReader (std::shared_ptr<libgpsfile2::provider::ProviderReaderBase> dpr, const DatahandlerType& dht, const std::string& path, const std::string& type) {
	// Variable to store the data handler in.
	BaseDatahandlerReaderPlugin *dhr = nullptr;

	// Select the correct data handler class.
	if (dht == PLUGIN_TYPE_DATA_FILE_NONE) dhr = new libgpsfile2::DatahandlerPlainReader (dpr, path);
		else if (dht == PLUGIN_TYPE_DATA_FILE_READ) {
			// Get the struct that contains all the data handler creator functions.
			const pluginDatahandler *dh = this->getDatahandlerFunctions (type);

			// Check if this plugin supports the requested data handler type.
			if (dh->route_read_class == nullptr) return nullptr;

			// Run the data handler creator function to get and instance of te data handler class.
			dhr = dh->route_read_class (dh->instance, std::dynamic_pointer_cast<libgpsfile2::provider::ProviderRouteReaderBase>(dpr), path);
		} else if (dht == PLUGIN_TYPE_DATA_FILE_WAYPOINT) {
			// Get the struct that contains all the data handler creator functions.
			const pluginDatahandler *dh = this->getDatahandlerFunctions (type);

			// Check if this plugin supports the requested data handler type.
			if (dh->waypoint_read_class == nullptr) return nullptr;

			// Run the data handler creator function to get and instance of te data handler class.
			dhr = dh->waypoint_read_class (dh->instance, std::dynamic_pointer_cast<libgpsfile2::provider::ProviderWaypointReaderBase>(dpr), path);
		} else if (dht == PLUGIN_TYPE_DATA_FILE_TABLE) {
			// Get the struct that contains all the data handler creator functions.
			const pluginDatahandler *dh = this->getDatahandlerFunctions (type);

			// Check if this plugin supports the requested data handler type.
			if (dh->table_read_class == nullptr) return nullptr;

			// Run the data handler creator function to get and instance of te data handler class.
			dhr = dh->table_read_class (dh->instance, std::dynamic_pointer_cast<libgpsfile2::provider::ProviderTableReaderBase>(dpr), path);
		}
		return dhr;
}

BaseDatahandlerWriterPlugin *GpsfilePlugin::createWriter (std::shared_ptr<libgpsfile2::provider::ProviderWriterBase> dpw, const DatahandlerType& dht, const std::string& path, const std::string& type) {
	// Variable to store the data handler in.
	BaseDatahandlerWriterPlugin *dhw = nullptr;

	if (dht == PLUGIN_TYPE_DATA_FILE_NONE) dhw = new libgpsfile2::DatahandlerPlainWriter (dpw, path);
		else if (dht == PLUGIN_TYPE_DATA_FILE_WRITE) {
			// Get the struct that contains all the data handler creator functions.
			const pluginDatahandler *dh = this->getDatahandlerFunctions (type);

			// Check if this plugin supports the requested data handler type.
			if (dh->route_write_class == nullptr) return nullptr;

			// Run the data handler creator function to get and instance of te data handler class.
			dhw = dh->route_write_class (dh->instance, std::dynamic_pointer_cast<libgpsfile2::provider::ProviderRouteWriterBase>(dpw), path);
		};
	return dhw;
}

bool GpsfilePlugin::isExtentionMatch (const std::string& ext, const DatahandlerType& t) const {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %d)\n", __func__, ext, t);
	return this->isExtentionMatch (ext.c_str (), t);
}

bool GpsfilePlugin::isExtentionMatch (const char *ext, const DatahandlerType& t) const {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %d)\n", __func__, ext, t);
	char *c = const_cast<char *>(ext);

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return true.
	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (c) == 0 && IS_SET (e.handlers, t)) return true;
	}

	// No match was found.
	return false;
}

void GpsfilePlugin::registerPlugin (const char *code, const pluginDetails& details, void *data) {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %p, %p)\n", __func__, code, details, data);
	GpsfilePlugin::addDataFileEntry (std::string (code), details, static_cast<const pluginDatahandler *>(data));
	return;
}

void GpsfilePlugin::removePlugin (const char *code) {
	DEBUG_MSG("GpsfilePlugin::%s (%s)\n", __func__, code);
	GpsfilePlugin::removeDataFileEntry (std::string (code));
	return;
}

const std::vector<std::string> GpsfilePlugin::getDataFileExtentions (const DatahandlerType& t) {
	DEBUG_MSG("GpsfilePlugin::%s (%d)\n", __func__, t);
	std::vector<std::string> v;
	v.reserve (GpsfilePlugin::data_file_table.size ());

	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if ((t == PLUGIN_TYPE_DATA_FILE_READ && e.data.route_read_class != nullptr) ||
			(t == PLUGIN_TYPE_DATA_FILE_WRITE && e.data.route_write_class != nullptr) ||
			(t == PLUGIN_TYPE_DATA_FILE_WAYPOINT && e.data.waypoint_read_class != nullptr) ||
			(t == PLUGIN_TYPE_DATA_FILE_TABLE && e.data.table_read_class != nullptr)) {
			v.push_back(e.code);
		}
	}

	v.shrink_to_fit ();

	return v;
}

const pluginDatahandler *GpsfilePlugin::getDatahandlerFunctions (const std::string& ext) {
	DEBUG_MSG("GpsfilePlugin::%s (%s)\n", __func__, ext);
	char *c = const_cast<char *>(ext.c_str ());

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return adress of data struct.
	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (c) == 0) {
			DEBUG_MSG("found at match with %s.\n", e.details.name);
			return &(e.data);
		}
	}
	INFO_MSG("No data handler found for data type %s.\n", c);

	return nullptr;
}

bool GpsfilePlugin::addDataFileEntry (const std::string& c, const pluginDetails& d, const pluginDatahandler *f) {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %p, %p)\n", __func__, c, d, f);
	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (c) == 0) return false;
	}

	unsigned int h = PLUGIN_TYPE_DATA_FILE_NONE;

	if (f->route_read_class != nullptr) h |= PLUGIN_TYPE_DATA_FILE_READ;
	if (f->route_write_class != nullptr) h |= PLUGIN_TYPE_DATA_FILE_WRITE;
	if (f->waypoint_read_class != nullptr) h |= PLUGIN_TYPE_DATA_FILE_WAYPOINT;
	if (f->table_read_class != nullptr) h |= PLUGIN_TYPE_DATA_FILE_TABLE;

	const pluginDataFileEntry e = {c, static_cast<DatahandlerType>(h), d, *f};

	GpsfilePlugin::data_file_table.push_back (e);

	return true;
}

bool GpsfilePlugin::removeDataFileEntry (const std::string& code) {
	DEBUG_MSG("GpsfilePlugin::%s (%s)\n", __func__, code);
	if (code.empty ()) return false;
	for (auto it = GpsfilePlugin::data_file_table.cbegin (); it != GpsfilePlugin::data_file_table.cend (); ++it) {
		const pluginDataFileEntry& e = *it;
		if (e.code.compare (code) == 0) {
			GpsfilePlugin::data_file_table.erase (it);
			return true;
		}
	}
	return false;
}

void GpsfilePlugin::loadPlugins (void) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	const char plugin_dir[] = PKGLIBDIR;
	struct dirent **entry;
	int j, i;
	if((j = scandir (plugin_dir, &entry, GpsfilePlugin::loadPluginFilter, nullptr)) != -1)
	{
		for(i = 0; i < j; i++)
		{
			std::string file = std::string(plugin_dir) + "/" + entry[i]->d_name;
			INFO_MSG("try to load module: %s\n", file.c_str ());
			void *plugin = dlopen(file.c_str (), RTLD_NOW);
			if (!plugin)
			{
				WARNING_MSG("Cannot load module %s: %s\n", file, dlerror());
			}
		}
	}
}

int GpsfilePlugin::loadPluginFilter (const struct dirent *file) {
	DEBUG_MSG("GpsfilePlugin::%s (%p)\n", __func__, file);
	if ((file->d_type == DT_REG) && (strcmp (file->d_name + strlen (file->d_name) - 3, ".so") == 0)) return 1;
	return 0;
}
