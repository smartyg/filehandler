#include "config.h"

#include "HandlerPlainWriter.hpp"

#include <memory>
#include <string>
#include <istream>
#include <Logger.hpp>

#include "libgpsfile2/provider/ProviderWriterBase.hpp"
#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::handler::HandlerPlainWriter;

HandlerPlainWriter::HandlerPlainWriter (std::unique_ptr<libgpsfile2::provider::ProviderWriterBase> dp, const std::string& path) : HandlerBase(path), HandlerWriterBase (std::move (dp), path) {
	DEBUG_MSG ("HandlerPlainWriter::{:s} ({:p}, {:s})\n", __func__, fmt::ptr (dp), path);
}

HandlerPlainWriter::~HandlerPlainWriter (void) {
	DEBUG_MSG ("HandlerPlainWriter::{:s} ()\n", __func__);
}

bool HandlerPlainWriter::write (std::istream *s, const bool& finished) {
	DEBUG_MSG ("HandlerPlainWriter::{:s} ({:p}, {:d})\n", __func__, fmt::ptr (s), finished);

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