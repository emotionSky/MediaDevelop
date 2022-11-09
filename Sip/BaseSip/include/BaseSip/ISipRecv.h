#ifndef __I_SIP_RECV_H__
#define __I_SIP_RECV_H__

#include "BaseSip/SipDefine.h"
#include <string>

namespace sip
{
	struct SipMsgInfo
	{
		/* event */
		int m_tid; /**< unique id for transactions (to be used for answers) */
		int m_did; /**< unique id for SIP dialogs */
		int m_rid; /**< unique id for registration */
		int m_cid; /**< unique id for SIP calls (but multiple dialogs!) */
		int m_sid; /**< unique id for outgoing subscriptions */
		int m_nid; /**< unique id for incoming subscriptions */
		int m_ssStatus; /**< current Subscription-State for subscription */
		int m_ssReason; /**< current Reason status for subscription */

		/* method */
		int m_expires;
		int m_statusCode;
		std::string m_fromUser;
		std::string m_fromHost;
		int m_fromPort;
		std::string m_toUser;
		std::string m_toHost;
		int m_toPort;
		std::string m_callid;
		std::string m_method;
	};

	class ISipRecv
	{
	public:
		virtual int OnRecvRegister(const SipMsgInfo& info) = 0;
		virtual int OnRecvRegisterResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvUnRegister(const SipMsgInfo& info) = 0;
		virtual int OnRecvOption(const SipMsgInfo& info) = 0;
		virtual int OnRecvInvite(const SipMsgInfo& info) = 0;
		virtual int OnRecvInviteResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvReinvite(const SipMsgInfo& info) = 0;
		virtual int OnRecvAck(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallCancel(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallTimeout(const SipMsgInfo& info) = 0;
		virtual int OnRecvBye(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallRelease(const SipMsgInfo& info) = 0;
		virtual int OnRecvUpdate(const SipMsgInfo& info) = 0;
		virtual int OnRecvUpdateResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvMessage(const SipMsgInfo& info) = 0;
		virtual int OnRecvMessageResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvInfo(const SipMsgInfo& info) = 0;
		virtual int OnRecvInfoResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallMessage(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallMessageResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallInfo(const SipMsgInfo& info) = 0;
		virtual int OnRecvCallInfoResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvSubscribe(const SipMsgInfo& info) = 0;
		virtual int OnRecvSubScribeTimeout(const SipMsgInfo& info) = 0;
		virtual int OnRecvSubscribeResponse(const SipMsgInfo& info) = 0;
		virtual int OnRecvNotify(const SipMsgInfo& info) = 0;
		virtual int OnRecvNotifyTimeout(const SipMsgInfo& info) = 0;
		virtual int OnRecvNotifyResponse(const SipMsgInfo& info) = 0;
	};
}

#endif // !__I_SIP_RECV_H__

