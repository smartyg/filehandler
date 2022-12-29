#ifndef _FILEHANDLER_PLUGINHELPER_
#define _FILEHANDLER_PLUGINHELPER_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <pluginframework/types/PluginDetails.h>

#include <filehandler/FileHandlerManager.hpp>
#include <filehandler/types/PluginDetails.hpp>
#include <filehandler/types/HandlerCreatorFunc.hpp>
#include <filehandler/types/PluginCreatorFunc.hpp>
#include <filehandler/traits/ProviderReaderTrait.hpp>
#include <filehandler/traits/ProviderWriterTrait.hpp>
#include <filehandler/traits/HandlerReaderTrait.hpp>
#include <filehandler/traits/HandlerWriterTrait.hpp>
#include <filehandler/utils/dynamic_unique_ptr_cast.hpp>

namespace filehandler {
	class PluginHelper final {
		const char* _file_type;
		const char* _name;
		const char* _author;
		const char* _license;
		const uint8_t _major_version;
		const uint8_t _minor_version;
		std::vector<PluginCreatorFunc> _handlers;
		std::weak_ptr<PluginHelper> _self;

		PluginHelper (void)                               = delete;
		PluginHelper (const PluginHelper&)                = delete; // copy constructor
		PluginHelper (PluginHelper&&) noexcept            = delete; // move constructor
		PluginHelper& operator= (const PluginHelper&)     = delete; // copy assignment
		PluginHelper& operator= (PluginHelper&&) noexcept = delete; // move assignment

		PluginHelper (const char* file_type, const char* name, const char* author, const char* license, const uint8_t& major_version, const uint8_t& minor_version) : _file_type (file_type), _name (name), _author (author), _license (license), _major_version (major_version), _minor_version (minor_version) {
			this->_handlers.reserve (8);
		}

	public:
		~PluginHelper (void) = default;

		static void deleteData (::PluginDetails& details) {
			types::PluginDetails* data = static_cast<types::PluginDetails*>(details.data);
			details.data = nullptr;
			delete data;
		}

		[[nodiscard]] static const std::shared_ptr<PluginHelper> constructPlugin (const char* file_type, const char* name, const char* author, const char* license, const uint8_t& major_version, const uint8_t& minor_version) {
			const std::shared_ptr<PluginHelper> helper = std::shared_ptr<PluginHelper>(new PluginHelper (file_type, name, author, license, major_version, minor_version));
			helper->_self = helper;

			return helper;
		}

		template <provider::ProviderReaderAbstractTrait Provider, handler::HandlerReaderFinalTrait Reader>
		[[nodiscard]] const std::shared_ptr<PluginHelper> addReader (void) {
			PluginCreatorFunc func = [extension = this->_file_type] (const std::shared_ptr<FileHandlerManager>& manager) -> void {
				HandlerCreatorFunc f1 = [] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
					std::unique_ptr<Provider> provider = utils::dynamic_unique_ptr_cast<Provider>(std::move (provider_base));
					std::unique_ptr<handler::HandlerBase> reader = std::make_unique<Reader>(std::move (provider), path);
					return reader;
				};

				manager->addPlugin (extension, FileHandlerManager::getProviderType<Provider>(), f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <provider::ProviderWriterAbstractTrait Provider, handler::HandlerWriterFinalTrait Writer>
		[[nodiscard]] const std::shared_ptr<PluginHelper> addWriter (void) {
			PluginCreatorFunc func = [extension = this->_file_type] (const std::shared_ptr<FileHandlerManager>& manager) -> void {
				HandlerCreatorFunc f1 = [] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
					std::unique_ptr<Provider> provider = utils::dynamic_unique_ptr_cast<Provider>(std::move (provider_base));
					std::unique_ptr<handler::HandlerBase> writer = std::make_unique<Writer>(std::move (provider), path);
					return writer;
				};

				manager->addPlugin (extension, FileHandlerManager::getProviderType<Provider>(), f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <provider::ProviderReaderAbstractTrait Provider, handler::HandlerReaderFinalTrait Reader, class Base>
		[[nodiscard]] const std::shared_ptr<PluginHelper> addReader (std::shared_ptr<Base> base) {
			PluginCreatorFunc func = [base, extension = this->_file_type] (const std::shared_ptr<FileHandlerManager>& manager) -> void {
				HandlerCreatorFunc f1 = [base] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
					std::unique_ptr<Provider> provider = utils::dynamic_unique_ptr_cast<Provider>(std::move (provider_base));
					std::unique_ptr<handler::HandlerBase> reader = std::make_unique<Reader>(base, std::move (provider), path);
					return reader;
				};

				manager->addPlugin (extension, FileHandlerManager::getProviderType<Provider>(), f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <provider::ProviderWriterAbstractTrait Provider, handler::HandlerWriterFinalTrait Writer, class Base>
		[[nodiscard]] const std::shared_ptr<PluginHelper> addWriter (const std::shared_ptr<Base> base) {
			PluginCreatorFunc func = [base, extension = this->_file_type] (const std::shared_ptr<FileHandlerManager>& manager) -> void {
				HandlerCreatorFunc f1 = [base] (std::unique_ptr<provider::ProviderBase> provider_base, const std::string& path) -> std::unique_ptr<handler::HandlerBase> {
					std::unique_ptr<Provider> provider = utils::dynamic_unique_ptr_cast<Provider>(std::move (provider_base));
					std::unique_ptr<handler::HandlerBase> writer = std::make_unique<Writer>(base, std::move (provider), path);
					return writer;
				};

				manager->addPlugin (extension, FileHandlerManager::getProviderType<Provider>(), f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		void construct (::PluginDetails& details) const {
			details.id = this->_file_type;
			details.name = this->_name;
			details.author = this->_author;
			details.license = this->_license;
			details.major_version = this->_major_version;
			details.minor_version = this->_minor_version;

			details.data = static_cast<void*>(new types::PluginDetails (this->_handlers));
		}
	};
}

#endif /* _FILEHANDLER_PLUGINHELPER_ */