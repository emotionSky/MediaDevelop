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
		 * @brief ��ʼ��SIP
		 * @param[in] host  SIPʹ�õ�ip�����Ϊ��ָ�룬��ô������������
		 * @param[in] port  SIPʹ�õĶ˿�
		 * @param[in] agent SIP��agent
		 * @param[in] pCb   SIP�¼��Ļص�����
		 * @return ��ʼ���ɹ��򷵻�true����ʼ��ʧ���򷵻�false
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
