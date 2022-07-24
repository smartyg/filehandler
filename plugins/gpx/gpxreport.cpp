#include "config.h"

#include "gpxreport.hpp"

#include <string>
#include <gpx/Report.h>
#include <gpx/Node.h>
#include <gpsdata/utils/Logger.hpp>

#include "libgpsfile2.hpp"

GpxReport::GpxReport (void) {
}

GpxReport::~GpxReport (void) {
}

void GpxReport::report(const gpx::Node *node, gpx::Report::Warning warning,  const std::string &extra)
{
	std::string text;
	if (node != nullptr)
	{
		text = (node->getType() == gpx::Node::ATTRIBUTE ? "Attribute " : "Element ") + node->getName() + " : ";
	}

	text += GpxReport::text(warning);

	if (!extra.empty())
	{
		text += ": " + extra;
	}

	text + ".";

	NOTICE_MSG("%s\n", text.c_str());
}



