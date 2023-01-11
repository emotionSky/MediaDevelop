#include "BaseSip/internal/SipMessage.h"
#include "BaseSip/internal/SipMessageHelper.h"
#include "BaseSip/internal/SipSend.h"
#include <eXosip2/eXosip.h>

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#else
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <thread>
#include <Log/log_core.h>
#include "BaseSip/BaseSip.h"

namespace sip
{
	class BaseData
	{
	public:
		BaseData() : m_bRunning(false), m_pThread(nullptr), m_pCtx(nullptr) 
		{
			m_localAddr = "127.0.0.0:0";
		}
		bool             m_bRunning;
		std::string      m_localAddr;
		std::thread*     m_pThread;
		struct eXosip_t* m_pCtx;
	};

	class SipLock
	{
	public:
		SipLock(struct eXosip_t* pCtx, bool isLock = true) : 
			m_pCtx(pCtx), m_bIsLock(isLock)
		{
			if (m_bIsLock)
			{
				eXosip_lock(m_pCtx);
			}
		}

		~SipLock()
		{
			if (m_bIsLock)
			{
				eXosip_unlock(m_pCtx);
				m_bIsLock = false;
			}
		}

		void Lock()
		{
			if (!m_bIsLock)
			{
				eXosip_lock(m_pCtx);
				m_bIsLock = true;
			}
		}

		void Unlock()
		{
			if (m_bIsLock)
			{
				eXosip_unlock(m_pCtx);
				m_bIsLock = false;
			}
		}

	private:
		bool m_bIsLock;
		struct eXosip_t* m_pCtx;
	};

	BaseSip::BaseSip()
	{
		m_pCb = nullptr;
		m_pData = new BaseData();
	}

	BaseSip::~BaseSip()
	{
		m_pCb = nullptr;
		StopSip();
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	static BaseSip* g_pSip = nullptr;
	BaseSip* BaseSip::Instance()
	{
		if (g_pSip == nullptr)
			g_pSip = new BaseSip();

		return g_pSip;
	}

	void BaseSip::Destroy()
	{
		if (g_pSip)
		{
			delete g_pSip;
			g_pSip = nullptr;
		}
	}

	static void SipMessageCallback(osip_message_t* msg, int received)
	{
		std::string method = GetMethod(msg);
		if (msg && msg->cseq)
		{
			if (received)
			{
				BaseSip::Instance()->DealCallBack(msg);
			}
		}		
	}

	bool BaseSip::Init(const char* host, int port, const char* agent, ISipRecv* pCb, bool bIsOnlyHost)
	{
		return Init(host, port, agent, AF_INET, true, false, pCb, bIsOnlyHost);
	}

	bool BaseSip::Init(const char* host, int port, const char* agent, int family, bool bIsUdp, bool bIsSecure, ISipRecv* pCb, bool bIsOnlyHost)
	{
		if (m_pData->m_pCtx != nullptr)
		{
			log_printf(LOG_NOTICE, "Init BaseSip while it is inited.");
			return true;
		}

		if (port <= 0 || port > 65535 || host == nullptr || host[0] == '\0')
			return false;

		bool ret = false;
		int transport = bIsUdp ? IPPROTO_UDP : IPPROTO_TCP;
		int secure = bIsSecure ? 1 : 0;
		do
		{
			m_pData->m_pCtx = eXosip_malloc(); //实际上就是 osip_malloc

			/* 初始化 exosip */
			if (eXosip_init(m_pData->m_pCtx) != OSIP_SUCCESS)
				break;

			/* 通讯协议及端口监听 */
			if (eXosip_listen_addr(m_pData->m_pCtx, transport, (bIsOnlyHost ? host : nullptr), port, family, secure) != OSIP_SUCCESS)
			{
				eXosip_quit(m_pData->m_pCtx);
				break;
			}

			eXosip_set_cbsip_message(m_pData->m_pCtx, SipMessageCallback);
			
			if(agent)
				eXosip_set_user_agent(m_pData->m_pCtx, agent);

			m_pData->m_localAddr = std::string(host) + ":" + std::to_string(port);
			m_pCb = pCb;
			ret = true;
			break;
		} while (true);

		if (!ret)
		{
			osip_free(m_pData->m_pCtx);
			m_pData->m_pCtx = nullptr;
		}

		return ret;
	}

	void BaseSip::DealCallBack(void* pMsg)
	{
		osip_message_t* msg = static_cast<osip_message_t*>(pMsg);
		std::string method = GetMethod(msg);

		//is response
		if (!msg->sip_method)
		{
			if (method == "REGISTER")
			{
				//register response
			}
		}
		else
		{
			if (method == "SUBSCRIBE")
			{
				//subscribe refresh
			}
		}
	}

	void BaseSip::Release()
	{

	}

	void BaseSip::StartSip()
	{
		if (m_pData->m_bRunning)
			return;

		m_pData->m_bRunning = true;
		m_pData->m_pThread = new std::thread(&BaseSip::Run, this);
	}

	void BaseSip::StopSip()
	{
		m_pData->m_bRunning = false;
		if (m_pData->m_pThread != nullptr)
		{
			m_pData->m_pThread->join();
			delete m_pData->m_pThread;
			m_pData->m_pThread = nullptr;
		}
	}

	void BaseSip::Run()
	{
		/* thread worker */
		eXosip_event_t* evt = nullptr;
		if (m_pData == nullptr || m_pData->m_pCtx == nullptr)
		{
			log_printf(LOG_ERROR, "Cannot run sip while ctx not inited.");
			return;
		}

		SipLock lock(m_pData->m_pCtx, false);
		while (m_pData->m_bRunning)
		{
			//设置exosip的超时时间，此处设置为50ms
			evt = eXosip_event_wait(m_pData->m_pCtx, 0, 50);
			if (evt)
			{
				lock.Lock();
				eXosip_automatic_action(m_pData->m_pCtx);
				//eXosip_automatic_refresh(m_pData->m_pCtx);
				lock.Unlock();

				DealEvt(evt);
				evt = nullptr;
			}
		}
	}

	int BaseSip::SendRegister(BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& pwd = p.GetInfosParams(SIP_INFO_REGISTER_PWD, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);

		if (from.empty() || route.empty() || pwd.empty())
			return OSIP_BADPARAMETER;

		auto& contact = p.GetInfosParams(SIP_INFO_CONTACT, "");
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		osip_message_t* reg = nullptr;
		std::string user = GetUserInString(from.c_str());
		int expires = std::stoi(p.GetInfosParams(SIP_INFO_EXPIRES, "0"));
		auto sip_from = std::string("sip:") + from;
		auto sip_route = std::string("sip:") + route;

		std::string contact_str;
		const char* sip_contact = nullptr;
		if (!contact.empty())
		{
			contact_str = std::string("sip:") + contact;
			sip_contact = contact_str.c_str();
		}		

		SipLock lock(m_pData->m_pCtx);
		/* 可能存在注册失败，然后添加代理处进行了密码的修改，这里就需要先删除原有的认证信息，然后替换修改后的认证信息 */
		eXosip_remove_authentication_info(m_pData->m_pCtx, user.c_str(), nullptr);
		eXosip_add_authentication_info(m_pData->m_pCtx, user.c_str(), user.c_str(), pwd.c_str(), "MD5", nullptr);
		int rid = eXosip_register_build_initial_register(m_pData->m_pCtx,
			sip_from.c_str(),
			route.c_str(),
			sip_contact,
			expires,
			&reg);

		if (rid > 0)
		{
			SetVia(reg, sip_via.c_str());
			int ret = eXosip_register_send_register(m_pData->m_pCtx, rid, reg);

			if (ret == OSIP_SUCCESS)
			{
				p.SetInfosParams(SIP_INFO_CALLID, GetCallidNumber(reg));
				p.SetInfosParams(SIP_INFO_RID, std::to_string(rid));
				return OSIP_SUCCESS;
			}
		}

		return OSIP_UNDEFINED_ERROR;
	}

	int BaseSip::RefreshRegister(int rid, int expires)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		osip_message_t* reg = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_register_build_register(m_pData->m_pCtx, rid, expires, &reg);
		if (ret == OSIP_SUCCESS)
		{
			return eXosip_register_send_register(m_pData->m_pCtx, rid, reg);
		}

		return ret;
	}

	int BaseSip::UpdateRegisterPwd(const char* user, const char* newPwd)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		SipLock lock(m_pData->m_pCtx);
		eXosip_remove_authentication_info(m_pData->m_pCtx, user, nullptr);
		eXosip_add_authentication_info(m_pData->m_pCtx, user, user, newPwd, "MD5", nullptr);

		return OSIP_SUCCESS;
	}

	int BaseSip::SendRegisterResponse(int tid, int statusCode)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		SipLock lock(m_pData->m_pCtx);
		return eXosip_message_send_answer(m_pData->m_pCtx, tid, statusCode, nullptr);
	}

	int BaseSip::SendOptions(BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);
		
		if (from.empty() || to.empty())
			return OSIP_BADPARAMETER;

		std::string sip_from = std::string("sip:") + from;
		std::string sip_to = std::string("sip:") + to;
		std::string sip_route = std::string("sip:") + route;
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);

		osip_message_t* options = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_options_build_request(m_pData->m_pCtx,
			&options,
			sip_to.c_str(),
			sip_from.c_str(),
			sip_route.c_str());
		
		if (ret == OSIP_SUCCESS)
		{ 
			SetVia(options, sip_via.c_str());
			SetContact(options, sip_contact.c_str());
			return eXosip_options_send_request(m_pData->m_pCtx, options);
		}
		
		return ret;
	}

	int BaseSip::SendInvite(BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);
		auto& subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");
		auto& refresher = p.GetInfosParams(SIP_INFO_REFRESHER, "");
		int expires = std::stoi(p.GetInfosParams(SIP_INFO_EXPIRES, "0"));

		if (from.empty() || to.empty() || refresher.empty() || expires <= 0)
			return OSIP_BADPARAMETER;

		std::string sip_from = std::string("sip:") + from;
		std::string sip_to = std::string("sip:") + to;
		std::string sip_route = std::string("sip:") + route;
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		const char* sip_subject = subject.empty() ? nullptr : subject.c_str();
		auto& headers = p.GetHeaders();
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& sdp = p.GetInfosParams(SIP_INFO_CONTENT, "");

		osip_message_t* invite = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_initial_invite(m_pData->m_pCtx,
			&invite,
			sip_to.c_str(),
			sip_from.c_str(),
			sip_route.c_str(),
			sip_subject);

		if (ret == OSIP_SUCCESS)
		{
			SetVia(invite, sip_via.c_str());
			SetContact(invite, sip_contact.c_str());
			SetSessionExpires(invite, refresher.c_str(), expires);

			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(invite, iter->first.c_str(), iter->second.c_str());

			p.SetInfosParams(SIP_INFO_CALLID, GetCallidNumber(invite));
			if (!sdp.empty())
			{
				osip_message_set_body(invite, sdp.c_str(), sdp.size());
				if(content_type.empty())
					osip_message_set_content_type(invite, "application/sdp");
				else
					osip_message_set_content_type(invite, content_type.c_str());
			}

			int cid = eXosip_call_send_initial_invite(m_pData->m_pCtx, invite);
			p.SetInfosParams(SIP_INFO_CID, std::to_string(cid));

			if (cid > 0)
				return OSIP_SUCCESS;
			return OSIP_UNDEFINED_ERROR;
		}
		return ret;
	}

	int BaseSip::SendTrying(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		int tid = std::stoi(p.GetInfosParams(SIP_INFO_TID, "0"));
		if(tid <=0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, "");
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);

		osip_message_t* trying = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_answer(m_pData->m_pCtx, tid, SIP_TRYING, &trying);
		if (ret == OSIP_SUCCESS)
		{
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(trying, iter->first.c_str(), iter->second.c_str());
			if (!sip_contact.empty())
				SetContact(trying, sip_contact.c_str());
			SetVia(trying, sip_via.c_str());
			return eXosip_call_send_answer(m_pData->m_pCtx, tid, SIP_TRYING, trying);
		}
		return ret;
	}

	int BaseSip::SendRinging(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		int tid = std::stoi(p.GetInfosParams(SIP_INFO_TID, "0"));
		if (tid <= 0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, "");
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);

		osip_message_t* ringing = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_answer(m_pData->m_pCtx, tid, SIP_RINGING, &ringing);
		if (ret == OSIP_SUCCESS)
		{
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(ringing, iter->first.c_str(), iter->second.c_str());
			if (!sip_contact.empty())
				SetContact(ringing, sip_contact.c_str());
			SetVia(ringing, sip_via.c_str());
			return eXosip_call_send_answer(m_pData->m_pCtx, tid, SIP_RINGING, ringing);
		}
		return ret;
	}

	int BaseSip::SendInviteResponse(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		int tid = std::stoi(p.GetInfosParams(SIP_INFO_TID, "0"));
		if (tid <= 0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& sdp = p.GetInfosParams(SIP_INFO_CONTENT, "");
		int status_code = std::stoi(p.GetInfosParams(SIP_INFO_STATUS_CODE, "0"));
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, "");
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		osip_message_t* response = nullptr;

		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_answer(m_pData->m_pCtx, tid, status_code, &response);
		if (ret == OSIP_SUCCESS)
		{
			if (status_code == SIP_OK)
			{
				if (!sdp.empty())
				{
					osip_message_set_body(response, sdp.c_str(), sdp.size());
					if (content_type.empty())
						osip_message_set_content_type(response, "application/sdp");
					else
						osip_message_set_content_type(response, content_type.c_str());
				}
			}

			if (!sip_contact.empty())
				SetContact(response, sip_contact.c_str());
			SetVia(response, sip_via.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(response, iter->first.c_str(), iter->second.c_str());

			return eXosip_call_send_answer(m_pData->m_pCtx, tid, status_code, response);
		}

		return ret;
	}

	int BaseSip::SendAck(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);
		auto& subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");
		int did = std::stoi(p.GetInfosParams(SIP_INFO_DID, "0"));

		if (from.empty() || to.empty() || did <= 0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& sdp = p.GetInfosParams(SIP_INFO_CONTENT, "");
		osip_message_t* ack = nullptr;

		SipLock lock(m_pData->m_pCtx);		
		int ret = eXosip_call_build_ack(m_pData->m_pCtx, did, &ack);
		if (ret == OSIP_SUCCESS)
		{
			if (!route.empty())
				SetRequestUri(ack, route.c_str());
			SetVia(ack, sip_via.c_str());
			SetContact(ack, sip_contact.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(ack, iter->first.c_str(), iter->second.c_str());

			if (!sdp.empty())
			{
				osip_message_set_body(ack, sdp.c_str(), sdp.size());
				if (content_type.empty())
					osip_message_set_content_type(ack, "application/sdp");
				else
					osip_message_set_content_type(ack, content_type.c_str());
			}

			return eXosip_call_send_ack(m_pData->m_pCtx, did, ack);
		}
		return ret;
	}

	int BaseSip::SendUpdate(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);
		auto& subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");
		int did = std::stoi(p.GetInfosParams(SIP_INFO_DID, "0"));

		if (from.empty() || to.empty() || did <= 0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& sdp = p.GetInfosParams(SIP_INFO_CONTENT, "");
		osip_message_t* update = nullptr;

		SipLock lock(m_pData->m_pCtx);		
		int ret = eXosip_call_build_update(m_pData->m_pCtx, did, &update);
		if (ret == OSIP_SUCCESS)
		{
			if (!route.empty())
				SetRequestUri(update, route.c_str());
			SetVia(update, sip_via.c_str());
			SetContact(update, sip_contact.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(update, iter->first.c_str(), iter->second.c_str());

			return eXosip_call_send_request(m_pData->m_pCtx, did, update);
		}
		return ret;
	}

	int BaseSip::SendMessage(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		return SendMessageMethod(p, "MESSAGE");
	}

	int BaseSip::SendCallOptions(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		int did = std::stoi(p.GetInfosParams(SIP_INFO_DID, "0"));
		if (did <= 0)
			return OSIP_BADPARAMETER;

		auto& headers = p.GetHeaders();
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, "");
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, "");
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		osip_message_t* options = nullptr;

		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_options(m_pData->m_pCtx, did, &options);
		if (ret == OSIP_SUCCESS)
		{
			if (!route.empty())
				SetRequestUri(options, route.c_str());
			if (!sip_contact.empty())
				SetContact(options, sip_contact.c_str());
			SetVia(options, sip_via.c_str());			
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(options, iter->first.c_str(), iter->second.c_str());

			return eXosip_call_send_request(m_pData->m_pCtx, did, options);
		}
		return ret;
	}

	int BaseSip::SendCallMessage(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		return SendCallMessageMethod(p, "MESSAGE");
	}

	int BaseSip::SendCallInfo(const BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		return SendCallMessageMethod(p, "INFO");
	}

	int BaseSip::SendSubscribe(BaseSipInfos& p)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);
		auto& subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");

		if (from.empty() || to.empty())
			return OSIP_BADPARAMETER;

		std::string sip_from = std::string("sip:") + from;
		std::string sip_to = std::string("sip:") + to;
		std::string sip_route = std::string("sip:") + route;
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		const char* sip_subject = subject.empty() ? nullptr : subject.c_str();
		auto& headers = p.GetHeaders();
		int expires = std::stoi(p.GetInfosParams(SIP_INFO_EXPIRES, "0"));
		auto& event = p.GetInfosParams(SIP_INFO_SUBSCRIBE_EVENT, "presence;id=presence");
		auto& state = p.GetInfosParams(SIP_INFO_SUBSCRIBE_STATE, "active");

		osip_message_t* subscribe = nullptr;
		SipLock lock(m_pData->m_pCtx);
		int sid = eXosip_subscription_build_initial_subscribe(
			m_pData->m_pCtx, 
			&subscribe, 
			sip_to.c_str(),
			sip_from.c_str(),
			sip_route.c_str(),
			event.c_str(),
			expires);

		if (sid > 0)
		{
			SetCseqNumber(subscribe, 1);
			//SetSubscribeEvent(subscribe, "presence;id=presence");
			SetSubscribeState(subscribe, state.c_str());
			SetSubject(subscribe, sip_subject);
			SetVia(subscribe, sip_via.c_str());
			SetContact(subscribe, sip_contact.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(subscribe, iter->first.c_str(), iter->second.c_str());

			int ret = eXosip_subscription_send_initial_request(m_pData->m_pCtx, subscribe);
			if (ret == OSIP_SUCCESS)
			{
				p.SetInfosParams(SIP_INFO_SID, std::to_string(sid));
				p.SetInfosParams(SIP_INFO_CALLID, GetCallidNumber(subscribe));
			}
			return ret;
		}

		return OSIP_UNDEFINED_ERROR;
	}

	int BaseSip::SendSubscribeResponse(int tid, int statusCode)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		if (tid <= 0)
			return OSIP_BADPARAMETER;

		osip_message_t* answer = nullptr;

		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_insubscription_build_answer(m_pData->m_pCtx, tid, statusCode, &answer);
		if (ret == OSIP_SUCCESS)
		{
			return eXosip_insubscription_send_answer(m_pData->m_pCtx, tid, statusCode, answer);
		}

		return ret;
	}

	int BaseSip::RemoveRegister(int rid)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		if (rid <= 0)
			return OSIP_BADPARAMETER;

		SipLock lock(m_pData->m_pCtx);
		return eXosip_register_remove(m_pData->m_pCtx, rid);
	}

	int BaseSip::RemoveSubscribe(int sid)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_BADPARAMETER;

		if (sid <= 0)
			return OSIP_BADPARAMETER;

		SipLock lock(m_pData->m_pCtx);
		return eXosip_subscription_remove(m_pData->m_pCtx, sid);
	}

	int BaseSip::RemoveInSubscribe(int nid)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return OSIP_API_NOT_INITIALIZED;

		if (nid <= 0)
			return OSIP_BADPARAMETER;

		SipLock lock(m_pData->m_pCtx);
		return eXosip_insubscription_remove(m_pData->m_pCtx, nid);
	}

	int BaseSip::SendMessageMethod(const BaseSipInfos& p, const char* method)
	{
		auto& from = p.GetInfosParams(SIP_INFO_FROM, "");
		auto& to = p.GetInfosParams(SIP_INFO_TO, "");
		auto& route = p.GetInfosParams(SIP_INFO_ROUTE, to);

		if (from.empty() || to.empty())
			return OSIP_BADPARAMETER;

		std::string sip_from = std::string("sip:") + from;
		std::string sip_to = std::string("sip:") + to;
		std::string sip_route = std::string("sip:") + route;
		std::string com_str = GetUserInString(from.c_str()) + "@" + m_pData->m_localAddr;
		auto& sip_contact = p.GetInfosParams(SIP_INFO_CONTACT, com_str);
		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		auto& sip_subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");
		auto& headers = p.GetHeaders();
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& content = p.GetInfosParams(SIP_INFO_CONTENT, "");
		auto& encoding = p.GetInfosParams(SIP_INFO_ENCODING, "");
		osip_message_t* msg = nullptr;

		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_message_build_request(
			m_pData->m_pCtx,
			&msg,
			method,
			sip_to.c_str(),
			sip_from.c_str(),
			sip_route.c_str());

		if (ret == OSIP_SUCCESS)
		{
			SetContentType(msg, content_type.c_str());
			SetContentEncoding(msg, encoding.c_str());
			SetContent(msg, content.c_str(), content.size());
			SetVia(msg, sip_via.c_str());
			SetContact(msg, sip_contact.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(msg, iter->first.c_str(), iter->second.c_str());

			return eXosip_message_send_request(m_pData->m_pCtx, msg);
		}
		return ret;
	}

	int BaseSip::SendCallMessageMethod(const BaseSipInfos& p, const char* method)
	{
		int did = std::stoi(p.GetInfosParams(SIP_INFO_DID, "0"));
		if (did <= 0)
			return OSIP_BADPARAMETER;

		auto& sip_via = p.GetInfosParams(SIP_INFO_VIA, m_pData->m_localAddr);
		auto& sip_subject = p.GetInfosParams(SIP_INFO_SUBJECT, "");
		auto& headers = p.GetHeaders();
		auto& content_type = p.GetInfosParams(SIP_INFO_CONTENT_TYPE, "");
		auto& content = p.GetInfosParams(SIP_INFO_CONTENT, "");
		auto& encoding = p.GetInfosParams(SIP_INFO_ENCODING, "");
		osip_message_t* msg = nullptr;

		SipLock lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_request(m_pData->m_pCtx, did, method, &msg);
		if (ret == OSIP_SUCCESS)
		{
			SetContentType(msg, content_type.c_str());
			SetContentEncoding(msg, encoding.c_str());
			SetContent(msg, content.c_str(), content.size());
			SetVia(msg, sip_via.c_str());
			for (auto iter = headers.begin(); iter != headers.end(); ++iter)
				osip_message_replace_header(msg, iter->first.c_str(), iter->second.c_str());

			return eXosip_call_send_request(m_pData->m_pCtx, did, msg);
		}
		return ret;
	}

	static void ParseSipMsgInfo(SipMsgInfo& info, osip_message_t* msg)
	{
		info.m_expires = GetExpires(msg);
		info.m_fromUser = GetFromUser(msg);
		info.m_fromHost = GetFromHost(msg);
		info.m_fromPort = GetFromPort(msg);
		info.m_toUser = GetFromUser(msg);
		info.m_toHost = GetFromHost(msg);
		info.m_toPort = GetFromPort(msg);
		info.m_callid = GetCallidNumber(msg);
		info.m_method = osip_message_get_method(msg);
	}

	static void ParseEventInfo(SipMsgInfo& info, eXosip_event_t* evt)
	{
		info.m_tid = evt->tid;
		info.m_did = evt->did;
		info.m_rid = evt->rid;
		info.m_cid = evt->cid;
		info.m_sid = evt->sid;
		info.m_nid = evt->nid;
		info.m_ssStatus = evt->ss_status;
		info.m_ssReason = evt->ss_reason;
		info.m_tid = evt->tid;
	}

	void BaseSip::DealEvt(void* pEvt)
	{
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (evt != nullptr && evt->request)
		{
			const char* method = osip_message_get_method(evt->request);

			switch (evt->type)
			{
			/******************************* 关于注册 ************************************/
			/************************   REGISTER related events   ************************/
			case EXOSIP_REGISTRATION_SUCCESS: //注册的成功响应   user is successfully registred
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					info.m_statusCode = 200;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->response);
					m_pCb->OnRecvRegisterResponse(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_REGISTRATION_FAILURE: //注册的失败响应   user is not registred
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					info.m_statusCode = -1; //GetStatusCode(evt->response)
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->response);
					m_pCb->OnRecvRegisterResponse(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}				
				break;
			}

			/******************************* 关于呼叫 ************************************/
			/*******************   INVITE related events within calls  *******************/
			case EXOSIP_CALL_INVITE:  //呼叫的请求   announce a new call
			{
				if (m_pCb)
				{ 
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvInvite(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_CALL_REINVITE: //重呼叫的请求   announce a new INVITE within call
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvReinvite(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}				
				break;
			}

			case EXOSIP_CALL_NOANSWER:	 //呼叫超时没有响应   announce no answer within the timeout
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvReinvite(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_CALL_PROCEEDING: //远端正在处理  announce processing by a remote app
			case EXOSIP_CALL_RINGING:	 //振铃          announce ringback
				break;

			case EXOSIP_CALL_ANSWERED: //呼叫被成功响应，即收到invite的成功响应   announce start of call
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->response);
					m_pCb->OnRecvInviteResponse(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_CALL_REDIRECTED:	 //通知一个redirection   announce a redirection
			case EXOSIP_CALL_REQUESTFAILURE: //一个请求失败          announce a request failure
			case EXOSIP_CALL_SERVERFAILURE:	 //一个服务失败          announce a server failure
			case EXOSIP_CALL_GLOBALFAILURE:	 //一个全局失败          announce a global failure
				break;

			case EXOSIP_CALL_ACK: //收到一个ACK    ACK received for 200ok to INVITE
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvAck(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_CALL_CANCELLED: //呼叫被取消   announce that call has been cancelled
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvCallCancel(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			/******************************* 呼叫相关事务 ************************************/
			/************** request related events within calls (except INVITE) **************/
			case EXOSIP_CALL_MESSAGE_NEW: //接收到呼叫中的新消息   announce new incoming request
			{
				this->DealCallMessage(method, pEvt);
				break;
			}

			case EXOSIP_CALL_MESSAGE_PROCEEDING: //呼叫过程的消息处理中  announce a 1xx for request
				break;

			case EXOSIP_CALL_MESSAGE_ANSWERED: //接收到呼叫过程中的消息事件的响应  announce a 200ok
			{
				this->DealCallMessageResponse(method, pEvt);
				break;
			}

			case EXOSIP_CALL_MESSAGE_REDIRECTED:	 //失败状态  announce a failure
			case EXOSIP_CALL_MESSAGE_REQUESTFAILURE: //失败状态  announce a failure
			case EXOSIP_CALL_MESSAGE_SERVERFAILURE:	 //失败状态  announce a failure
			case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:	 //失败状态  announce a failure
				break;

			case EXOSIP_CALL_CLOSED: //呼叫结束，即bye   a BYE was received for this call
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvBye(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}
			
			/**************************  客户端和服务端都可能出现的 **************************/
			/*************************** for both UAS & UAC events ***************************/
			case EXOSIP_CALL_RELEASED: //呼叫的context被清除了  call context is cleared
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvCallRelease(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			/******************************* 呼叫外的消息 ************************************/
			/******************* events received for request outside calls *******************/
			case EXOSIP_MESSAGE_NEW: //接收到呼叫外的消息  announce new incoming request
			{
				this->DealMessage(method, pEvt);
				break;
			}

			case EXOSIP_MESSAGE_PROCEEDING: //接收到呼叫外的消息的处理中 announce a 1xx for request
				break;

			case EXOSIP_MESSAGE_ANSWERED: //接收到呼叫外的消息的响应  announce a 200ok
			{
				this->DealMessageResponse(method, pEvt);
				break;
			}

			case EXOSIP_MESSAGE_REDIRECTED:		//失败状态 announce a failure
			case EXOSIP_MESSAGE_REQUESTFAILURE: //失败状态 announce a failure
			case EXOSIP_MESSAGE_SERVERFAILURE:	//失败状态 announce a failure
			case EXOSIP_MESSAGE_GLOBALFAILURE:	//失败状态 announce a failure
				break;

			/************************* 存在和即时消息传递 *******************************/
			/********************** Presence and Instant Messaging **********************/
			case EXOSIP_SUBSCRIPTION_NOANSWER: //消息超时没有响应 announce no answer
			{
				this->DealSubscribeResponse(method, pEvt);
				break;
			}

			case EXOSIP_SUBSCRIPTION_PROCEEDING: //处理中 announce a 1xx 
				break;

			case EXOSIP_SUBSCRIPTION_ANSWERED: //接收到响应 announce a 200ok
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->response);
					m_pCb->OnRecvSubscribeResponse(info);
				}
				else
				{
					log_printf(LOG_WARN, "There is not a cb to deal.");
				}
				break;
			}

			case EXOSIP_SUBSCRIPTION_REDIRECTED: //收到一个redirection  announce a redirection
				break;

			case EXOSIP_SUBSCRIPTION_REQUESTFAILURE: //请求失败   announce a request failure
			case EXOSIP_SUBSCRIPTION_SERVERFAILURE:	 //服务失败   announce a server failure 
			case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:	 //全局失败   announce a global failure
			{
				SipMsgInfo info;
				ParseEventInfo(info, evt);
				ParseSipMsgInfo(info, evt->response);

				if (m_pCb)
					m_pCb->OnRecvSubscribeResponse(info);
				break;
			}

			case EXOSIP_SUBSCRIPTION_NOTIFY:
			{
				if (m_pCb)
				{
					SipMsgInfo info;
					ParseEventInfo(info, evt);
					ParseSipMsgInfo(info, evt->request);
					m_pCb->OnRecvNotify(info);
				}
				else
				{
					eXosip_lock(m_pData->m_pCtx);
					eXosip_message_send_answer(m_pData->m_pCtx, evt->tid, 200, nullptr);
					eXosip_unlock(m_pData->m_pCtx);
				}
				break;
			}

			case EXOSIP_IN_SUBSCRIPTION_NEW://收到SUBSCRIBE/REFER的请求   announce new incoming SUBSCRIBE/REFER
			{
				SipMsgInfo info;
				ParseEventInfo(info, evt);
				ParseSipMsgInfo(info, evt->request);

				if (m_pCb)
					m_pCb->OnRecvSubscribe(info);
				break;
			}

			/* 暂时不处理notification */
			case EXOSIP_NOTIFICATION_NOANSWER:       //notify超时没有收到响应   announce no answer   
			case EXOSIP_NOTIFICATION_PROCEEDING:     //处理中  announce a 1xx 
			case EXOSIP_NOTIFICATION_ANSWERED:       //接收到响应  announce a 200ok  
			case EXOSIP_NOTIFICATION_REDIRECTED:     //收到一个redirection  announce a redirection
			case EXOSIP_NOTIFICATION_REQUESTFAILURE: //请求失败
			case EXOSIP_NOTIFICATION_SERVERFAILURE:	 //服务失败
			case EXOSIP_NOTIFICATION_GLOBALFAILURE:	 //全局失败
			default:
				break;
			}
			eXosip_event_free(evt);
		}
	}

	void BaseSip::DealMessage(const char* methed, void* pEvt)
	{
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->request);

			if (CompareStringCase(methed, "message") == 0)//MESSAGE
				m_pCb->OnRecvMessage(info);
			else if (CompareStringCase(methed, "register") == 0)//REGISTER
				DealRegister(pEvt);
		}
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}

	void BaseSip::DealMessageResponse(const char* method, void* pEvt)
	{
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->response);

			if (CompareStringCase(method, "message") == 0)//MESSAGE
				m_pCb->OnRecvMessageResponse(info);
		}
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}

	void BaseSip::DealRegister(void* pEvt)
	{
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		osip_authorization_t* auth = nullptr;

		osip_message_get_authorization(evt->request, 0, &auth);
		if (auth && m_pCb)
		{
			SipMsgInfo info;

#if 0
			//需要针对可能出现空指针的情况进行处理！
			const char* auth_uri = osip_authorization_get_uri(auth);
			authInfo.auth_uri = auth_uri == nullptr ? "" : auth_uri;
			const char* nonce = osip_authorization_get_nonce(auth);
			authInfo.nonce = nonce == nullptr ? "" : nonce;
			const char* realm = osip_authorization_get_realm(auth);
			authInfo.realm = realm == nullptr ? "" : realm;
			const char* response = osip_authorization_get_response(auth);
			authInfo.response = response == nullptr ? "" : response;

			authInfo.auth_type = "Digest";
			authInfo.algorithm = "md5";
			authInfo.stale = false;
			authInfo.ha1 = "";//为常量空字符串
			authInfo.method = "REGISTER";//常量字符串
#endif
			info.m_fromUser = GetFromUser(evt->request);
			info.m_fromHost = GetFromHost(evt->request);
			info.m_fromPort = GetFromPort(evt->request);

			info.m_toUser = GetToUser(evt->request);
			info.m_toHost = GetToHost(evt->request);
			info.m_toPort = GetToPort(evt->request);
			
			info.m_callid = GetCallidNumber(evt->request);
			info.m_tid = evt->tid;
			info.m_expires = GetExpires(evt->request);

			m_pCb->OnRecvRegister(info);
		}
		else
		{
			SipLock lock(m_pData->m_pCtx);
			eXosip_message_send_answer(m_pData->m_pCtx, evt->tid, 401, nullptr);
		}
	}

	void BaseSip::DealCallMessage(const char* method, void* pEvt)
	{
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->request);

			if (CompareStringCase(method, "message") == 0)//MESSAGE
				m_pCb->OnRecvCallMessage(info);
		}
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}

	void BaseSip::DealCallMessageResponse(const char* method, void* pEvt)
	{
		//需要判断消息类型
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->response);

			if (CompareStringCase(method, "message") == 0)//MESSAGE
				m_pCb->OnRecvCallMessageResponse(info);
		}		
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}

	void BaseSip::DealSubscribeTimeout(const char* method, void* pEvt)
	{
		//区分是SUBSCIBE还是NOTIFY
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->request);

			if (CompareStringCase(method, "subscribe") == 0)//SUBSCRIBE
				m_pCb->OnRecvSubScribeTimeout(info);
			else if(CompareStringCase(method, "notify") == 0)//NOTIFY
				m_pCb->OnRecvNotifyTimeout(info);
		}
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}

	void BaseSip::DealSubscribeResponse(const char* method, void* pEvt)
	{
		//区分是SUBSCIBE还是NOTIFY
		eXosip_event_t* evt = static_cast<eXosip_event_t*>(pEvt);
		if (m_pCb)
		{
			SipMsgInfo info;
			ParseEventInfo(info, evt);
			ParseSipMsgInfo(info, evt->request);
			if (CompareStringCase(method, "subscribe") == 0)//SUBSCRIBE
				m_pCb->OnRecvSubscribeResponse(info);
			else if (CompareStringCase(method, "notify") == 0)//NOTIFY
				m_pCb->OnRecvNotifyResponse(info);
		}
		else
		{
			log_printf(LOG_WARN, "There is not a cb to deal.");
		}
	}
}