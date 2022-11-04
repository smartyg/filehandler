#ifndef _LIBGPSFILE2_GPSFILEMANAGER_
#define _LIBGPSFILE2_GPSFILEMANAGER_

#include <memory>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <string_view>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <Logger.hpp>

#include <libgpsfile2/types/HandlerCreatorFunc.hpp>
#include <libgpsfile2/types/PluginCreatorFunc.hpp>
#include <libgpsfile2/types/PluginDetails.hpp>
#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/traits/ProviderReaderTrait.hpp>
#include <libgpsfile2/traits/ProviderWriterTrait.hpp>
#include <libgpsfile2/utils/dynamic_unique_ptr_cast.hpp>

namespace libgpsfile2 {

	class GpsfileManager final {
		using KeyType = std::pair<const std::string, const std::size_t>;

		std::map<const std::size_t, std::vector<std::string>> _provider_type_extension_map;
		std::map<const std::string, std::vector<std::size_t>> _extension_provider_type_map;
		std::map<KeyType, HandlerCreatorFunc> _handler_map;

		GpsfileManager (void);
		GpsfileManager (const GpsfileManager&) = delete;                // copy constructor
		GpsfileManager (GpsfileManager&&) noexcept = delete;            // move constructor
		GpsfileManager& operator= (const GpsfileManager&) = delete;     // copy assignment
		GpsfileManager& operator= (GpsfileManager&&) noexcept = delete; // move assignment

	public:
		constexpr static const std::string_view TAG = "GpsfileManager";

		~GpsfileManager (void);

		template <provider::ProviderReaderAbstractTrait ProviderBaseType, provider::ProviderReaderFinalTrait Provider>
		[[nodiscard]] std::unique_ptr<handler::HandlerReaderBase> createReader (std::unique_ptr<Provider> provider, const std::string& path, const std::string& extension) const {
			static_assert (std::is_base_of<ProviderBaseType, Provider>::value);
			DEBUG_MSG ("GpsfileManager::{:s} ({:p}, {:s}, {:s})\n", __func__, fmt::ptr (provider), path, extension);
			KeyType key = std::make_pair (extension, GpsfileManager::getProviderType<ProviderBaseType>());
			DEBUG_2_MSG (1, "GpsfileManager::{:s}: key {{{:s}, {:d}}}\n", __func__, key.first, key.second);
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
			DEBUG_MSG ("GpsfileManager::{:s} ({:p}, {:s}, {:s})\n", __func__, fmt::ptr (provider), path, extension);
			KeyType key = std::make_pair (extension, GpsfileManager::getProviderType<ProviderBaseType>());
			DEBUG_2_MSG (1, "GpsfileManager::{:s}: key {{{:s}, {:d}}}\n", __func__, key.first, key.second);
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

		[[nodiscard]] static const std::shared_ptr<GpsfileManager> getPtr (void);

		template <class ProviderBase>
		inline constexpr static std::size_t getProviderType (void) {
			static_assert (std::is_abstract<ProviderBase>::value && (std::is_base_of<provider::ProviderReaderBase, ProviderBase>::value || std::is_base_of<provider::ProviderWriterBase, ProviderBase>::value));
			return typeid (ProviderBase).hash_code ();
		}
	};
}

#endif /* _LIBGPSFILE2_GPSFILEMANAGER_ */