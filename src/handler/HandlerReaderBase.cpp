#include "config.h"
#include <features.h>

#include "libgpsfile2/handler/HandlerReaderBase.hpp"

#include <vector>
#include <ios>
#include <streambuf>
#include <fstream>
#include <string>
#include <algorithm>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/utils/Iobuf.hpp"


using libgpsfile2::handler::HandlerReaderBase;


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

	std::string file_out;
	if (file.empty ()) file_out = this->getPath ();
	else file_out = file;

	std::ofstream output_stream;
	output_stream.open (file_out);
	if (!output_stream.is_open()) {
		throw std::runtime_error ("can not open file: " + file_out);
	} else {
		const std::size_t buf_size = 4 * 1024;
		char buffer[buf_size];
		std::size_t read_size = -1;

		while (read_size != 0) {
			read_size = this->streamRead (buffer, buf_size, 0);
			output_stream.write (buffer, read_size);
		}
	}

	return false;
}
