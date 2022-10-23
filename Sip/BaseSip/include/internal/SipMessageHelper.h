#ifndef __SIP_MESSAGE_HELPER_H__
#define __SIP_MESSAGE_HELPER_H__

#include <cstring>
#include <cstdio>

namespace sip
{
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

