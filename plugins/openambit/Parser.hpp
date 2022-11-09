#ifndef _PARSER_
#define _PARSER_

#include <memory>
#include <thread>
#include <chrono>
#include <utility>
#include <cassert>
#include <libxml/xmlreader.h>
#include <Logger.hpp>

#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>
#include <libgpsfile2/utils/Iobuf.hpp>

#include "OpenambitPlugin.hpp"
#include "OpenambitWriter.hpp"

class Parser {
	const libgpsfile2::provider::ProviderRouteWriterBase* _provider;
	const OpenambitWriter* _writer;
	const std::shared_ptr<OpenambitPlugin> _base;

	xmlTextReaderPtr _reader;

public:
	Parser (libgpsfile2::provider::ProviderRouteWriterBase* provider, OpenambitWriter* writer, const std::shared_ptr<OpenambitPlugin> base) : _provider (provider), _writer (writer), _base (base) {
		DEBUG_MSG ("Parser::{:s} ({:p}, {:p}, {:p})\n", __func__, fmt::ptr (provider), fmt::ptr (writer), fmt::ptr (base));
	}
	~Parser (void) {
		DEBUG_MSG ("Parser::{:s} ()\n", __func__);
	}

	bool parse (void) {
		DEBUG_MSG ("Parser::{:s} ()\n", __func__);

		if ((this->_reader = xmlReaderForIO (readerCallback, closeCallback, const_cast<OpenambitWriter*>(this->_writer), this->_writer->getPath ().c_str (), "utf-8", XML_PARSE_PEDANTIC | XML_PARSE_NONET | XML_PARSE_HUGE)) == nullptr)
			ERROR_MSG ("can not load libxml2 reader for file {:s}\n", this->_writer->getPath ());

		int ret = xmlTextReaderRead (this->_reader);
		while (ret == 1) {
			//this->processNode (this->_reader);
			DEBUG_MSG ("processNode: {:p}\n", fmt::ptr (this->_reader));
			//if (!is_final && sbuf->in_avail () <= 256) break;
			//ret = xmlTextReaderRead (this->_reader);
			ret = xmlTextReaderNext (this->_reader);
		}

		if (ret == -1) {
			ERROR_MSG ("{:s} : failed to parse ({:d})\n", this->_writer->getPath (), ret);
		}

		xmlFreeTextReader (this->_reader);

		return true;
	}

private:
	static int readerCallback (void* context, char* out_buffer, int len) {
		DEBUG_MSG ("Parser::{:s} ({:p}, {:p}, {:d})\n", __func__, fmt::ptr (context), fmt::ptr (out_buffer), len);

		OpenambitWriter* ptr = static_cast<OpenambitWriter*>(context);
		libgpsfile2::utils::Iobuf *sbuf = reinterpret_cast<libgpsfile2::utils::Iobuf *>(ptr->getStream ()->rdbuf ());

		while (!ptr->isFinished () && sbuf->in_avail () == 0) {
			std::this_thread::sleep_for (std::chrono::milliseconds (100));
		}

		ptr->lock ();
		// Return upto `availible` bytes, but not more than requested.
		const std::size_t read = std::min (static_cast<std::streamsize>(len), sbuf->in_avail ());
		DEBUG_MSG ("will copy {:d} bytes into xml reader buffer.\n", read);
		const void* ret = std::memcpy (out_buffer, sbuf->getBuffer (), read);
		assert (ret == out_buffer);
		DEBUG_MSG ("copied {:d} bytes into xml reader buffer.\n", read);
		sbuf->consumeGet (read);
		ptr->unlock ();

		DEBUG_2_MSG (2, "return: {:d}\n", read);

		return read;
	}

	static int closeCallback (void* context) {
		DEBUG_MSG ("OpenambitWriter::{:s} ({:p})\n", __func__, fmt::ptr (context));

		OpenambitWriter* ptr = static_cast<OpenambitWriter*>(context);

		assert (ptr->isFinished());

		//ptr->_provider->finished ();
		//xmlFreeTextReader (ptr->_reader);

		return 0;
	}
};

#endif /* _PARSER_ */