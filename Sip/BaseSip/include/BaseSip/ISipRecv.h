#ifndef __I_SIP_RECV_H__
#define __I_SIP_RECV_H__

#include "BaseSip/SipDefine.h"
#include "BaseSip/BaseSipInfos.h"

namespace sip
{
	class ISipRecv
	{
	public:
		virtual int OnRecvRegister(const BaseSipInfos& p) = 0;
		virtual int OnRecvRegisterResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvUnRegister(const BaseSipInfos& p) = 0;
		virtual int OnRecvOption(const BaseSipInfos& p) = 0;
		virtual int OnRecvInvite(const BaseSipInfos& p) = 0;
		virtual int OnRecvInviteResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvReinvite(const BaseSipInfos& p) = 0;
		virtual int OnRecvAck(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallCancel(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallTimeout(const BaseSipInfos& p) = 0;
		virtual int OnRecvBye(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallRelease(const BaseSipInfos& p) = 0;
		virtual int OnRecvUpdate(const BaseSipInfos& p) = 0;
		virtual int OnRecvUpdateResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvMessage(const BaseSipInfos& p) = 0;
		virtual int OnRecvMessageResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvInfo(const BaseSipInfos& p) = 0;
		virtual int OnRecvInfoResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallMessage(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallMessageResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallInfo(const BaseSipInfos& p) = 0;
		virtual int OnRecvCallInfoResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvSubscribe(const BaseSipInfos& p) = 0;
		virtual int OnRecvSubScribeTimeout(const BaseSipInfos& p) = 0;
		virtual int OnRecvSubscribeResponse(const BaseSipInfos& p) = 0;
		virtual int OnRecvNotify(const BaseSipInfos& p) = 0;
		virtual int OnRecvNotifyTimeout(const BaseSipInfos& p) = 0;
		virtual int OnRecvNotifyResponse(const BaseSipInfos& p) = 0;
	};
}

#endif // !__I_SIP_RECV_H__

