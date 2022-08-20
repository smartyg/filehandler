#ifndef _X_GPSPLUGIN_HPP_
#define _X_GPSPLUGIN_HPP_

#include "libgpsfile2/types/BasePlugin.hpp"

class GpxPlugin : public libgpsfile2::BasePlugin {
public:
	GpxPlugin (void);
	~GpxPlugin (void);
};

#endif /* _X_GPSPLUGIN_HPP_ */
