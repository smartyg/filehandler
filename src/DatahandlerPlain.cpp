#include "config.h"

#include "DatahandlerPlain.hpp"

#include <string.h>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2/Datahandler.hpp"
#include "libgpsfile2/provider/provider.hpp"
#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::BaseDatahandlerPlugin;
using libgpsfile2::BaseDatahandlerReaderPlugin;
using libgpsfile2::BaseDatahandlerWriterPlugin;
using libgpsfile2::DatahandlerPlainReader;
using libgpsfile2::DatahandlerPlainWriter;

DatahandlerPlainReader::DatahandlerPlainReader (std::shared_ptr<libgpsfile2::provider::ProviderReaderBase> dp, const std::string& path) : BaseDatahandlerPlugin(path), BaseDatahandlerReaderPlugin (path) {
	DEBUG_MSG("DatahandlerPlainReader::%s (%p, %s)\n", __func__, dp, path);
	this->data_provider = dp;
}

DatahandlerPlainReader::~DatahandlerPlainReader (void) {
	DEBUG_MSG("DatahandlerPlainReader::%s ()\n", __func__);
}

//size_t DatahandlerPlainReader::read (void *buf, size_t len, off_t offset) {
bool DatahandlerPlainReader::read (std::ostream *s, const std::size_t& len) {
	DEBUG_MSG("DatahandlerPlainReader::%s (%p, %ld)\n", __func__, s, len);
	// len is not used as we write only once and put in all characters that we can get.
	(void)len;

	// Get the data from the provider.
	const std::string data = this->data_provider->getData ();

	// Write the data to the stream.
	s->write (data.c_str (), data.size ());

	// Return true to indicate this was the last piece of data to be written (eof).
	return true;
}

DatahandlerPlainWriter::DatahandlerPlainWriter (std::shared_ptr<libgpsfile2::provider::ProviderWriterBase> dp, const std::string& path) : BaseDatahandlerPlugin(path), BaseDatahandlerWriterPlugin (path) {
	DEBUG_MSG("DatahandlerPlainWriter::%s (%p, %s)\n", __func__, dp, path);
	this->data_provider = dp;
}

DatahandlerPlainWriter::~DatahandlerPlainWriter (void) {
	DEBUG_MSG("DatahandlerPlainWriter::%s ()\n", __func__);
}

bool DatahandlerPlainWriter::write (std::istream *s, const bool& finished) {
	DEBUG_MSG("DatahandlerPlainWriter::%s (%p, %d)\n", __func__, s, finished);

	// Only write when all data is availible
	if (finished) {
		// First make sure the stream is really \0 terminated by writing an (additional) \0 to the stream.
		libgpsfile2::utils::Iobuf *b = reinterpret_cast<libgpsfile2::utils::Iobuf *>(s->rdbuf ());
		b->sputc ('\0');
		bool ret = this->data_provider->setData (b->getBuffer ());
		b->consumeAllGet ();
		return ret;
	}

	return true;
}
