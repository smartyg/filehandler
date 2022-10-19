#ifndef _LIBGPSFILE2_PLUGINHELPER_
#define _LIBGPSFILE2_PLUGINHELPER_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <cpluginmanager/types/PluginDetails.h>

#include "GpsfileManager.hpp"
#include "libgpsfile2/types/PluginDetails.hpp"
#include "libgpsfile2/types/Creator.hpp"
#include "libgpsfile2/PluginHandler.hpp"
#include "libgpsfile2/handler/HandlerReaderBase.hpp"
#include "libgpsfile2/handler/HandlerWriterBase.hpp"

namespace libgpsfile2 {
	class PluginHelper final {
		//using creator = std::function<void(const std::shared_ptr<GpsfileManager>&, const std::shared_ptr<PluginHandler>&)>;
		const char* _file_type;
		const char* _name;
		const char* _author;
		const char* _license;
		uint8_t _major_version;
		uint8_t _minor_version;
		std::vector<Creator> _handlers;
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

		static const std::shared_ptr<PluginHelper> constructPlugin (const char* file_type, const char* name, const char* author, const char* license, const uint8_t& major_version, const uint8_t& minor_version) {
			auto helper = std::shared_ptr<PluginHelper>(new PluginHelper (file_type, name, author, license, major_version, minor_version));
			helper->_self = helper;
/*
			helper._file_type = file_type;
			helper._name = name;
			helper._author = author;
			helper._license = license;
			helper._major_version = major_version;
			helper._minor_version = minor_version;
*/
			return helper;
		}

		template <class Provider, class Reader>
		const std::shared_ptr<PluginHelper> addReader (void) {
			Creator func = [] (const std::shared_ptr<GpsfileManager>& manager, const std::shared_ptr<PluginHandler>& handler) -> void {
				HandlerType reader_type;
				manager->registerReaderType<Provider> (reader_type);

				std::function<std::unique_ptr<handler::HandlerReaderBase>(std::unique_ptr<provider::ProviderReaderBase>, const std::string&)> f1 = [] (std::unique_ptr<Provider> dp, const std::string& path) -> std::unique_ptr<handler::HandlerReaderBase> {
					std::unique_ptr<handler::HandlerReaderBase> reader = std::make_unique<Reader>(std::move (dp), path);
					return reader;
				};

				handler->addReader (reader_type, f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <class Provider, class Writer>
		const std::shared_ptr<PluginHelper> addWriter (void) {
			Creator func = [] (const std::shared_ptr<GpsfileManager>& manager, const std::shared_ptr<PluginHandler>& handler) -> void {
				HandlerType writer_type;
				manager->registerWriterType<Provider> (writer_type);

				std::function<std::unique_ptr<handler::HandlerWriterBase>(std::unique_ptr<Provider>, const std::string&)> f1 = [] (std::unique_ptr<Provider> dp, const std::string& path) -> std::unique_ptr<handler::HandlerWriterBase> {
					std::unique_ptr<handler::HandlerWriterBase> writer = std::make_unique<Writer>(std::move (dp), path);
					return writer;
				};

				handler->addWriter (writer_type, f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <class Provider, class Reader, class Base>
		const std::shared_ptr<PluginHelper> addReader (std::shared_ptr<Base> base) {
			Creator func = [base] (const std::shared_ptr<GpsfileManager>& manager, const std::shared_ptr<PluginHandler>& handler) -> void {
				HandlerType reader_type;
				manager->registerReaderType<Provider> (reader_type);

				std::function<std::unique_ptr<handler::HandlerReaderBase>(std::unique_ptr<Provider>, const std::string&)> f1 = [base] (std::unique_ptr<Provider> dp, const std::string& path) -> std::unique_ptr<handler::HandlerReaderBase> {
					std::unique_ptr<handler::HandlerReaderBase> reader = std::make_unique<Reader>(base, std::move (dp), path);
					return reader;
				};

				handler->addReader (reader_type, f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		template <class Provider, class Writer, class Base>
		const std::shared_ptr<PluginHelper> addWriter (std::shared_ptr<Base> base) {
			Creator func = [base] (const std::shared_ptr<GpsfileManager>& manager, const std::shared_ptr<PluginHandler>& handler) -> void {
				HandlerType writer_type;
				manager->registerWriterType<Provider> (writer_type);

				std::function<std::unique_ptr<handler::HandlerWriterBase>(std::unique_ptr<Provider>, const std::string&)> f1 = [base] (std::unique_ptr<Provider> dp, const std::string& path) -> std::unique_ptr<handler::HandlerWriterBase> {
					std::unique_ptr<handler::HandlerWriterBase> writer = std::make_unique<Writer>(base, std::move (dp), path);
					return writer;
				};

				handler->addWriter (writer_type, f1);
			};

			this->_handlers.push_back (func);

			return this->_self.lock ();
		}

		void construct (::PluginDetails& details) {
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

#endif /* _LIBGPSFILE2_PLUGINHELPER_ */