#include "config.h"
#include <features.h>

#include "HandlerPlainReader.hpp"

#include <memory>
#include <string>
#include <ostream>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/provider/ProviderReaderBase.hpp"

using libgpsfile2::handler::HandlerPlainReader;

HandlerPlainReader::HandlerPlainReader (std::unique_ptr<libgpsfile2::provider::ProviderReaderBase> provider, const std::string& path) : HandlerBase(path), HandlerReaderBase (std::move (provider), path) {
	DEBUG_MSG("HandlerPlainReader::%s (%p, %s)\n", __func__, provider.get (), path.c_str ());
}

HandlerPlainReader::~HandlerPlainReader (void) {
	DEBUG_MSG("HandlerPlainReader::%s ()\n", __func__);
}

bool HandlerPlainReader::read (std::ostream *s, const std::size_t& len) {
	DEBUG_MSG("HandlerPlainReader::%s (%p, %ld)\n", __func__, s, len);
	// len is not used as we write only once and put in all characters that we can get.
	(void)len;

	// Get the data from the provider.
	const std::string data = this->_dp->getData ();

	// Write the data to the stream.
	s->write (data.c_str (), data.size ());

	// Return true to indicate this was the last piece of data to be written (eof).
	return true;
}
