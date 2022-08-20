#ifndef _LIBGPSFILE2_TYPES_PLUGINDETAILS_
#define _LIBGPSFILE2_TYPES_PLUGINDETAILS_

namespace libgpsfile2 {
	inline namespace types {

		struct PluginDetails {
			const char *name;
			const char *author;
			const char *license;
			uint8_t major_version;
			uint8_t minor_version;
		};
	}
}

#endif /* _LIBGPSFILE2_TYPES_PLUGINDETAILS_ */
