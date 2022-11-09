#ifndef __BASE_SIP_H__
#define __BASE_SIP_H__

#include "BaseSip/ISipRecv.h"
#include "BaseSip/ESipSend.h"
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

		int SendRegister(ESipRegisterParams& params);
		int RefreshRegister(int rid, int expires);
		int UpdateRegisterPwd(const char* user, const char* newPwd);
		int SendRegisterResponse(int tid, int statusCode);
		int SendOptions(ESipMessageParams& params);
		int SendInvite(ESipCallParams& params);
		int SendRinging(ESipCallResponseParams& params);
		int SendInviteResponse(ESipCallResponseParams& params);
		int SendAck(ESipCallParams& params);
		int SendUpdate(ESipCallParams& params);
		int SendMessage(ESipMessageParams& params);
		int SendInfo(ESipMessageParams& params);
		int SendCallOptions(ESipCallMessageParams& params);
		int SendCallMessage(ESipCallMessageParams& params);
		int SendCallInfo(ESipCallMessageParams& params);
		int SendSubscribe(ESipSuscribeParams& params);
		int SendSubscribeResponse(int tid, int statusCode);
		int RemoveRegister(int rid);
		int RemoveSubscribe(int did); //outgoing
		int RemoveInSubscribe(int did); //incoming

	private:
		int SendMessageMethod(ESipMessageParams& params, const char* method);
		int SendCallMessageMethod(ESipCallMessageParams& params, const char* method);
		
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
