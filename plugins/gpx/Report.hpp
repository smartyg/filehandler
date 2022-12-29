#ifndef _PLUGIN_GPX_REPORT_
#define _PLUGIN_GPX_REPORT_

#include <gpx/Report.h>

///
/// @class ReportCerr
///
/// @brief The report on cerr class for reporting warnings
///

namespace plugin::gpx {

	class Report : public ::gpx::Report
	{
	public:

		///
		/// Constructor
		///
		Report (void);

		///
		/// Deconstructor
		///
		virtual ~Report (void);

		///
		/// Report a warning for a node
		///
		/// @param  node    the node for with the warning (can be 0)
		/// @param  warning the warning
		/// @param  extra   the extra information
		///
		void report (const ::gpx::Node *node, ::gpx::Report::Warning warning, const std::string& extra) override;
	};
}

#endif /* _PLUGIN_GPX_REPORT_ */