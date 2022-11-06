#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

#define PRINT_MACRO_HELPER(x)  #x
#define PRINT_MACRO(x)         #x "=" PRINT_MACRO_HELPER(x)

#if defined WIN32 || defined _WINDLL || defined __CYGWIN__
	#if defined DREAMSKY_EXPORTS
		#if defined __GNUC__ || defined __clang__
			#pragma message("log exports in gnuc-win.")
			#define DREAMSKY_API           __attribute__ ((dllexport))
		#elif defined _MSC_VER
			#pragma message("log exports in msvc-win.")
			#define DREAMSKY_API           __declspec(dllexport)
		#else
			#define DREAMSKY_API
		#endif
	#else
		#if defined                        __GNUC__ || defined __clang__
			#define DREAMSKY_API           __attribute__ ((dllimport))
		#elif defined _MSC_VER
			#define DREAMSKY_API           __declspec(dllimport)
		#else
			#define DREAMSKY_API
		#endif
	#endif
#else
	#if defined DREAMSKY_EXPORTS
		#if                                __GNUC__ >= 4 || defined __clang__
			#pragma message("log exports in gnuc-unix.")
			#define DREAMSKY_API           __attribute__((visibility ("default")))
		#else
			#define DREAMSKY_API
		#endif
	#else
		#define DREAMSKY_API
	#endif
#endif

#if defined __GNUC__ || defined __clang__
	#pragma message("log exports in gnuc-win.")
	#define DREAMSKY_API           __attribute__ ((dllexport))
	#define DREAMSKY_UNUSED        __attribute__((unused))
	#define DREAMSKY_CONST         __attribute__((const))
	#define DREAMSKY_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined _MSC_VER
	#pragma message("log exports in msvc-win.")
	#define DREAMSKY_API           __declspec(dllexport)
	#define DREAMSKY_UNUSED
	#define DREAMSKY_CONST
	#define DREAMSKY_ALWAYS_INLINE __forceinline
#else
	#define DREAMSKY_API
	#define DREAMSKY_UNUSED
	#define DREAMSKY_CONST
	#define DREAMSKY_ALWAYS_INLINE inline
#endif


#endif