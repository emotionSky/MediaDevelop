#ifndef __SIP_MESSAGE_HELPER_H__
#define __SIP_MESSAGE_HELPER_H__

#include <string>
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

	static inline std::string GetUserInUri(const char* uri)
	{
		//uri   <sip:alice@10.10.1.12:5060>
		std::string user;
		if (uri)
		{
			char buf[64] = { 0 };
			int ret = sscanf(uri, "<sip:%[^@]", buf);

			if (ret == 1)
				user = buf;
		}
		return user;
	}

	static inline std::string GetUserInString(const char* str)
	{
		if (!str || str[0] == '\0')
			return "";
		//str alice@10.10.1.12:5060
		std::string user;
		char buf[64] = { 0 };
		if (sscanf(str, "%[^@]", buf) == 1)
			user = buf;
		return user;
	}

	static inline bool GetSipInfoInString(const char* str, char* user, char* host, int& port)
	{
		//str alice@10.10.1.12:5060
		if (str)
		{
			int ret = sscanf(str, "%[^@]@%[^:]:%d", user, host, &port);
			if (ret == 3)
				return true;
		}
		return false;
	}
}

#endif // !__SIP_MESSAGE_HELPER_H__

