#include "config.h"
#include <features.h>

#include "libgpsfile2/GpsfileManager.hpp"

#include <vector>
#include <list>
#include <string>
#include <utility>
#include <stdexcept>
#include <gpsdata/utils/Logger.hpp>

#include <libgpsfile2/PluginHandler.hpp>
#include <libgpsfile2/types/HandlerType.hpp>
#include <libgpsfile2/types/PluginDetails.hpp>
#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>

#include "./handler/HandlerPlainReader.hpp"
#include "./handler/HandlerPlainWriter.hpp"

#ifndef IS_SET
#define IS_SET(v, m) (((v) & (m)) == (m))
#endif

#define PLUGIN_TYPE_DATA_FILE_NONE (0)


using libgpsfile2::GpsfileManager;
using libgpsfile2::HandlerType;
using libgpsfile2::PluginDetails;
using libgpsfile2::PluginHandler;
using libgpsfile2::handler::HandlerReaderBase;
using libgpsfile2::handler::HandlerWriterBase;
using libgpsfile2::handler::HandlerPlainReader;
using libgpsfile2::handler::HandlerPlainWriter;
using libgpsfile2::provider::ProviderReaderBase;
using libgpsfile2::provider::ProviderWriterBase;

GpsfileManager::GpsfileManager (void) {
	DEBUG_MSG("GpsfileManager::%s ()\n", __func__);

	this->_writer_type_map.reserve (32);
	this->_reader_type_map.reserve (32);
	this->_writer_type_map.insert (this->_writer_type_map.begin (), 0);
	this->_reader_type_map.insert (this->_reader_type_map.begin (), 0);
}

GpsfileManager::~GpsfileManager (void) {
	DEBUG_MSG("GpsfileManager::%s ()\n", __func__);

	this->_data_file_table.clear ();
	this->_writer_type_map.clear ();
	this->_reader_type_map.clear ();
}

std::unique_ptr<HandlerReaderBase> GpsfileManager::createReader (std::unique_ptr<ProviderReaderBase> provider, const HandlerType& dht, const std::string& path, const std::string& type) {
	DEBUG_MSG("GpsfileManager::%s (..., %d, %s, %s)\n", __func__, static_cast<int>(dht), path.c_str (), type.c_str ());

	assert (dht);
	assert (this->_reader_type_map[static_cast<int>(dht)] == dht.getHash ());
	if (!dht) throw std::runtime_error ("dht is empty");

	// Select the correct data handler class.
	if (static_cast<int>(dht) == 0) return std::make_unique<HandlerPlainReader>(std::move (provider), path);

	// Get the struct that contains all the data handler creator functions.
	const std::shared_ptr<PluginHandler> dh = this->getDatahandlerFunctions (type);

	return dh->createReader (this->_reader_type_map[static_cast<int>(dht)], std::move (provider), dht, path);
}

std::unique_ptr<HandlerWriterBase> GpsfileManager::createWriter (std::unique_ptr<ProviderWriterBase> provider, const HandlerType& dht, const std::string& path, const std::string& type) {
	DEBUG_MSG("GpsfileManager::%s (..., %d, %s, %s)\n", __func__, static_cast<int>(dht), path.c_str (), type.c_str ());

	assert (dht);
	assert (this->_writer_type_map[static_cast<int>(dht)] == dht.getHash ());
	if (!dht) throw std::runtime_error ("dht is empty");

	// Select the correct data handler class.
	if (static_cast<int>(dht) == 0) return std::make_unique<HandlerPlainWriter>(std::move (provider), path);

	// Get the struct that contains all the data handler creator functions.
	const std::shared_ptr<PluginHandler> dh = this->getDatahandlerFunctions (type);

	return dh->createWriter (this->_writer_type_map[static_cast<int>(dht)], std::move (provider), dht, path);
}

bool GpsfileManager::isExtentionMatch (const std::string& ext, const HandlerType& t) const {
	DEBUG_MSG("GpsfileManager::%s (%s, %d)\n", __func__, ext.c_str (), static_cast<int>(t));
	return this->isExtentionMatch (ext.c_str (), t);
}

bool GpsfileManager::isExtentionMatch (const char *ext, const HandlerType& t) const {
	DEBUG_MSG("GpsfileManager::%s (%s, %d)\n", __func__, ext, static_cast<int>(t));
	char *c = const_cast<char *>(ext);

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return true.
	for (const pluginDataFileEntry& e : this->_data_file_table) {
		if (e.code.compare (c) == 0 && IS_SET (e.handler_types, static_cast<int>(t))) return true;
	}

	// No match was found.
	return false;
}
/*
void GpsfileManager::registerPlugin (const std::string code, const std::shared_ptr<PluginDetails> details, const std::shared_ptr<PluginHandler> handlers) {
	DEBUG_MSG("GpsfileManager::%s (%s, %p, %p)\n", __func__, code.c_str (), details.get (), handlers.get ());
	//if (GpsfileManager::instance_counter == 0) throw std::runtime_error ("Can not register a plugin before the GpsfilePlugin class is initialized");

	for (const pluginDataFileEntry& e : this->_data_file_table) {
		if (e.code.compare (code) == 0) return;
	}

	int h = 0;

	for (const auto& [key, value] : handlers->_reader_creators) {
		(void)value;
		h |= (1 << key);
	}

	for (const auto& [key, value] : handlers->_writer_creators) {
		(void)value;
		h |= (1 << key);
	}

	const pluginDataFileEntry e = {code, h, details, handlers};

	this->_data_file_table.push_back (e);

	return;
}

void GpsfileManager::removePlugin (const std::string code) {
	DEBUG_MSG("GpsfileManager::%s (%s)\n", __func__, code.c_str ());

	if (code.empty ()) return;
	for (auto it = this->_data_file_table.cbegin (); it != this->_data_file_table.cend (); ++it) {
		const pluginDataFileEntry& e = *it;
		if (e.code.compare (code) == 0) {
			this->_data_file_table.erase (it);
			return;
		}
	}
	return;
}*/

const std::vector<std::string> GpsfileManager::getDataFileExtentions (const HandlerType& t) {
	DEBUG_MSG("GpsfileManager::%s (%d)\n", __func__, t);
	std::vector<std::string> v;
	v.reserve (this->_data_file_table.size ());

	for (const pluginDataFileEntry& e : this->_data_file_table)
		if (e.handlers->_reader_creators.size () > 0 || e.handlers->_reader_creators.size () > 0) v.push_back (e.code);

	v.shrink_to_fit ();

	return v;
}

const std::shared_ptr<PluginHandler> GpsfileManager::getDatahandlerFunctions (const std::string& ext) {
	DEBUG_MSG("GpsfileManager::%s (%s)\n", __func__, ext.c_str ());
	char *c = const_cast<char *>(ext.c_str ());

	// If *ext starts with a '.', increase the pointer to exclusive the dot from the match.
	while (c[0] == '.') c++;

	// Loop over all data file handlers and check the type and code, if a match is found, return adress of data struct.
	for (const pluginDataFileEntry& e : this->_data_file_table) {
		if (e.code.compare (c) == 0) {
			DEBUG_MSG("found at match with %s.\n", e.details->name);
			return e.handlers;
		}
	}
	INFO_MSG("No data handler found for data type %s.\n", c);

	return nullptr;
}

int32_t GpsfileManager::registerWriter (const std::size_t& hash) {
	DEBUG_MSG("GpsfileManager::%s (%ld)\n", __func__, hash);

	int32_t num = static_cast<int32_t>(this->_writer_type_map.size ());

	if (num > 31) throw std::runtime_error ("out of range");

	this->_writer_type_map.insert (this->_writer_type_map.end (), hash);
	//DEBUG_MSG("registered writer function with id: %d and hash: %ld\n", num, hash);
	//DEBUG_MSG("size of _writer_type_map: %ld\n", GpsfileManager::_writer_type_map.size ());
	//DEBUG_MSG("capacity of _writer_type_map: %ld\n", GpsfileManager::_writer_type_map.capacity ());
	assert (this->_writer_type_map[num] == hash);
	return num;
}

int32_t GpsfileManager::registerReader (const std::size_t& hash) {
	DEBUG_MSG("GpsfileManager::%s (%ld)\n", __func__, hash);

	int32_t num = static_cast<int32_t>(this->_reader_type_map.size ());
	//if (num == 0) num++;
	if (num > 31) throw std::runtime_error ("out of range");
	this->_reader_type_map.insert (this->_reader_type_map.end (), hash);
	//DEBUG_MSG("registered reader function with id: %d and hash: %ld\n", num, hash);
	//DEBUG_MSG("size of _reader_type_map: %ld\n", GpsfileManager::_reader_type_map.size ());
	//DEBUG_MSG("capacity of _reader_type_map: %ld\n", GpsfileManager::_reader_type_map.capacity ());
	assert (this->_reader_type_map[num] == hash);
	return num;
}
