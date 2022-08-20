#ifndef _LIBGPSFILE2_PLUGINHANDLER_
#define _LIBGPSFILE2_PLUGINHANDLER_

#include <functional>
#include <memory>
#include <string>
#include <map>
#include <stdexcept>
#include <utility>
#include <cassert>

#include <libgpsfile2/types/BasePlugin.hpp>
#include <libgpsfile2/types/HandlerType.hpp>
#include <libgpsfile2/utils/dynamic_unique_ptr_cast.hpp>
#include <libgpsfile2/handler/HandlerReaderBase.hpp>
#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderReaderBase.hpp>
#include <libgpsfile2/provider/ProviderWriterBase.hpp>
#include <libgpsfile2/traits/ProviderReaderTrait.hpp>
#include <libgpsfile2/traits/ProviderWriterTrait.hpp>

namespace libgpsfile2 {
	using reader_creator = std::function<std::unique_ptr<handler::HandlerReaderBase>(const std::size_t&, std::unique_ptr<provider::ProviderReaderBase>, const std::string&)>;
	using writer_creator = std::function<std::unique_ptr<handler::HandlerWriterBase>(const std::size_t&, std::unique_ptr<provider::ProviderWriterBase>, const std::string&)>;

	template<class T>
	using ReaderFunc = std::unique_ptr<handler::HandlerReaderBase> (*) (const std::shared_ptr<BasePlugin>, std::unique_ptr<T>, const std::string&);

	template<class T>
	using WriterFunc = std::unique_ptr<handler::HandlerWriterBase> (*) (const std::shared_ptr<BasePlugin>, std::unique_ptr<T>, const std::string&);

	class PluginHandler {
		friend class GpsfilePlugin;

		const std::shared_ptr<BasePlugin> _instance;
		std::map<int, reader_creator> _reader_creators;
		std::map<int, writer_creator> _writer_creators;

	public:
		PluginHandler (const std::shared_ptr<BasePlugin> instance) noexcept : _instance(instance) { }
		~PluginHandler (void) {
			//this->_instance.reset ();
			this->_reader_creators.clear ();
			this->_writer_creators.clear ();
		}

		template <provider::ProviderReaderTrait T>
		void addReader (const HandlerType& dht, ReaderFunc<T> func) {
			assert (dht);
			if (!dht) throw std::runtime_error ("no type given");
			reader_creator init_func = [base = this->_instance, func] (const std::size_t& type_hash, std::unique_ptr<provider::ProviderReaderBase> provider, const std::string& path) -> std::unique_ptr<handler::HandlerReaderBase> {
				assert (typeid (T).hash_code () == type_hash);
				// Convert `provider` to type T
				std::unique_ptr<T> dp_downcasted = dynamic_unique_ptr_cast<T> (std::move (provider));
				if (!dp_downcasted) throw std::runtime_error ("can not cast");

				return func (base, std::move (dp_downcasted), path);
			};

			this->_reader_creators[static_cast<int>(dht)] = init_func;
		}

		template <provider::ProviderWriterTrait T>
		void addWriter (const HandlerType& dht, WriterFunc<T> func) {
			assert (dht);
			if (!dht) throw std::runtime_error ("no type given");
			writer_creator init_func = [base = this->_instance, func] (const std::size_t& type_hash, std::unique_ptr<provider::ProviderWriterBase> provider, const std::string& path) -> std::unique_ptr<handler::HandlerWriterBase> {
				assert (typeid (T).hash_code () == type_hash);
				// Convert `provider` to type T
				std::unique_ptr<T> dp_downcasted = dynamic_unique_ptr_cast<T> (std::move (provider));
				if (!dp_downcasted) throw std::runtime_error ("can not cast");

				return func (base, std::move (dp_downcasted), path);
			};

			this->_writer_creators[static_cast<int>(dht)] = init_func;
		}

	private:
		// These functions are only allowed to be called by `GpsfilePlugin` class.

		std::unique_ptr<handler::HandlerReaderBase> createReader (const std::size_t& type_hash, std::unique_ptr<provider::ProviderReaderBase> provider, const HandlerType& dht, const std::string& path) const {
			// Lookup correct reader function.
			auto func = this->_reader_creators.at(static_cast<int>(dht));

			// Run the function and return result.
			return func (type_hash, std::move (provider), path);
		}

		std::unique_ptr<handler::HandlerWriterBase> createWriter (const std::size_t& type_hash, std::unique_ptr<provider::ProviderWriterBase> provider, const HandlerType& dht, const std::string& path) const {
			// Lookup correct writer function.
			auto func = this->_writer_creators.at(static_cast<int>(dht));

			// Run the function and return result.
			return func (type_hash, std::move (provider), path);
		}
	};
}

#endif /* _LIBGPSFILE2_PLUGINHANDLER_ */
