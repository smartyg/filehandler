#ifndef _LIBGPSFILE2_TYPES_HANDLERTYPE_
#define _LIBGPSFILE2_TYPES_HANDLERTYPE_

#include <exception>

namespace libgpsfile2 {
	class GpsfilePlugin;

	inline namespace types {
		class HandlerType {
			friend GpsfilePlugin;

		public:
			uint8_t _value;
			enum :uint8_t {
				UNKNOWN = 0,
				READER = 1,
				WRITER = 2
			} _type = UNKNOWN;
			std::size_t _hash;

			void setValue (uint8_t value, uint8_t type, std::size_t hash) {
				if (this->_type == UNKNOWN || this->_type == type) {
					this->_value = value;
					this->_type = static_cast<decltype(this->_type)>(type);
					this->_hash = hash;
				} else
					throw std::runtime_error ("types do not match");
			}

		public:
			bool operator== (const HandlerType& rhs) const noexcept {
				return (this->_value == rhs._value &&
				this->_type == rhs._type &&
				this->_hash == rhs._hash);
			}

			explicit operator bool() const noexcept {
				return this->valid ();
			}

			explicit operator int() const noexcept {
				return static_cast<int>(this->_value);
			}

			bool valid (void) const noexcept {
				return (!(this->_type == UNKNOWN));
			}

			uint8_t getValue (void) const noexcept {
				return this->_value;
			}

			uint8_t getType (void) const noexcept {
				return static_cast<uint8_t>(this->_type);
			}

			std::size_t getHash (void) const noexcept {
				return this->_hash;
			}
		};
	}
}

#endif /* _LIBGPSFILE2_TYPES_HANDLERTYPE_ */