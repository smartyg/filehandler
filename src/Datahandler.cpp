#include "config.h"

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/DatahandlerRoute.hpp"

#include <cstring>
#include <vector>
#include <istream>
#include <ostream>
#include <fstream>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::HandlerBase;
using libgpsfile2::HandlerReaderBase;
using libgpsfile2::HandlerWriterBase;
//using libgpsfile2::HandlerRouteReaderBase;
//using libgpsfile2::HandlerRouteWriterBase;
//using libgpsfile2::BaseDatahandlerWaypointReaderPlugin;
//using libgpsfile2::BaseDatahandlerTableReaderPlugin;
/*
HandlerBase::HandlerBase (const std::string& path) {
	DEBUG_MSG("HandlerBase::%s ()\n", __func__);
	this->_path = path;
	this->_buf = new libgpsfile2::utils::Iobuf ();
}
*/
HandlerBase::~HandlerBase (void) {
	DEBUG_MSG("HandlerBase::%s ()\n", __func__);
	this->_path.clear ();
	delete this->_buf;
}

const std::string HandlerBase::getPath (void) const {
	DEBUG_MSG("HandlerBase::%s ()\n", __func__);
	return this->_path;
}
/*
HandlerReaderBase::HandlerReaderBase (std::unique_ptr<provider::ProviderReaderBase> dp, const std::string& path) : HandlerBase (path) {
	DEBUG_MSG("HandlerReaderBase::%s ()\n", __func__);
	this->_dp = std::move (dp);
	this->_s = new std::ostream (this->_buf);
	this->_read_finished = false;
}
*/
HandlerReaderBase::~HandlerReaderBase (void) {
	DEBUG_MSG("HandlerReaderBase::%s ()\n", __func__);
	delete this->_s;
}

std::size_t HandlerReaderBase::streamRead (char *buf, const std::size_t& s, const off_t& offset) {
	DEBUG_MSG("HandlerReaderBase::%s (%p, %ld, %ld)\n", __func__, buf, s, offset);
	std::streambuf *sbuf = this->_s->rdbuf ();
	if ((sbuf->in_avail () < static_cast<std::streamsize>(s)) && !this->_read_finished) this->_read_finished = this->read (this->_s, sbuf->in_avail () - s);
	std::size_t n_read = std::min (static_cast<std::size_t>(sbuf->in_avail ()), s);
	return sbuf->sgetn (buf, n_read);
}

ssize_t HandlerReaderBase::streamAvailible (const off_t& offset) const {
	DEBUG_MSG("HandlerReaderBase::%s (%ld)\n", __func__, offset);
	libgpsfile2::utils::Iobuf *sbuf = reinterpret_cast<libgpsfile2::utils::Iobuf *>(this->_s->rdbuf ());
	if (offset < sbuf->getNumGet ()) return -1;
	return sbuf->in_avail () + sbuf->getNumGet () - offset;
}

bool HandlerReaderBase::release (void) {
	DEBUG_MSG("HandlerReaderBase::%s ()\n", __func__);
	return true;
}


bool HandlerReaderBase::writeFile (const std::string& file) {
	DEBUG_MSG("HandlerReaderBase::%s (%s)\n", __func__, file.c_str ());

	std::string file_in;
	if (file.empty ()) file_in = this->getPath ();
	else file_in = file;
	return false;
}
/*
HandlerWriterBase::HandlerWriterBase (std::unique_ptr<provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase (path) {
	DEBUG_MSG("HandlerWriterBase::%s ()\n", __func__);
	this->_dp = std::move (dp);
	this->_s = new std::istream (this->_buf);
	this->_write_finished = false;
}
*/
HandlerWriterBase::~HandlerWriterBase (void) {
	DEBUG_MSG("HandlerWriterBase::%s ()\n", __func__);
	delete this->_s;
}

std::size_t HandlerWriterBase::streamWrite (const char *buf, const std::size_t& s, const off_t& o) {
	DEBUG_MSG("HandlerWriterBase::%s (%p, %ld, %ld)\n", __func__, buf, s, o);
	if (this->_write_finished) return 0;
	std::streambuf *sbuf = this->_s->rdbuf ();
	std::size_t n_put = sbuf->sputn (buf, s);

	if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");

	return n_put;
}

bool HandlerWriterBase::release (void) {
	DEBUG_MSG("HandlerWriterBase::%s ()\n", __func__);
	this->_write_finished = true;
	if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");
	return true;
}

bool HandlerWriterBase::readFile (const std::string& file) {
	DEBUG_MSG("HandlerWriterBase::%s (%s)\n", __func__, file.c_str ());

	if (this->_write_finished) return false;

	std::string file_in;
	if (file.empty ()) file_in = this->getPath ();
	else file_in = file;

	std::ifstream input_stream;
	input_stream.open (file_in);
	if (!input_stream.is_open()) {
		throw std::runtime_error ("can not open file: " + file_in);
	} else {
		std::size_t size_total = 0;
		std::vector<char> buffer (2048);
		while (!input_stream.eof ()) { // keep reading until end-of-file
			//std::vector<char> buffer (2048);
			//if (input_stream.read (buffer.data (), 2048))
			input_stream.read (buffer.data (), 2048);
			{
				std::streamsize size = input_stream.gcount ();
				size_total += size;
				DEBUG_MSG("read %d bytes in buffer, total read: %d\n", size, size_total);
				this->streamWrite (buffer.data (), size, 0);
			}
		}
		this->release ();
		input_stream.close ();
		return true;
	}

	return false;
}

//HandlerRouteReaderBase::HandlerRouteReaderBase (std::unique_ptr<provider::ProviderReaderBase> provider, const std::string& path) : HandlerReaderBase (std::move (provider), path) {}

//HandlerRouteWriterBase::HandlerRouteWriterBase (std::unique_ptr<provider::ProviderWriterBase> provider, const std::string& path) : HandlerWriterBase (std::move (provider), path) {}
/*
BaseDatahandlerWaypointReaderPlugin::BaseDatahandlerWaypointReaderPlugin (const std::string& path) : HandlerReaderBase (path) {}

BaseDatahandlerTableReaderPlugin::BaseDatahandlerTableReaderPlugin (const std::string& path) : HandlerReaderBase (path) {}
*/