#include "config.h"

#include "Report.hpp"

#include <string>
#include <gpx/Report.h>
#include <gpx/Node.h>
#include <Logger.hpp>

using plugin::gpx::Report;

Report::Report (void) {
}

Report::~Report (void) {
}

void Report::report(const ::gpx::Node *node, ::gpx::Report::Warning warning,  const std::string &extra)
{
	std::string text;
	if (node != nullptr)
	{
		text = (node->getType() == ::gpx::Node::ATTRIBUTE ? "Attribute " : "Element ") + node->getName() + " : ";
	}

	text += Report::text (warning);

	if (!extra.empty())
	{
		text += ": " + extra;
	}

	text + ".";

	NOTICE_MSG ("{:s}\n", text);
}



