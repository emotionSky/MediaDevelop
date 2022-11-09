#include "BaseSip/internal/SipMessage.h"

namespace sip
{
	void* SipMalloc(int len)
	{
		return osip_malloc(len);
	}

	void SipFree(void* buf)
	{
		osip_free(buf);
	}
}
