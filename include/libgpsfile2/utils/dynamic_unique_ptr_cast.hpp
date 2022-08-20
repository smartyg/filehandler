#ifndef _LIBGPSFILE2_UTILS_DYNAMIC_UNIQUE_PTR_CAST_
#define _LIBGPSFILE2_UTILS_DYNAMIC_UNIQUE_PTR_CAST_

#include <memory>

namespace libgpsfile2::types {
	template<typename Derived, typename Base>
	std::unique_ptr<Derived> dynamic_unique_ptr_cast (std::unique_ptr<Base>&& p) {
		if(Derived *result = dynamic_cast<Derived *>(p.get ())) {
			p.release ();
			return std::unique_ptr<Derived>(result);
		}
		return std::unique_ptr<Derived>(nullptr);
	}
}

#endif /* _LIBGPSFILE2_UTILS_DYNAMIC_UNIQUE_PTR_CAST_ */
