#ifndef _X_LIBGPSFILE2_DATAHANDLER_
#define _X_LIBGPSFILE2_DATAHANDLER_

#include <cstddef>
#include <ostream>
#include <istream>
#include <memory>
#include <map>

#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/BasePlugin.hpp"
#include "libgpsfile2/provider/provider.hpp"
#include "libgpsfile2/utils/Iobuf.hpp"
#include "libgpsfile2/traits/provider.hpp"

namespace libgpsfile2 {/*
	template<typename Derived, typename Base, typename Del>
	std::unique_ptr<Derived, Del> dynamic_unique_ptr_cast (std::unique_ptr<Base, Del>&& p) {
		if(Derived *result = dynamic_cast<Derived *>(p.get ())) {
			p.release ();
			return std::unique_ptr<Derived, Del>(result, std::move (p.get_deleter()));
		}
		return std::unique_ptr<Derived, Del>(nullptr, p.get_deleter ());
	}*/

	template<typename Derived, typename Base>
	std::unique_ptr<Derived> dynamic_unique_ptr_cast (std::unique_ptr<Base>&& p) {
		if(Derived *result = dynamic_cast<Derived *>(p.get ())) {
			p.release ();
			return std::unique_ptr<Derived>(result);
		}
		return std::unique_ptr<Derived>(nullptr);
	}

	// Base data handler class
	class HandlerBase : public BasePlugin {
	protected:
		//HandlerBase (const std::string&);
		HandlerBase (const std::string& path) {
			DEBUG_MSG("HandlerBase::%s ()\n", __func__);
			this->_path = path;
			this->_buf = new libgpsfile2::utils::Iobuf ();
		}

	public:
		virtual ~HandlerBase (void);
		virtual const std::string getPath (void) const final;
		virtual bool release (void) = 0;

	protected:
		std::streambuf *_buf;

	private:
		std::string _path;
	};

	class HandlerReaderBase : virtual public HandlerBase {
	protected:
		std::unique_ptr<provider::ProviderReaderBase> _dp;

		//HandlerReaderBase (std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
		HandlerReaderBase (std::unique_ptr<provider::ProviderReaderBase> dp, const std::string& path) : HandlerBase (path) {
			DEBUG_MSG("HandlerReaderBase::%s ()\n", __func__);
			this->_dp = std::move (dp);
			this->_s = new std::ostream (this->_buf);
			this->_read_finished = false;
		}

	public:
		virtual ~HandlerReaderBase (void);

		virtual std::size_t streamRead (char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Write the content as provided by the provider to a file. If no file is given, the value of path (passed to the constructor is used).
		virtual bool writeFile (const std::string& = {}) final;

		template<class T>
		std::unique_ptr<T> getProvider (void) {
			if (this->_read_finished)
				return std::move(std::dynamic_pointer_cast<T>(this->_dp));
			return std::unique_ptr<T>();
		}

	protected:
		virtual ssize_t streamAvailible (const off_t& = 0) const final;

		/**
		 * This is where the actual reading happens. When this method is called there is a request to read at least n bytes of data.
		 * The method should write at least n bytes to the provided stream. If writen less than requested bytes or if true is returned,
		 * it will be interperated as EOF (end of file) and no more calls to this method will be made.
		 */
		virtual bool read (std::ostream *stream, const std::size_t& len) = 0;

	private:
		std::ostream *_s;
		bool _read_finished;
	};

	class HandlerWriterBase : virtual public HandlerBase {

	protected:
		std::unique_ptr<provider::ProviderWriterBase> _dp;

		//HandlerWriterBase (std::unique_ptr<provider::ProviderWriterBase>, const std::string&);
		HandlerWriterBase (std::unique_ptr<provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase (path) {
			DEBUG_MSG("HandlerWriterBase::%s ()\n", __func__);
			assert (dp);
			this->_dp = std::move (dp);
			assert (this->_dp);
			this->_s = new std::istream (this->_buf);
			this->_write_finished = false;
		}

	public:
		virtual ~HandlerWriterBase (void);

		virtual std::size_t streamWrite (const char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Read the content of a file to the provider. If no file is given, the value of path (passed to the constructor is used).
		virtual bool readFile (const std::string& = {}) final;

		template<provider::ProviderWriterTrait T>
		std::unique_ptr<T> getProvider (void) {
			if (this->_write_finished) {/*
				provider::ProviderWriterBase *provider_base_ptr = this->_dp.get ();
				T* ptr = dynamic_cast<T*>(provider_base_ptr);
				if (ptr == nullptr) throw std::runtime_error ("Can not cast provider");
				std::unique_ptr<T> o = std::unique_ptr<T, decltype (this->_dp.get_deleter ())>(ptr, std::move (this->_dp.get_deleter ()));
				this->_dp.release ();
				return o;*/
				auto res = dynamic_unique_ptr_cast<T> (std::move (this->_dp));
				return res;
			}
			return std::unique_ptr<T>();
		}

	protected:
		//virtual std::istream *getStream (void) const final;

		/**
		 * This function is called after each write request. The writen data is contained in the provided stream.
		 * When finished is true, this is the last and final call (there will be no more data provided after that final call).
		 * returns true for each successfull write, if returned false this means there occured an error in the write and the action will be terminated.
		 */
		virtual bool write (std::istream *stream, const bool& finished) = 0;

	private:
		std::istream *_s;
		bool _write_finished;
	};
#if 0
	class BaseDatahandlerRouteReaderPlugin : virtual public BaseDatahandlerReaderPlugin {
	public:
		BaseDatahandlerRouteReaderPlugin (const std::string&);
		virtual ~BaseDatahandlerRouteReaderPlugin (void) = default;
/*
	protected:
		const std::shared_ptr<DataProviderRouteReader> _dp;*/
	};

	class BaseDatahandlerRouteWriterPlugin : virtual public BaseDatahandlerWriterPlugin {
	public:
		BaseDatahandlerRouteWriterPlugin (const std::string&);
		virtual ~BaseDatahandlerRouteWriterPlugin (void) = default;
/*
	protected:
		const std::shared_ptr<DataProviderRouteWriter> _dp;*/
	};

	class BaseDatahandlerWaypointReaderPlugin : virtual public BaseDatahandlerReaderPlugin {
	public:
		BaseDatahandlerWaypointReaderPlugin (const std::string&);
		virtual ~BaseDatahandlerWaypointReaderPlugin (void) = default;
/*
	protected:
		const std::shared_ptr<DataProviderWaypointReader> _dp;*/
	};

	class BaseDatahandlerTableReaderPlugin : virtual public BaseDatahandlerReaderPlugin {
	public:
		BaseDatahandlerTableReaderPlugin (const std::string&);
		virtual ~BaseDatahandlerTableReaderPlugin (void) = default;
/*
	protected:
		const std::shared_ptr<DataProviderTableReader> _dp;*/
	};
#endif
	typedef std::unique_ptr<HandlerReaderBase> (*reader_creator) (const std::shared_ptr<const BasePlugin>&, std::unique_ptr<provider::ProviderReaderBase>, const std::string&);
	typedef std::unique_ptr<HandlerWriterBase> (*writer_creator) (const std::shared_ptr<const BasePlugin>&, std::unique_ptr<provider::ProviderWriterBase>, const std::string&);

	struct pluginDatahandler {
		std::shared_ptr<const BasePlugin> instance;
		std::map<int, reader_creator> reader_creators;
		std::map<int, writer_creator> writer_creators;
		//BaseDatahandlerRouteReaderPlugin *(*route_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderRouteReaderBase>, const std::string&);
		//BaseDatahandlerRouteWriterPlugin *(*route_write_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderRouteWriterBase>, const std::string&);
		//BaseDatahandlerWaypointReaderPlugin *(*waypoint_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderWaypointReaderBase>, const std::string&);
		//BaseDatahandlerTableReaderPlugin *(*table_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderTableReaderBase>, const std::string&);
	};
}

#endif /* _X_LIBGPSFILE2_DATAHANDLER_ */
