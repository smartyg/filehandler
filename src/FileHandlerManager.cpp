#include "config.h"

#include "filehandler/FileHandlerManager.hpp"

#include <vector>
#include <list>
#include <string>
#include <utility>
#include <stdexcept>
#include <Logger.hpp>

#include "./handler/HandlerPlainReader.hpp"
#include "./handler/HandlerPlainWriter.hpp"

using filehandler::FileHandlerManager;
using filehandler::types::PluginDetails;
using filehandler::types::HandlerCreatorFunc;
using filehandler::types::PluginCreatorFunc;

FileHandlerManager::FileHandlerManager (void) {
	DEBUG_MSG ("FileHandlerManager::{:s} ()\n", __func__);

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

	this->addPlugin ({}, FileHandlerManager::getProviderType<provider::ProviderReaderBase>(), plain_reader);
	this->addPlugin ({}, FileHandlerManager::getProviderType<provider::ProviderWriterBase>(), plain_writer);
}

FileHandlerManager::~FileHandlerManager (void) {
	DEBUG_MSG ("FileHandlerManager::{:s} ()\n", __func__);
	this->_provider_type_extension_map.clear ();
	this->_extension_provider_type_map.clear ();
	this->_handler_map.clear ();
}

const std::shared_ptr<FileHandlerManager> FileHandlerManager::getPtr (void) {
	DEBUG_MSG ("FileHandlerManager::{:s} ()\n", __func__);
	static std::shared_ptr<FileHandlerManager> instance = std::shared_ptr<FileHandlerManager>(new FileHandlerManager ());

	return instance;
}

void FileHandlerManager::print (void) const {
	DEBUG_MSG ("FileHandlerManager::{:s} ()\n", __func__);

	cpplogger::Logger::get () (cpplogger::Level::NOTICE, "Content of extension-provider-map:\n");
	for (const auto& [ext, types] : this->_extension_provider_type_map) {
		std::string ext_str;
		if (ext.empty ()) ext_str = "PlainHandler";
		else ext_str = ext;
		NOTICE_MSG ("  {:s}:\n", ext_str);
		for (const std::size_t& type : types) {
			const KeyType key = std::make_pair (ext, type);
			const HandlerCreatorFunc& func = this->_handler_map.at (key);
			NOTICE_MSG ("  - {:d}: {:p}\n", type, fmt::ptr (&func));
		}
	}

	cpplogger::Logger::get () (cpplogger::Level::NOTICE, "Content of provider-extension-map:\n");
	for (const auto& [type, exts] : this->_provider_type_extension_map) {
		NOTICE_MSG ("  {:d}:\n", type);
		for (const std::string& ext : exts) {
			const KeyType key = std::make_pair (ext, type);
			const HandlerCreatorFunc& func = this->_handler_map.at (key);
			std::string ext_str;
			if (ext.empty ()) ext_str = "PlainHandler";
			else ext_str = ext;
			NOTICE_MSG ("  - {:s}: {:p}\n", ext_str, fmt::ptr (&func));
		}
	}
}

void FileHandlerManager::addPlugin (const std::string& extension, const std::size_t& provider_type, HandlerCreatorFunc func) {
	DEBUG_MSG ("FileHandlerManager::{:s} ({:s}, {:d}, {:p})\n", __func__, extension, provider_type, fmt::ptr (&func));
	FileHandlerManager::KeyType key = std::make_pair (extension, provider_type);
	this->_handler_map.insert (std::make_pair (key, func));
	this->_provider_type_extension_map[provider_type].push_back (extension);
	this->_extension_provider_type_map[extension].push_back (provider_type);
}

void FileHandlerManager::registar (const char* id, void* data_pointer) {
	DEBUG_MSG ("FileHandlerManager::{:s} ({:s}, {:p})\n", __func__, id, data_pointer);
	(void)id;

	const types::PluginDetails* ptr = static_cast<types::PluginDetails*>(data_pointer);
	for (const PluginCreatorFunc& func : ptr->handlers) {
		func (FileHandlerManager::getPtr ());
	}
}
void FileHandlerManager::deregistar (const char* id) {
	DEBUG_MSG ("FileHandlerManager::{:s} ({:s})\n", __func__, id);

	try {
		const std::string extension (id);
		const auto handlers = this->_extension_provider_type_map.at (extension);
		for (const auto& provider_type : handlers) {
			DEBUG_2_MSG (2, "type {:d} is (also) related to {:s}\n", provider_type, extension);
			const KeyType key = std::make_pair (extension, provider_type);
			try {
				DEBUG_2_MSG (2, "try to remove handler associated with key {{{:s}, {:d}}}\n", extension, provider_type);
				this->_handler_map.erase (key);
			} catch (std::exception& e) {
				EXCEPTION_INFO_MSG (e);
			}

			try {
				DEBUG_2_MSG (2, "try to remove {:s} from provider-type-map\n", extension);
				std::erase_if (this->_provider_type_extension_map.at (provider_type), [&extension] (const std::string& str) { return (!str.empty () && str.compare (extension)); });
			} catch (std::exception& e) {
				EXCEPTION_INFO_MSG (e);
			}
		}
		this->_extension_provider_type_map.erase (extension);
	} catch (std::exception& e) {
		EXCEPTION_INFO_MSG (e);
	}
}
