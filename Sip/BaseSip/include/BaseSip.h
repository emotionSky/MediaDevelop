#ifndef __BASE_SIP_H__
#define __BASE_SIP_H__

#include "ISipRecv.h"
#include <string>

namespace sip
{
	class BaseData;
	class BASESIP_API BaseSip
	{
	private:
		BaseSip();

	public:
		~BaseSip();
		static BaseSip* Instance();
		static void Destroy();

		bool Init(const char* host, int port, const char* agent, ISipRecv* pCb);
		bool Init(const char* host, int port, const char* agent, int family, bool bIsUdp, bool bIsSecure, ISipRecv* pCb);
		void DealCallBack(void* pMsg);
		void Release();
		void StartSip();
		void StopSip();
		void Run();

	private:
		void DealEvt(void* pEvt);		
		void DealMessage(const char* method, void* pEvt);
		void DealMessageResponse(const char* method, void* pEvt);
		void DealCallMessage(const char* methed, void* pEvt);
		void DealCallMessageResponse(const char* method, void* pEvt);
		void DealSubscribeTimeout(const char* method, void* pEvt);
		void DealSubscribeResponse(const char* method, void* pEvt);

	private:
		ISipRecv* m_pCb;
		BaseData* m_pData;
	};
}

#endif // !__BASE_SIP_H__
