#include "config.h"

#include "libgpsfile2/Datahandler.hpp"

#include <cstring>
#include <istream>
#include <ostream>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::BaseDatahandlerPlugin;
using libgpsfile2::BaseDatahandlerReaderPlugin;
using libgpsfile2::BaseDatahandlerWriterPlugin;
using libgpsfile2::BaseDatahandlerRouteReaderPlugin;
using libgpsfile2::BaseDatahandlerRouteWriterPlugin;
using libgpsfile2::BaseDatahandlerWaypointReaderPlugin;
using libgpsfile2::BaseDatahandlerTableReaderPlugin;

BaseDatahandlerPlugin::BaseDatahandlerPlugin (const std::string& path) {
	DEBUG_MSG("BaseDatahandlerPlugin::%s ()\n", __func__);
	this->_path = path;
	this->_buf = new libgpsfile2::utils::Iobuf ();
}

BaseDatahandlerPlugin::~BaseDatahandlerPlugin (void) {
	DEBUG_MSG("BaseDatahandlerPlugin::%s ()\n", __func__);
	this->_path.clear ();
	delete this->_buf;
}

const std::string BaseDatahandlerPlugin::getPath (void) const {
	DEBUG_MSG("BaseDatahandlerPlugin::%s ()\n", __func__);
	return this->_path;
}

BaseDatahandlerReaderPlugin::BaseDatahandlerReaderPlugin (const std::string& path) : BaseDatahandlerPlugin (path) {
	DEBUG_MSG("BaseDatahandlerReaderPlugin::%s ()\n", __func__);
	this->_s = new std::ostream(this->_buf);
	this->_read_finished = false;
}

BaseDatahandlerReaderPlugin::~BaseDatahandlerReaderPlugin (void) {
	DEBUG_MSG("BaseDatahandlerReaderPlugin::%s ()\n", __func__);
	delete this->_s;
}

std::size_t BaseDatahandlerReaderPlugin::streamRead (char *buf, const std::size_t& s, const off_t& offset) {
	DEBUG_MSG("BaseDatahandlerReaderPlugin::%s (%p, %ld, %ld)\n", __func__, buf, s, offset);
	std::streambuf *sbuf = this->_s->rdbuf ();
	if ((sbuf->in_avail () < static_cast<std::streamsize>(s)) && !this->_read_finished) this->_read_finished = this->read (this->_s, sbuf->in_avail () - s);
	std::size_t n_read = std::min (static_cast<std::size_t>(sbuf->in_avail ()), s);
	return sbuf->sgetn (buf, n_read);
}

ssize_t BaseDatahandlerReaderPlugin::streamAvailible (const off_t& offset) const {
	DEBUG_MSG("BaseDatahandlerReaderPlugin::%s (%ld)\n", __func__, offset);
	libgpsfile2::utils::Iobuf *sbuf = reinterpret_cast<libgpsfile2::utils::Iobuf *>(this->_s->rdbuf ());
	if (offset < sbuf->getNumGet ()) return -1;
	return sbuf->in_avail () + sbuf->getNumGet () - offset;
}

bool BaseDatahandlerReaderPlugin::release (void) {
	DEBUG_MSG("BaseDatahandlerReaderPlugin::%s ()\n", __func__);
	return true;
}

BaseDatahandlerWriterPlugin::BaseDatahandlerWriterPlugin (const std::string& path) : BaseDatahandlerPlugin (path) {
	DEBUG_MSG("BaseDatahandlerWriterPlugin::%s ()\n", __func__);
	this->_s = new std::istream(this->_buf);
	this->_write_finished = false;
}

BaseDatahandlerWriterPlugin::~BaseDatahandlerWriterPlugin (void) {
	DEBUG_MSG("BaseDatahandlerWriterPlugin::%s ()\n", __func__);
	delete this->_s;
}

std::size_t BaseDatahandlerWriterPlugin::streamWrite (const char *buf, const std::size_t& s, const off_t& o) {
	DEBUG_MSG("BaseDatahandlerWriterPlugin::%s (%p, %ld, %ld)\n", __func__, buf, s, o);
	if (this->_write_finished) return 0;
	std::streambuf *sbuf = this->_s->rdbuf ();
	std::size_t n_put = sbuf->sputn (buf, s);

	if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");

	return n_put;
}

bool BaseDatahandlerWriterPlugin::release (void) {
	DEBUG_MSG("BaseDatahandlerWriterPlugin::%s ()\n", __func__);
	this->_write_finished = true;
	if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");
	return true;
}

BaseDatahandlerRouteReaderPlugin::BaseDatahandlerRouteReaderPlugin (const std::string& path) : BaseDatahandlerReaderPlugin (path) {}

BaseDatahandlerRouteWriterPlugin::BaseDatahandlerRouteWriterPlugin (const std::string& path) : BaseDatahandlerWriterPlugin (path) {}

BaseDatahandlerWaypointReaderPlugin::BaseDatahandlerWaypointReaderPlugin (const std::string& path) : BaseDatahandlerReaderPlugin (path) {}

BaseDatahandlerTableReaderPlugin::BaseDatahandlerTableReaderPlugin (const std::string& path) : BaseDatahandlerReaderPlugin (path) {}