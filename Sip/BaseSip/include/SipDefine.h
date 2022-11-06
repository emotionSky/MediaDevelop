#ifndef __SIP_DEFINE_H__

#define PRINT_MACRO_HELPER(x)  #x
#define PRINT_MACRO(x)         #x "=" PRINT_MACRO_HELPER(x)

#if defined WIN32 || defined _WINDLL || defined __CYGWIN__
	#if defined BASESIP_EXPORTS
		#if defined __GNUC__ || defined __clang__
			#pragma message("log exports in gnuc-win.")
			#define BASESIP_API           __attribute__ ((dllexport))
		#elif defined _MSC_VER
			#pragma message("log exports in msvc-win.")
			#define BASESIP_API           __declspec(dllexport)
		#else
			#define BASESIP_API
		#endif
	#else
		#if defined                        __GNUC__ || defined __clang__
			#define BASESIP_API           __attribute__ ((dllimport))
		#elif defined _MSC_VER
			#define BASESIP_API           __declspec(dllimport)
		#else
			#define BASESIP_API
		#endif
	#endif
#else
	#if defined BASESIP_EXPORTS
		#if                                __GNUC__ >= 4 || defined __clang__
			#pragma message("log exports in gnuc-unix.")
			#define BASESIP_API           __attribute__((visibility ("default")))
		#else
			#define BASESIP_API
		#endif
	#else
		#define BASESIP_API
	#endif
#endif

#define SIP_ERROR   -1
#define SIP_SUCCESS 0

#endif // !__SIP_DEFINE_H__
