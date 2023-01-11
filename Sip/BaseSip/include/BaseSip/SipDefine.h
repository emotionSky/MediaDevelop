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
#define OSIP_SUCCESS             0   //�ɹ�
#define OSIP_UNDEFINED_ERROR     -1  //δ����Ĵ���
#define OSIP_BADPARAMETER        -2  //��Ч�Ĳ�������
#define OSIP_WRONG_STATE         -3  //״̬����
#define OSIP_NOMEM               -4  //û���ڴ�
#define OSIP_SYNTAXERROR         -5  //�﷨����
#define OSIP_NOTFOUND            -6  //û���ҵ�
#define OSIP_API_NOT_INITIALIZED -7  //û�г�ʼ��
#define OSIP_NO_NETWORK          -10 //û������
#define OSIP_PORT_BUSY           -11 //�˿ڷ�æ
#define OSIP_UNKNOWN_HOST        -12 //δ֪��host
#define OSIP_DISK_FULL           -30 //��������
#define OSIP_NO_RIGHTS           -31 //û��Ȩ��
#define OSIP_FILE_NOT_EXIST      -32 //�ļ�������
#define OSIP_TIMEOUT             -50 //��ʱ
#define OSIP_TOOMUCHCALL         -51 //���й���
#define OSIP_WRONG_FORMAT        -52 //����ĸ�ʽ
#define OSIP_NOCOMMONCODEC       -53 //����ͨ�õĸ�ʽ
#define OSIP_RETRY_LIMIT         -60 //��������
*/

#endif // !__SIP_DEFINE_H__
