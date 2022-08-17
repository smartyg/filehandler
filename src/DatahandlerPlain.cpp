#include "config.h"

#include "DatahandlerPlain.hpp"

#include <string.h>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"
#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::HandlerBase;
using libgpsfile2::HandlerReaderBase;
using libgpsfile2::HandlerWriterBase;
using libgpsfile2::HandlerPlainReader;
using libgpsfile2::HandlerPlainWriter;

HandlerPlainReader::HandlerPlainReader (std::unique_ptr<provider::ProviderReaderBase> dp, const std::string& path) : HandlerBase(path), HandlerReaderBase (std::move (dp), path) {
	DEBUG_MSG("HandlerPlainReader::%s (%p, %s)\n", __func__, dp.get (), path.c_str ());
}

HandlerPlainReader::~HandlerPlainReader (void) {
	DEBUG_MSG("HandlerPlainReader::%s ()\n", __func__);
}

//size_t HandlerPlainReader::read (void *buf, size_t len, off_t offset) {
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

HandlerPlainWriter::HandlerPlainWriter (std::unique_ptr<provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase(path), HandlerWriterBase (std::move (dp), path) {
	DEBUG_MSG("HandlerPlainWriter::%s (%p, %s)\n", __func__, dp.get (), path.c_str ());
}

HandlerPlainWriter::~HandlerPlainWriter (void) {
	DEBUG_MSG("HandlerPlainWriter::%s ()\n", __func__);
}

bool HandlerPlainWriter::write (std::istream *s, const bool& finished) {
	DEBUG_MSG("HandlerPlainWriter::%s (%p, %d)\n", __func__, s, finished);

	// Only write when all data is availible
	if (finished) {
		// First make sure the stream is really \0 terminated by writing an (additional) \0 to the stream.
		libgpsfile2::utils::Iobuf *b = reinterpret_cast<libgpsfile2::utils::Iobuf *>(s->rdbuf ());
		b->sputc ('\0');
		bool ret = this->_dp->setData (b->getBuffer ());
		b->consumeAllGet ();
		return ret;
	}

	return true;
}
