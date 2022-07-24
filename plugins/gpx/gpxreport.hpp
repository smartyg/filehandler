#ifndef _X_GPXREPORT_HPP_
#define _X_GPXREPORT_HPP_

#include <gpx/Report.h>

///
/// @class ReportCerr
///
/// @brief The report on cerr class for reporting warnings
///

class GpxReport : public gpx::Report
{
public:

	///
	/// Constructor
	///
	GpxReport();

	///
	/// Deconstructor
	///
	virtual ~GpxReport (void);

	///
	/// Report a warning for a node
	///
	/// @param  node    the node for with the warning (can be 0)
	/// @param  warning the warning
	/// @param  extra   the extra information
	///
	virtual void report (const gpx::Node *node, gpx::Report::Warning warning, const std::string& extra);
};

#endif /* _X_GPXREPORT_HPP_ */

