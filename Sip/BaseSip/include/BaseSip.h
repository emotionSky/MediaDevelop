#ifndef __BASE_SIP_H__
#define __BASE_SIP_H__

#include "ISipRecv.h"
#include "ESipSend.h"
#include <string>

template class __declspec(dllexport) std::basic_string<char, std::char_traits<char>, std::allocator<char> >;

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

		/**
		 * @brief 初始化SIP
		 * @param[in] host  SIP使用的ip，如果为空指针，那么将绑定所有网卡
		 * @param[in] port  SIP使用的端口
		 * @param[in] agent SIP的agent
		 * @param[in] pCb   SIP事件的回调对象
		 * @return 初始化成功则返回true，初始化失败则返回false
		 */
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
		int SendInvite(ESipCallParams& params);
		int SendRinging(ESipCallResponseParams& params);
		
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
		std::string m_info;
	};
}

#endif // !__BASE_SIP_H__
