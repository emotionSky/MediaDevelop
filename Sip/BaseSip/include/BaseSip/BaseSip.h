#ifndef __BASE_SIP_H__
#define __BASE_SIP_H__

#include "BaseSip/ISipRecv.h"
#include "BaseSip/ESipSend.h"
#include "BaseSip/BaseSipInfos.h"
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

		bool Init(const char* host, int port, const char* agent, ISipRecv* pCb, bool bIsOnlyHost = false);
		bool Init(const char* host, int port, const char* agent, int family, bool bIsUdp, bool bIsSecure, ISipRecv* pCb, bool bIsOnlyHost = false);
		void DealCallBack(void* pMsg);
		void Release();
		void StartSip();
		void StopSip();
		void Run();

		int SendRegister(BaseSipInfos& p);
		int RefreshRegister(int rid, int expires);
		int UpdateRegisterPwd(const char* user, const char* newPwd);
		int SendRegisterResponse(int tid, int statusCode);
		int SendOptions(BaseSipInfos& p);
		int SendInvite(BaseSipInfos& p);
		int SendTrying(const BaseSipInfos& p);
		int SendRinging(const BaseSipInfos& p);
		int SendInviteResponse(const BaseSipInfos& p);
		int SendAck(const BaseSipInfos& p);
		int SendUpdate(const BaseSipInfos& p);
		int SendMessage(const BaseSipInfos& p);
		int SendCallOptions(const BaseSipInfos& p);
		int SendCallMessage(const BaseSipInfos& p);
		int SendCallInfo(const BaseSipInfos& p);
		int SendSubscribe(BaseSipInfos& p);
		int SendSubscribeResponse(int tid, int statusCode);
		int RemoveRegister(int rid);
		int RemoveSubscribe(int sid); //outgoing
		int RemoveInSubscribe(int nid); //incoming

	private:
		int SendMessageMethod(const BaseSipInfos& p, const char* method);
		int SendCallMessageMethod(const BaseSipInfos& p, const char* method);
		
	private:
		void DealEvt(void* pEvt);		
		void DealMessage(const char* method, void* pEvt);
		void DealMessageResponse(const char* method, void* pEvt);
		void DealRegister(void* pEvt);
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
