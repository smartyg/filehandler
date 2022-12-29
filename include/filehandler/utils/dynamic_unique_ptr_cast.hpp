#ifndef _FILEHANDLER_UTILS_DYNAMIC_UNIQUE_PTR_CAST_
#define _FILEHANDLER_UTILS_DYNAMIC_UNIQUE_PTR_CAST_

#include <memory>

namespace filehandler::utils {
	template<typename Derived, typename Base>
	std::unique_ptr<Derived> dynamic_unique_ptr_cast (std::unique_ptr<Base>&& p) {
		if(Derived *result = dynamic_cast<Derived *>(p.get ())) {
			p.release ();
			return std::unique_ptr<Derived>(result);
		}
		return std::unique_ptr<Derived>(nullptr);
	}
}

#endif /* _FILEHANDLER_UTILS_DYNAMIC_UNIQUE_PTR_CAST_ */
