#ifndef _PLUGIN_OPENAMBIT_PLUGIN_
#define _PLUGIN_OPENAMBIT_PLUGIN_

#include <filehandler/types/BasePlugin.hpp>

namespace plugin::openambit {

class Plugin : public filehandler::BasePlugin {

public:
	Plugin (void);
	~Plugin (void);
};
}

#endif /* _PLUGIN_OPENAMBIT_PLUGIN_ */
