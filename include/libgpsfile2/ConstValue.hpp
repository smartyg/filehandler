#ifndef _LIBGPSFILE2_CONSTVALUE_
#define _LIBGPSFILE2_CONSTVALUE_

#include <exception>

//#include <libgpsfile2/GpsfilePlugin.hpp>

namespace libgpsfile2 {
	class GpsfilePlugin;

	template<typename I>
	class ConstValue {
		friend GpsfilePlugin;

	public:
		using DataType = I;

		DataType _value;
		std::size_t _type_hash;
		bool _valid = false;

		template<class E>
		void setValue (const DataType& value) {
			if (!this->_valid) {
				this->_type_hash = typeid (E).hash_code ();
				this->_valid = true;
			}

			if (this->_type_hash == typeid (E).hash_code ())
				this->_value = value;
			else
				throw std::runtime_error ("types do not match");
		}

	public:
		bool operator== (const ConstValue& rhs) const noexcept {
			return (this->_type_hash == rhs._type_hash &&
			this->_value == rhs._value);
		}

		explicit operator bool() const noexcept {
			return this->_valid;
		}

		explicit operator int() const noexcept {
			return this->_value;
		}

		explicit operator DataType() const noexcept {
			return this->_value;
		}

		std::size_t getTypeHash (void) const noexcept {
			return this->_type_hash;
		}

		template<class T>
		bool isSameType (void) const noexcept {
			return this->_type_hash == typeid (T).hash_code ();
		}
	};
}

#endif /* _LIBGPSFILE2_CONSTVALUE_ */