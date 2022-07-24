#ifndef _X_LIBGPSFILE2_DATAHANDLER_
#define _X_LIBGPSFILE2_DATAHANDLER_

#include <cstddef>
#include <ostream>
#include <istream>

#include "libgpsfile2/BasePlugin.hpp"
#include "libgpsfile2/provider/provider.hpp"

namespace libgpsfile2 {
	// Base data handler class
	class BaseDatahandlerPlugin : public BasePlugin {
	protected:
		BaseDatahandlerPlugin (const std::string&);

	public:
		virtual ~BaseDatahandlerPlugin (void);
		virtual const std::string getPath (void) const final;
		virtual bool release (void) = 0;

	protected:
		std::streambuf *_buf;
		//std::shared_ptr<BaseDataProviderReader> dpr;
		//std::shared_ptr<BaseDataProviderWriter> dpw;

	private:
		std::string _path;
	};

	class BaseDatahandlerReaderPlugin : virtual public BaseDatahandlerPlugin {
	protected:
		BaseDatahandlerReaderPlugin (const std::string&);

	public:
		virtual ~BaseDatahandlerReaderPlugin (void);

		virtual std::size_t streamRead (char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Write the content as provided by the provider to a file. If no file is given, the value of path (passed to the constructor is used).
		virtual bool writeFile (const std::string& = {}) final;

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

	class BaseDatahandlerWriterPlugin : virtual public BaseDatahandlerPlugin {
	protected:
		BaseDatahandlerWriterPlugin (const std::string&);

	public:
		virtual ~BaseDatahandlerWriterPlugin (void);

		virtual std::size_t streamWrite (const char *, const std::size_t&, const off_t&) final;
		virtual bool release (void) final;

		// Read the content of a file to the provider. If no file is given, the value of path (passed to the constructor is used).
		virtual bool readFile (const std::string& = {}) final;

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

	struct pluginDatahandler {
		std::shared_ptr<const BasePlugin> instance;
		BaseDatahandlerRouteReaderPlugin *(*route_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderRouteReaderBase>, const std::string&);
		BaseDatahandlerRouteWriterPlugin *(*route_write_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderRouteWriterBase>, const std::string&);
		BaseDatahandlerWaypointReaderPlugin *(*waypoint_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderWaypointReaderBase>, const std::string&);
		BaseDatahandlerTableReaderPlugin *(*table_read_class) (const std::shared_ptr<const BasePlugin>&, std::shared_ptr<libgpsfile2::provider::ProviderTableReaderBase>, const std::string&);
	};
}

#endif /* _X_LIBGPSFILE2_DATAHANDLER_ */
