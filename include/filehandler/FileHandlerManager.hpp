#ifndef _FILEHANDLER_FILEHANDLERMANAGER_
#define _FILEHANDLER_FILEHANDLERMANAGER_

#include <memory>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <string_view>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <Logger.hpp>

#include <filehandler/types/HandlerCreatorFunc.hpp>
#include <filehandler/types/PluginCreatorFunc.hpp>
#include <filehandler/types/PluginDetails.hpp>
#include <filehandler/handler/HandlerReaderBase.hpp>
#include <filehandler/handler/HandlerWriterBase.hpp>
#include <filehandler/provider/ProviderReaderBase.hpp>
#include <filehandler/provider/ProviderWriterBase.hpp>
#include <filehandler/traits/ProviderReaderTrait.hpp>
#include <filehandler/traits/ProviderWriterTrait.hpp>
#include <filehandler/utils/dynamic_unique_ptr_cast.hpp>

namespace filehandler {

	class FileHandlerManager final {
		using KeyType = std::pair<const std::string, const std::size_t>;

		std::map<const std::size_t, std::vector<std::string>> _provider_type_extension_map;
		std::map<const std::string, std::vector<std::size_t>> _extension_provider_type_map;
		std::map<KeyType, HandlerCreatorFunc> _handler_map;

		FileHandlerManager (void);
		FileHandlerManager (const FileHandlerManager&) = delete;                // copy constructor
		FileHandlerManager (FileHandlerManager&&) noexcept = delete;            // move constructor
		FileHandlerManager& operator= (const FileHandlerManager&) = delete;     // copy assignment
		FileHandlerManager& operator= (FileHandlerManager&&) noexcept = delete; // move assignment

	public:
		constexpr static const std::string_view TAG = "FileHandlerManager";

		~FileHandlerManager (void);

		template <provider::ProviderReaderAbstractTrait ProviderBaseType, provider::ProviderReaderFinalTrait Provider>
		[[nodiscard]] std::unique_ptr<handler::HandlerReaderBase> createReader (std::unique_ptr<Provider> provider, const std::string& path, const std::string& extension) const {
			static_assert (std::is_base_of<ProviderBaseType, Provider>::value);
			DEBUG_MSG ("FileHandlerManager::{:s} ({:p}, {:s}, {:s})\n", __func__, fmt::ptr (provider), path, extension);
			KeyType key = std::make_pair (extension, FileHandlerManager::getProviderType<ProviderBaseType>());
			DEBUG_2_MSG (1, "FileHandlerManager::{:s}: key {{{:s}, {:d}}}\n", __func__, key.first, key.second);
			try {
				HandlerCreatorFunc func = this->_handler_map.at (key);
				std::unique_ptr<handler::HandlerBase> handler_base = func (std::move (provider), path);
				return utils::dynamic_unique_ptr_cast<handler::HandlerReaderBase>(std::move (handler_base));
			} catch (std::exception& e) {
				EXCEPTION_INFO_MSG (e);
			}
			return {};
		}

		template <provider::ProviderWriterAbstractTrait ProviderBaseType, provider::ProviderWriterFinalTrait Provider>
		[[nodiscard]] std::unique_ptr<handler::HandlerWriterBase> createWriter (std::unique_ptr<Provider> provider, const std::string& path, const std::string& extension) const {
			static_assert (std::is_base_of<ProviderBaseType, Provider>::value);
			DEBUG_MSG ("FileHandlerManager::{:s} ({:p}, {:s}, {:s})\n", __func__, fmt::ptr (provider), path, extension);
			KeyType key = std::make_pair (extension, FileHandlerManager::getProviderType<ProviderBaseType>());
			DEBUG_2_MSG (1, "FileHandlerManager::{:s}: key {{{:s}, {:d}}}\n", __func__, key.first, key.second);
			try {
				HandlerCreatorFunc func = this->_handler_map.at (key);
				std::unique_ptr<handler::HandlerBase> handler_base = func (std::move (provider), path);
				return utils::dynamic_unique_ptr_cast<handler::HandlerWriterBase>(std::move (handler_base));
			} catch (std::exception& e) {
				EXCEPTION_INFO_MSG (e);
			}
			return {};
		}

		void print (void) const;

		void addPlugin (const std::string& extension, const std::size_t& provider_type, HandlerCreatorFunc func);
		void registar (const char* id, void* data_pointer);
		void deregistar (const char* id);

		[[nodiscard]] static const std::shared_ptr<FileHandlerManager> getPtr (void);

		template <class ProviderBase>
		inline constexpr static std::size_t getProviderType (void) {
			static_assert (std::is_abstract<ProviderBase>::value && (std::is_base_of<provider::ProviderReaderBase, ProviderBase>::value || std::is_base_of<provider::ProviderWriterBase, ProviderBase>::value));
			return typeid (ProviderBase).hash_code ();
		}
	};
}

#endif /* _FILEHANDLER_FILEHANDLERMANAGER_ */