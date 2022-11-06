#ifndef __SIP_MESSAGE_HELPER_H__
#define __SIP_MESSAGE_HELPER_H__

#include <cstring>
#include <cstdio>

namespace sip
{
	static inline int CompareString(const char* str1, const char* str2)
	{
		return strcmp(str1, str2);
	}

	static inline int CompareNumString(const char* str1, const char* str2, size_t n)
	{
		return strncmp(str1, str2, n);
	}

	static inline int CompareStringCase(const char* str1, const char* str2)
	{
#ifdef WIN32
		return _stricmp(str1, str2);
#else
		return strcasecmp(str1, str2);
#endif
	}

	static inline int CompareNumStringCase(const char* str1, const char* str2, size_t n)
	{
#ifdef WIN32
		return _strnicmp(str1, str2, n);
#else
		return strncasecmp(str1, str2, n);
#endif
	}

	/*
	 * @brief ��sip��uri�н���������
	 * @param[in]  str  uri��������from to subject�ȣ����� <sip:alice@10.1.1.21:1234>
	 * @param[out] num  ����
	 * @return void
	 */
	static inline void GetSipNumber(const char* str, char* num)
	{
		if (str && num)
		{
			sscanf(str, "<sip:%[^@]", num);
		}
	}
}

#endif // !__SIP_MESSAGE_HELPER_H__

