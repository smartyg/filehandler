#ifndef _OPENAMBITWRITER_
#define _OPENAMBITWRITER_

#include <memory>
#include <istream>
#include <future>
#include <libxml/xmlreader.h>

#include <libgpsfile2/handler/HandlerWriterBase.hpp>
#include <libgpsfile2/provider/ProviderRouteWriterBase.hpp>

#include "OpenambitPlugin.hpp"
//#include "Hierarchy.hpp"

class OpenambitWriter final : public libgpsfile2::handler::HandlerWriterBase {
public:
	OpenambitWriter (const std::shared_ptr<OpenambitPlugin>, std::unique_ptr<libgpsfile2::provider::ProviderRouteWriterBase>, const std::string&);
	~OpenambitWriter (void);

private:
	bool write (std::istream *, const bool&) override;

	std::shared_ptr<OpenambitPlugin> _base_instance;
	libgpsfile2::provider::ProviderRouteWriterBase* _provider;
	//xmlTextReaderPtr _reader;
	std::future<bool> _reader;
	/*
	Hierarchy<xmlChar> _hierarchy;
	int _cur_route;
	int _cur_segment;
	int _cur_point;

	static int readerCallback (void* context, char* buffer, int len);
	static int closeCallback (void* context);

	void processNode (const xmlTextReaderPtr ptr);
	template <class CharT>
	static bool addData (libgpsfile2::provider::ProviderRouteWriterBase* provider, const Hierarchy<CharT>& hierarchy, const int& depth, const xmlChar* value);*/
};

#endif /* _OPENAMBITWRITER_ */