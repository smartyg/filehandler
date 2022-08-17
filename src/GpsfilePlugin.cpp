#include "config.h"

#include "libgpsfile2/GpsfilePlugin.hpp"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <dirent.h>
#include <dlfcn.h>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"

#include "DatahandlerPlain.hpp"

#ifndef IS_SET
#define IS_SET(v, m) (((v) & (m)) == (m))
#endif

#define PLUGIN_TYPE_DATA_FILE_NONE (0)

using libgpsfile2::GpsfilePlugin;
using libgpsfile2::HandlerReaderBase;
using libgpsfile2::HandlerWriterBase;
using libgpsfile2::pluginDatahandler;

std::map<std::size_t, GpsfilePlugin::WriterConvFunc> GpsfilePlugin::_writer_func_map;
std::map<std::size_t, GpsfilePlugin::ReaderConvFunc> GpsfilePlugin::_reader_func_map;
std::vector<std::size_t> GpsfilePlugin::_writer_type_map;
std::vector<std::size_t> GpsfilePlugin::_reader_type_map;
std::vector<GpsfilePlugin::pluginDataFileEntry> GpsfilePlugin::data_file_table;

int GpsfilePlugin::instance_counter = 0;

GpsfilePlugin::GpsfilePlugin (const std::list<std::string>& plugin_directories) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	/* TODO */
	// get exclusive lock (mutex)
	this->instance_counter++;
	GpsfilePlugin::_writer_type_map.reserve (32);
	GpsfilePlugin::_reader_type_map.reserve (32);
	GpsfilePlugin::_writer_type_map.insert (GpsfilePlugin::_writer_type_map.begin (), 0);
	GpsfilePlugin::_reader_type_map.insert (GpsfilePlugin::_reader_type_map.begin (), 0);
	this->loadPlugins (plugin_directories);
	// release exclusive lock
}

GpsfilePlugin::~GpsfilePlugin (void) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	/* TODO */
	// get exclusive lock (mutex)
	this->instance_counter--;
	if (this->instance_counter == 0) {
		GpsfilePlugin::data_file_table.clear ();
		GpsfilePlugin::_writer_func_map.clear ();
		GpsfilePlugin::_reader_func_map.clear ();
		GpsfilePlugin::_writer_type_map.clear ();
		GpsfilePlugin::_reader_type_map.clear ();
	}
	// release exclusive lock
}

GpsfilePlugin::WriterConvFunc GpsfilePlugin::getWriterFunc (const DatahandlerType& dht) {
	DEBUG_MSG("GpsfilePlugin::%s (%d)\n", __func__, static_cast<int>(dht));
	try {
		//DEBUG_MSG("looking for type id: %ld\n", type_hash);
		std::size_t type_hash = GpsfilePlugin::_writer_type_map.at (static_cast<int32_t>(dht));
		DEBUG_MSG("looking for type hash: %ld\n", type_hash);
		return GpsfilePlugin::_writer_func_map.at (type_hash);
	} catch (const std::out_of_range& e) {
		return nullptr;
	}
}

GpsfilePlugin::ReaderConvFunc GpsfilePlugin::getReaderFunc (const DatahandlerType& dht) {
	DEBUG_MSG("GpsfilePlugin::%s (%d)\n", __func__, static_cast<int>(dht));
	try {
		std::size_t type_hash = GpsfilePlugin::_reader_type_map.at (static_cast<int32_t>(dht));
		DEBUG_MSG("looking for type hash: %ld\n", type_hash);
		return GpsfilePlugin::_reader_func_map.at (type_hash);
	} catch (const std::out_of_range& e) {
		return nullptr;
	}
}

std::unique_ptr<HandlerReaderBase> GpsfilePlugin::createReader (std::unique_ptr<provider::ProviderReaderBase> provider, const DatahandlerType& dht, const std::string& path, const std::string& type) {
	DEBUG_MSG("GpsfilePlugin::%s (..., %d, %s, %s)\n", __func__, static_cast<int>(dht), path.c_str (), type.c_str ());

	if (!dht) throw std::runtime_error ("dht is empty");
	// Select the correct data handler class.
	if (static_cast<int32_t>(dht) == 0) return std::make_unique<HandlerPlainReader>(std::move (provider), path);
/*
		// Get the struct that contains all the data handler creator functions.
		const std::shared_ptr<pluginDatahandler> dh = this->getDatahandlerFunctions (type);
		auto it = dh->reader_creators.find (dht);
		if (it == dh->reader_creators.end ()) return nullptr;

		return (*it).second (dh->instance, std::move (dpr), path);*/

	// Get the struct that contains all the data handler creator functions.
	const std::shared_ptr<pluginDatahandler> dh = GpsfilePlugin::getDatahandlerFunctions (type);

	// Lookup the creator function for this type. If none exists, return empty poiner
	ReaderConvFunc func = GpsfilePlugin::getReaderFunc (dht);
	if (func == nullptr) return std::unique_ptr<HandlerReaderBase>();

	// Run the creator and return the handler
	return func (dh, std::move (provider), dht, path);
}

std::unique_ptr<HandlerWriterBase> GpsfilePlugin::createWriter (std::unique_ptr<provider::ProviderWriterBase> provider, const DatahandlerType& dht, const std::string& path, const std::string& type) {
	DEBUG_MSG("GpsfilePlugin::%s (..., %d, %s, %s)\n", __func__, static_cast<int>(dht), path.c_str (), type.c_str ());

	if (!dht) throw std::runtime_error ("dht is empty");
	if (static_cast<int32_t>(dht) == 0) return std::make_unique<HandlerPlainWriter>(std::move (provider), path);
/*
	// Get the struct that contains all the data handler creator functions.
	const std::shared_ptr<pluginDatahandler> dh = this->getDatahandlerFunctions (type);
	auto it = dh->writer_creators.find (dht);
	if (it == dh->writer_creators.end ()) return nullptr;

	return (*it).second (dh->instance, std::move (dpw), path);*/

	// Get the struct that contains all the data handler creator functions.
	const std::shared_ptr<pluginDatahandler> dh = GpsfilePlugin::getDatahandlerFunctions (type);

	WriterConvFunc func = GpsfilePlugin::getWriterFunc (dht);
	if (func == nullptr) return std::unique_ptr<HandlerWriterBase>();
	return func (dh, std::move (provider), dht, path);
}

bool GpsfilePlugin::isExtentionMatch (const std::string& ext, const DatahandlerType& t) const {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %d)\n", __func__, ext.c_str (), static_cast<int>(t));
	return this->isExtentionMatch (ext.c_str (), t);
}

bool GpsfilePlugin::isExtentionMatch (const char *ext, const DatahandlerType& t) const {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %d)\n", __func__, ext, static_cast<int>(t));
	char *c = const_cast<char *>(ext);

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return true.
	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (c) == 0 && IS_SET (e.handler_types, static_cast<int>(t))) return true;
	}

	// No match was found.
	return false;
}

void GpsfilePlugin::registerPlugin (const std::string code, const std::shared_ptr<pluginDetails> details, const std::shared_ptr<pluginDatahandler> handlers) {
	DEBUG_MSG("GpsfilePlugin::%s (%s, %p, %p)\n", __func__, code.c_str (), details.get (), handlers.get ());
	if (GpsfilePlugin::instance_counter == 0) throw std::runtime_error ("Can not register a plugin before the GpsfilePlugin class is initialized");

	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (code) == 0) return;
	}

	int h = 0;

	for (const auto& [key, value] : handlers->reader_creators) {
		(void)value;
		h |= (1 << key);
	}

	for (const auto& [key, value] : handlers->writer_creators) {
		(void)value;
		h |= (1 << key);
	}

	const pluginDataFileEntry e = {code, h, details, handlers};

	GpsfilePlugin::data_file_table.push_back (e);

	return;
}

void GpsfilePlugin::removePlugin (const std::string code) {
	DEBUG_MSG("GpsfilePlugin::%s (%s)\n", __func__, code);
	if (GpsfilePlugin::instance_counter == 0) throw std::runtime_error ("Can not remove a plugin before the GpsfilePlugin class is initialized");

	if (code.empty ()) return;
	for (auto it = GpsfilePlugin::data_file_table.cbegin (); it != GpsfilePlugin::data_file_table.cend (); ++it) {
		const pluginDataFileEntry& e = *it;
		if (e.code.compare (code) == 0) {
			GpsfilePlugin::data_file_table.erase (it);
			return;
		}
	}
	return;
}

const std::vector<std::string> GpsfilePlugin::getDataFileExtentions (const DatahandlerType& t) {
	DEBUG_MSG("GpsfilePlugin::%s (%d)\n", __func__, t);
	std::vector<std::string> v;
	v.reserve (GpsfilePlugin::data_file_table.size ());

	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table)
		if (e.handlers->reader_creators.size () > 0 || e.handlers->reader_creators.size () > 0) v.push_back (e.code);

	v.shrink_to_fit ();

	return v;
}

const std::shared_ptr<pluginDatahandler> GpsfilePlugin::getDatahandlerFunctions (const std::string& ext) {
	DEBUG_MSG("GpsfilePlugin::%s (%s)\n", __func__, ext.c_str ());
	char *c = const_cast<char *>(ext.c_str ());

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return adress of data struct.
	for (const pluginDataFileEntry& e : GpsfilePlugin::data_file_table) {
		if (e.code.compare (c) == 0) {
			DEBUG_MSG("found at match with %s.\n", e.details->name);
			return e.handlers;
		}
	}
	INFO_MSG("No data handler found for data type %s.\n", c);

	return nullptr;
}

int32_t GpsfilePlugin::registerWriter (const std::size_t& hash, WriterConvFunc func) {
	DEBUG_MSG("GpsfilePlugin::%s (%ld, ...)\n", __func__, hash);
	if (GpsfilePlugin::instance_counter == 0) throw std::runtime_error ("Can not register a type before the GpsfilePlugin class is initialized");

	GpsfilePlugin::_writer_func_map[hash] = func;
	int32_t num = static_cast<int32_t>(GpsfilePlugin::_writer_type_map.size ());
	//if (num == 0) num++;
	if (num > 31) throw std::runtime_error ("out of range");
	//GpsfilePlugin::_writer_type_map[num] = hash;
	GpsfilePlugin::_writer_type_map.insert (GpsfilePlugin::_writer_type_map.end (), hash);
	DEBUG_MSG("registered writer function with id: %d and hash: %ld\n", num, hash);
	DEBUG_MSG("size of _writer_type_map: %ld\n", GpsfilePlugin::_writer_type_map.size ());
	DEBUG_MSG("capacity of _writer_type_map: %ld\n", GpsfilePlugin::_writer_type_map.capacity ());
	assert (GpsfilePlugin::_writer_type_map[num] == hash);
	return num;
}

int32_t GpsfilePlugin::registerReader (const std::size_t& hash, ReaderConvFunc func) {
	DEBUG_MSG("GpsfilePlugin::%s (%ld, ...)\n", __func__, hash);
	if (GpsfilePlugin::instance_counter == 0) throw std::runtime_error ("Can not register a type before the GpsfilePlugin class is initialized");

	GpsfilePlugin::_reader_func_map[hash] = func;
	int32_t num = static_cast<int32_t>(GpsfilePlugin::_reader_type_map.size ());
	if (num == 0) num++;
	if (num > 31) throw std::runtime_error ("out of range");
	GpsfilePlugin::_reader_type_map.insert (GpsfilePlugin::_reader_type_map.end (), hash);
	DEBUG_MSG("registered reader function with id: %d and hash: %ld\n", num, hash);
	DEBUG_MSG("size of _reader_type_map: %ld\n", GpsfilePlugin::_reader_type_map.size ());
	DEBUG_MSG("capacity of _reader_type_map: %ld\n", GpsfilePlugin::_reader_type_map.capacity ());
	assert (GpsfilePlugin::_reader_type_map[num] == hash);
	return num;
}

void GpsfilePlugin::loadPlugins (const std::list<std::string>& plugin_directories, bool scan_default) {
	DEBUG_MSG("GpsfilePlugin::%s ()\n", __func__);
	const char plugin_dir[] = PKGLIBDIR;
	struct dirent **entry;
	int j, i;

	char *scan_dir;
	auto it = plugin_directories.cbegin ();
	if (scan_default || (plugin_directories.cbegin () == plugin_directories.cend ()))
		scan_dir = const_cast<char *>(plugin_dir);
	else {
		scan_dir = const_cast<char *>((*it).c_str ());
		++it;
	}
	do {
		DEBUG_MSG("scan directory: %s\n", scan_dir);
		if((j = scandir (scan_dir, &entry, GpsfilePlugin::loadPluginFilter, nullptr)) != -1) {
			for(i = 0; i < j; i++) {
				std::string file = std::string (scan_dir) + "/" + entry[i]->d_name;
				INFO_MSG("try to load module: %s\n", file.c_str ());
				void *plugin = dlopen(file.c_str (), RTLD_NOW);
				if (!plugin) {
					WARNING_MSG("Cannot load module %s: %s\n", file, dlerror());
				}
			}
		}
		if (it == plugin_directories.cend ()) break;
		scan_dir = const_cast<char *>((*it).c_str ());
		++it;
	} while (true);
}

int GpsfilePlugin::loadPluginFilter (const struct dirent *file) {
	DEBUG_MSG("GpsfilePlugin::%s (%p)\n", __func__, file);
	if ((file->d_type == DT_REG) && (strcmp (file->d_name + strlen (file->d_name) - 3, ".so") == 0)) return 1;
	return 0;
}
