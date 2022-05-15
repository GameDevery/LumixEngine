#pragma once

#include <engine/lumix.h>

# define EE_API LUMIX_ENGINE_API

namespace EE {
#ifdef EE_USE_DOUBLES
typedef double Float;
	#define eesqrt sqrt
	#define eesin sin
	#define eecos cos
	#define eetan tan
	#define eeacos acos
	#define eeatan2 atan2
	#define eemod fmod
	#define eeexp exp
	#define eepow pow
	#define eefloor floor
	#define eeceil ceil
	#define eeabs abs
#else
typedef float Float; //! The internal floating point used on EE++. \n This can help to improve
					 //! compatibility with some platforms. \n And helps for an easy change from
					 //! single precision to double precision.
	#define eesqrt sqrtf
	#define eesin sinf
	#define eecos cosf
	#define eetan tanf
	#define eeacos acosf
	#define eeatan2 atan2f
	#define eemod fmodf
	#define eeexp expf
	#define eepow powf
	#define eefloor floorf
	#define eeceil ceilf
	#define eeabs fabs
#endif

template <typename T> T eemax(T a, T b) {
	return (a > b) ? a : b;
}

template <typename T> T eemin(T a, T b) {
	return (a < b) ? a : b;
}

template <typename T> T eeabs(T n) {
	return (n < 0) ? -n : n;
}

template <typename T> T eeclamp(T val, T min, T max) {
	return (val < min) ? min : ((val > max) ? max : val);
}

template <typename T> void eeclamp(T* val, T min, T max) {
	(*val < min) ? * val = min : ((*val > max) ? * val = max : *val);
}

using Int8 = Lumix::i8;
using Uint8 = Lumix::u8;
using Int16 = Lumix::i16;
using Uint16 = Lumix::u16;
using Int32 = Lumix::i32;
using Uint32 = Lumix::u32;
using Int64 = Lumix::i64;
using Uint64 = Lumix::u64;

//using IntPtr = Int32;
using UintPtr = Lumix::uintptr;

#define EE_PI (3.14159265358979323846)
#define EE_PI2 (6.28318530717958647692)
#define EE_PI_180 ((Float)EE_PI / 180.f)
#define EE_PI_360 ((Float)EE_PI / 360.f)
#define EE_180_PI (180.f / (Float)EE_PI)
#define EE_360_PI (360.f / (Float)EE_PI)
#define EE_1B (1)
#define EE_1KB (1024)
#define EE_1MB (1048576)
#define EE_1GB (1073741824)
#define EE_1TB (1099511627776)

} // namespace EE
