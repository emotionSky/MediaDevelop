#include <eXosip2/eXosip.h>
#include "internal/SipMessage.h"
#include "internal/SipMessageHelper.h"
#include "internal/SipSend.h"

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
#include "BaseSip.h"

namespace sip
{
	class BaseData
	{
	public:
		BaseData() : m_bRunning(false), m_localPort(0), m_pThread(nullptr), m_pCtx(nullptr) 
		{
			m_localHost = "127.0.0.0";
		}
		bool             m_bRunning;
		int              m_localPort;
		std::string      m_localHost;
		std::thread*     m_pThread;
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

	bool BaseSip::Init(const char* host, int port, const char* agent, ISipRecv* pCb)
	{
		return Init(host, port, agent, AF_INET, true, false, pCb);
	}

	bool BaseSip::Init(const char* host, int port, const char* agent, int family, bool bIsUdp, bool bIsSecure, ISipRecv* pCb)
	{
		if (m_pData->m_pCtx != nullptr)
		{
			log_printf(LOG_NOTICE, "Init BaseSip while it is inited.");
			return true;
		}

		if (port <= 0 || port > 65535)
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
			if (eXosip_listen_addr(m_pData->m_pCtx, transport, host, port, family, secure) != OSIP_SUCCESS)
			{
				eXosip_quit(m_pData->m_pCtx);
				break;
			}

			eXosip_set_cbsip_message(m_pData->m_pCtx, SipMessageCallback);
			
			if(agent)
				eXosip_set_user_agent(m_pData->m_pCtx, agent);

			if (host)
				m_pData->m_localHost = host;
			m_pData->m_localPort = port;
			m_pCb = pCb;
			ret = true;
			break;
		} while (true);

		if (!ret)
		{
			SipFree(m_pData->m_pCtx);
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
		while (m_pData->m_bRunning)
		{
			if (m_pData && m_pData->m_pCtx)
			{
				//设置exosip的超时时间，此处设置为50ms
				evt = eXosip_event_wait(m_pData->m_pCtx, 0, 50);
				if (evt)
				{
					eXosip_lock(m_pData->m_pCtx);
					eXosip_automatic_action(m_pData->m_pCtx);
					//eXosip_automatic_refresh(m_pData->m_pCtx);
					eXosip_unlock(m_pData->m_pCtx);

					DealEvt(evt);
					evt = nullptr;
				}
			}
			else
			{
				log_printf(LOG_ERROR, "Cannot run sip while ctx not inited.");
			}
		}
	}

	int BaseSip::SendRegister(ESipRegisterParams& params)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		SipRegisterParams* p = params.GetData();
		if (!p->IsValid())
			return SIP_ERROR;

		osip_message_t* reg = nullptr;
		std::string user = GetUserInString(p->m_from.c_str());
		std::string from = std::string("sip:") + p->m_from;
		std::string proxy = std::string("sip:") + p->m_to;

		eXosip_lock(m_pData->m_pCtx);
		/* 可能存在注册失败，然后添加代理处进行了密码的修改，这里就需要先删除原有的认证信息，然后替换修改后的认证信息 */
		eXosip_remove_authentication_info(m_pData->m_pCtx, user.c_str(), nullptr);
		eXosip_add_authentication_info(m_pData->m_pCtx, user.c_str(), user.c_str(), p->m_pwd.c_str(), "MD5", nullptr);
		int rid = eXosip_register_build_initial_register(m_pData->m_pCtx,
			from.c_str(),
			proxy.c_str(),
			nullptr,
			p->m_expires,
			&reg);

		if (rid > 0)
		{
			if (!p->m_viaHost.empty())
				SetViaHost(reg, p->m_viaHost.c_str());
			if (p->m_viaPort > 0)
				SetViaPort(reg, p->m_viaPort);
			if (!p->m_contact.empty())
			{
				char user[64], host[64];
				int port;
				if (GetSipInfoInString(p->m_contact.c_str(), user, host, port))
				{
					SetContactUser(reg, user);
					SetContactHost(reg, host);
					SetContactPort(reg, port);
				}
			}
			eXosip_register_send_register(m_pData->m_pCtx, rid, reg);
		}
		eXosip_unlock(m_pData->m_pCtx);
		
		if (rid > 0)
		{
			p->m_callid = GetCallidNumber(reg);
			p->m_rid = rid;
			return SIP_SUCCESS;
		}
		
		return SIP_ERROR;
	}

	int BaseSip::RefreshRegister(int rid, int expires)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		osip_message_t* reg = nullptr;
		eXosip_lock(m_pData->m_pCtx);
		eXosip_register_build_register(m_pData->m_pCtx, rid, expires, &reg);
		if (reg)
			eXosip_register_send_register(m_pData->m_pCtx, rid, reg);

		eXosip_unlock(m_pData->m_pCtx);
		return SIP_SUCCESS;
	}

	int BaseSip::UpdateRegisterPwd(const char* user, const char* newPwd)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		eXosip_lock(m_pData->m_pCtx);
		eXosip_remove_authentication_info(m_pData->m_pCtx, user, nullptr);
		eXosip_add_authentication_info(m_pData->m_pCtx, user, user, newPwd, "MD5", nullptr);
		eXosip_unlock(m_pData->m_pCtx);

		return SIP_SUCCESS;
	}

	int BaseSip::SendRegisterResponse(int tid, int statusCode)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		eXosip_lock(m_pData->m_pCtx);
		eXosip_message_send_answer(m_pData->m_pCtx, tid, statusCode, nullptr);
		eXosip_unlock(m_pData->m_pCtx);

		return SIP_SUCCESS;
	}

	int BaseSip::SendInvite(ESipCallParams& params)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		SipCallParams* p = params.GetData();
		if (!p->IsValid())
			return SIP_ERROR;

		std::string from = std::string("sip:") + p->m_from;
		std::string to = std::string("sip:") + p->m_to;

		osip_message_t* invite = nullptr;
		eXosip_lock(m_pData->m_pCtx);
		int ret = eXosip_call_build_initial_invite(m_pData->m_pCtx,
			&invite,
			to.c_str(),
			from.c_str(),
			nullptr,
			p->m_subject.c_str());

		/* 创建失败就返回 */
		if (ret != OSIP_SUCCESS)
			return SIP_ERROR;

		if (!p->m_viaHost.empty())
			SetViaHost(invite, p->m_viaHost.c_str());
		if (p->m_viaPort > 0)
			SetViaPort(invite, p->m_viaPort);
		if (!p->m_contact.empty())
		{
			char user[64], host[64];
			int port;
			if (GetSipInfoInString(p->m_contact.c_str(), user, host, port))
			{
				SetContactUser(invite, user);
				SetContactHost(invite, host);
				SetContactPort(invite, port);
			}
		}
		SetSessionExpires(invite, p->m_bIsUacRefresh, p->m_expires);

		for(auto iter = p->m_headers.begin(); iter !=p->m_headers.end(); ++iter)
			osip_message_replace_header(invite, iter->first.c_str(), iter->second.c_str());
	
		p->m_callid = GetCallidNumber(invite);

		if (p->m_sdp.size() > 0)
		{
			osip_message_set_content_type(invite, "application/sdp");
			osip_message_set_body(invite, p->m_sdp.c_str(), p->m_sdp.size());
		}

		p->m_cid = eXosip_call_send_initial_invite(m_pData->m_pCtx, invite);
		eXosip_unlock(m_pData->m_pCtx);

		return SIP_SUCCESS;
	}

	int BaseSip::SendRinging(ESipCallResponseParams& params)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		SipCallResponseParams* p = params.GetData();
		if (!p->IsValid())
			return SIP_ERROR;
		
		osip_message_t* answer = nullptr;
		eXosip_lock(m_pData->m_pCtx);
		eXosip_call_build_answer(m_pData->m_pCtx, p->m_tid, 180, &answer); //这里直接写死180

		if(p->m_via.size() > 0)
			SetVia(answer, p->m_via.c_str());
		if (!p->m_contact.empty())
		{
			char user[64], host[64];
			int port;
			if (GetSipInfoInString(p->m_contact.c_str(), user, host, port))
			{
				SetContactUser(answer, user);
				SetContactHost(answer, host);
				SetContactPort(answer, port);
			}
		}
		for (auto iter = p->m_headers.begin(); iter != p->m_headers.end(); ++iter)
			osip_message_replace_header(answer, iter->first.c_str(), iter->second.c_str());

		eXosip_call_send_answer(m_pData->m_pCtx, p->m_tid, 180, answer);
		eXosip_unlock(m_pData->m_pCtx);

		return SIP_SUCCESS;
	}

	int BaseSip::SendInviteResponse(ESipCallResponseParams& params)
	{
		if (!m_pData || !m_pData->m_pCtx)
			return SIP_ERROR;

		SipCallResponseParams* p = params.GetData();
		if (!p->IsValid())
			return SIP_ERROR;

		osip_message_t* answer = nullptr;
		eXosip_lock(m_pData->m_pCtx);
		eXosip_call_build_answer(m_pData->m_pCtx, p->m_tid, p->m_statusCode, &answer);

		if (p->m_statusCode == 200)
		{
			osip_message_set_content_type(answer, "application/sdp");
			osip_message_set_body(answer, p->m_sdp.c_str(), p->m_sdp.size());
		}

		if (p->m_via.size() > 0)
			SetVia(answer, p->m_via.c_str());
		if (!p->m_contact.empty())
		{
			char user[64], host[64];
			int port;
			if (GetSipInfoInString(p->m_contact.c_str(), user, host, port))
			{
				SetContactUser(answer, user);
				SetContactHost(answer, host);
				SetContactPort(answer, port);
			}
		}
		for (auto iter = p->m_headers.begin(); iter != p->m_headers.end(); ++iter)
			osip_message_replace_header(answer, iter->first.c_str(), iter->second.c_str());

		eXosip_call_send_answer(m_pData->m_pCtx, p->m_tid, p->m_statusCode, answer);
		eXosip_unlock(m_pData->m_pCtx);

		return SIP_SUCCESS;
	}

	//BaseSip::
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
			case EXOSIP_NOTIFICATION_NOANSWER: //notify超时没有收到响应   announce no answer   
			case EXOSIP_NOTIFICATION_PROCEEDING: //处理中  announce a 1xx 
			case EXOSIP_NOTIFICATION_ANSWERED:  //接收到响应  announce a 200ok  
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