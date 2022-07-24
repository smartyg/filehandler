#ifndef _X_GPSPLUGIN_HPP_
#define _X_GPSPLUGIN_HPP_

#include <memory>

#include "libgpsfile2.hpp"

class GpxPlugin : public libgpsfile2::BasePlugin {
public:
	GpxPlugin (void);
	~GpxPlugin (void);
};

//typedef std::shared_ptr<const GpxPlugin> GpxPlugin_ptr_const;

#endif /* _X_GPSPLUGIN_HPP_ */
