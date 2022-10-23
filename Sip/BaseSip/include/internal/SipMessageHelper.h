#ifndef __SIP_MESSAGE_HELPER_H__
#define __SIP_MESSAGE_HELPER_H__

#include <cstring>
#include <cstdio>

namespace sip
{
	/*
	 * @brief 从sip的uri中解析出号码
	 * @param[in]  str  uri，可能是from to subject等，形如 <sip:alice@10.1.1.21:1234>
	 * @param[out] num  号码
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

