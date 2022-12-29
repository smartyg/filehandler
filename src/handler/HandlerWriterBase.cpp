#include "config.h"

#include "filehandler/handler/HandlerWriterBase.hpp"

#include <vector>
#include <ios>
#include <streambuf>
#include <fstream>
#include <string>
#include <stdexcept>
#include <Logger.hpp>

using filehandler::handler::HandlerWriterBase;

HandlerWriterBase::~HandlerWriterBase (void) {
	DEBUG_MSG ("HandlerWriterBase::{:s} ()\n", __func__);
	delete this->_s;
}

std::size_t HandlerWriterBase::streamWrite (const char *buf, const std::size_t& s, const off_t& o) {
	std::size_t n_put;
	{
		std::unique_lock lock (this->_mutex);
		DEBUG_MSG ("HandlerWriterBase::{:s} ({:p}, {:d}, {:d})\n", __func__, buf, s, o);

		if (this->_write_finished) return 0;
		std::streambuf *sbuf = this->_s->rdbuf ();
		n_put = sbuf->sputn (buf, s);
	}

	{
		std::shared_lock lock (this->_mutex);
		if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");
	}

	return n_put;
}

bool HandlerWriterBase::release (void) {
	DEBUG_MSG ("HandlerWriterBase::{:s} ()\n", __func__);
	this->_write_finished = true;
	if (!this->write (this->_s, this->_write_finished)) throw std::runtime_error ("Can not set value on object.");
	return true;
}

bool HandlerWriterBase::readFile (const std::string& file) {
	DEBUG_MSG ("HandlerWriterBase::{:s} ({:s})\n", __func__, file);

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
			input_stream.read (buffer.data (), 2048);
			{
				std::streamsize size = input_stream.gcount ();
				size_total += size;
				DEBUG_2_MSG (1, "read {:d} bytes in buffer, total read: {:d}\n", size, size_total);
				this->streamWrite (buffer.data (), size, 0);
			}
		}
		this->release ();
		input_stream.close ();
		return true;
	}

	return false;
}

std::istream* HandlerWriterBase::getStream (void) const {
	std::shared_lock lock (this->_mutex);
	return this->_s;
}

bool HandlerWriterBase::isFinished (void) const {
	std::shared_lock lock (this->_mutex);
	return this->_write_finished;
}

void HandlerWriterBase::lock (void) {
	this->_mutex.lock ();
}

void HandlerWriterBase::unlock (void) {
	this->_mutex.unlock ();
}