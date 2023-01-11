#ifndef __SIP_DEFINE_H__
#define __SIP_DEFINE_H__

#define PRINT_MACRO_HELPER(x)  #x
#define PRINT_MACRO(x)         #x "=" PRINT_MACRO_HELPER(x)

#if defined WIN32 || defined _WINDLL || defined __CYGWIN__
	#if defined DREAMSKY_EXPORTS
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
		#if defined                       __GNUC__ || defined __clang__
			#define BASESIP_API           __attribute__ ((dllimport))
		#elif defined _MSC_VER
			#define BASESIP_API           __declspec(dllimport)
		#else
			#define BASESIP_API
		#endif
	#endif
#else
	#if defined DREAMSKY_EXPORTS
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

/*
#define OSIP_SUCCESS             0   //成功
#define OSIP_UNDEFINED_ERROR     -1  //未定义的错误
#define OSIP_BADPARAMETER        -2  //无效的参数错误
#define OSIP_WRONG_STATE         -3  //状态错误
#define OSIP_NOMEM               -4  //没有内存
#define OSIP_SYNTAXERROR         -5  //语法错误
#define OSIP_NOTFOUND            -6  //没有找到
#define OSIP_API_NOT_INITIALIZED -7  //没有初始化
#define OSIP_NO_NETWORK          -10 //没有网络
#define OSIP_PORT_BUSY           -11 //端口繁忙
#define OSIP_UNKNOWN_HOST        -12 //未知的host
#define OSIP_DISK_FULL           -30 //磁盘已满
#define OSIP_NO_RIGHTS           -31 //没有权限
#define OSIP_FILE_NOT_EXIST      -32 //文件不存在
#define OSIP_TIMEOUT             -50 //超时
#define OSIP_TOOMUCHCALL         -51 //呼叫过多
#define OSIP_WRONG_FORMAT        -52 //错误的格式
#define OSIP_NOCOMMONCODEC       -53 //不是通用的格式
#define OSIP_RETRY_LIMIT         -60 //限制重试
*/

#endif // !__SIP_DEFINE_H__
