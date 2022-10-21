#include "config.h"
#include <features.h>

#include "libgpsfile2/GpsfileManager.hpp"

#include <vector>
#include <list>
#include <string>
#include <utility>
#include <stdexcept>
#include <gpsdata/utils/Logger.hpp>

#include "./handler/HandlerPlainReader.hpp"
#include "./handler/HandlerPlainWriter.hpp"

using libgpsfile2::GpsfileManager;
using libgpsfile2::types::PluginDetails;
using libgpsfile2::types::HandlerCreatorFunc;
using libgpsfile2::types::PluginCreatorFunc;

GpsfileManager::GpsfileManager (void) {
	DEBUG_MSG("GpsfileManager::%s ()\n", __func__);

	HandlerCreatorFunc plain_reader = [] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
		std::unique_ptr<provider::ProviderReaderBase> provider = utils::dynamic_unique_ptr_cast<provider::ProviderReaderBase>(std::move (provider_base));
		std::unique_ptr<handler::HandlerBase> reader = std::make_unique<handler::HandlerPlainReader>(std::move (provider), path);
		return reader;
	};
	HandlerCreatorFunc plain_writer = [] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
		std::unique_ptr<provider::ProviderWriterBase> provider = utils::dynamic_unique_ptr_cast<provider::ProviderWriterBase>(std::move (provider_base));
		std::unique_ptr<handler::HandlerBase> writer = std::make_unique<handler::HandlerPlainWriter>(std::move (provider), path);
		return writer;
	};

	this->addPlugin ({}, GpsfileManager::getProviderType<provider::ProviderReaderBase>(), plain_reader);
	this->addPlugin ({}, GpsfileManager::getProviderType<provider::ProviderWriterBase>(), plain_writer);
}

GpsfileManager::~GpsfileManager (void) {
	DEBUG_MSG("GpsfileManager::%s ()\n", __func__);
}

const std::shared_ptr<GpsfileManager> GpsfileManager::getPtr (void) {
	DEBUG_MSG("GpsfileManager::%s ()\n", __func__);
	static std::shared_ptr<GpsfileManager> instance = std::shared_ptr<GpsfileManager>(new GpsfileManager ());

	return instance;
}

void GpsfileManager::addPlugin (const std::string& extension, const std::size_t& provider_type, HandlerCreatorFunc func) {
	DEBUG_MSG("GpsfileManager::%s (%s, %ld, %p)\n", __func__, extension.c_str (), provider_type, func);
	GpsfileManager::KeyType key = std::make_pair (extension, provider_type);
	this->_handler_map.insert (std::make_pair (key, func));
	this->_provider_type_extension_map[provider_type].push_back (extension);
	this->_extension_provider_type_map[extension].push_back (provider_type);
}

void GpsfileManager::registar (const char* id, void* data_pointer) {
	DEBUG_MSG("GpsfileManager::%s (%s, %p)\n", __func__, id, data_pointer);
	(void)id;

	const types::PluginDetails* ptr = static_cast<types::PluginDetails*>(data_pointer);
	for (const PluginCreatorFunc& func : ptr->handlers) {
		func (GpsfileManager::getPtr ());
	}
}
void GpsfileManager::deregistar (const char* id) {
	DEBUG_MSG("GpsfileManager::%s (%s)\n", __func__, id);
	(void)id;
	/*
	 * try {
	 *	const std::string extension (id);
	 *	const auto handlers = this->_extension_provider_type_map.at (extension);
	 *	for (const auto& provider_type : handlers) {
	 *		const KeyType key = std::make_pair (extension, provider_type);
	 *		this->_handler_map.erase (key);
}
this->_extension_provider_type_map.erase (extension);*/
}
