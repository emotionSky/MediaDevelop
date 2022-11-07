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
			m_pData->m_pCtx = eXosip_malloc(); //ʵ���Ͼ��� osip_malloc

			/* ��ʼ�� exosip */
			if (eXosip_init(m_pData->m_pCtx) != OSIP_SUCCESS)
				break;

			/* ͨѶЭ�鼰�˿ڼ��� */
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
				//����exosip�ĳ�ʱʱ�䣬�˴�����Ϊ50ms
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
		/* ���ܴ���ע��ʧ�ܣ�Ȼ����Ӵ���������������޸ģ��������Ҫ��ɾ��ԭ�е���֤��Ϣ��Ȼ���滻�޸ĺ����֤��Ϣ */
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

		/* ����ʧ�ܾͷ��� */
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
		eXosip_call_build_answer(m_pData->m_pCtx, p->m_tid, 180, &answer); //����ֱ��д��180

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
			/******************************* ����ע�� ************************************/
			/************************   REGISTER related events   ************************/
			case EXOSIP_REGISTRATION_SUCCESS: //ע��ĳɹ���Ӧ   user is successfully registred
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

			case EXOSIP_REGISTRATION_FAILURE: //ע���ʧ����Ӧ   user is not registred
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

			/******************************* ���ں��� ************************************/
			/*******************   INVITE related events within calls  *******************/
			case EXOSIP_CALL_INVITE:  //���е�����   announce a new call
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

			case EXOSIP_CALL_REINVITE: //�غ��е�����   announce a new INVITE within call
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

			case EXOSIP_CALL_NOANSWER:	 //���г�ʱû����Ӧ   announce no answer within the timeout
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

			case EXOSIP_CALL_PROCEEDING: //Զ�����ڴ���  announce processing by a remote app
			case EXOSIP_CALL_RINGING:	 //����          announce ringback
				break;

			case EXOSIP_CALL_ANSWERED: //���б��ɹ���Ӧ�����յ�invite�ĳɹ���Ӧ   announce start of call
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

			case EXOSIP_CALL_REDIRECTED:	 //֪ͨһ��redirection   announce a redirection
			case EXOSIP_CALL_REQUESTFAILURE: //һ������ʧ��          announce a request failure
			case EXOSIP_CALL_SERVERFAILURE:	 //һ������ʧ��          announce a server failure
			case EXOSIP_CALL_GLOBALFAILURE:	 //һ��ȫ��ʧ��          announce a global failure
				break;

			case EXOSIP_CALL_ACK: //�յ�һ��ACK    ACK received for 200ok to INVITE
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

			case EXOSIP_CALL_CANCELLED: //���б�ȡ��   announce that call has been cancelled
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

			/******************************* ����������� ************************************/
			/************** request related events within calls (except INVITE) **************/
			case EXOSIP_CALL_MESSAGE_NEW: //���յ������е�����Ϣ   announce new incoming request
			{
				this->DealCallMessage(method, pEvt);
				break;
			}

			case EXOSIP_CALL_MESSAGE_PROCEEDING: //���й��̵���Ϣ������  announce a 1xx for request
				break;

			case EXOSIP_CALL_MESSAGE_ANSWERED: //���յ����й����е���Ϣ�¼�����Ӧ  announce a 200ok
			{
				this->DealCallMessageResponse(method, pEvt);
				break;
			}

			case EXOSIP_CALL_MESSAGE_REDIRECTED:	 //ʧ��״̬  announce a failure
			case EXOSIP_CALL_MESSAGE_REQUESTFAILURE: //ʧ��״̬  announce a failure
			case EXOSIP_CALL_MESSAGE_SERVERFAILURE:	 //ʧ��״̬  announce a failure
			case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:	 //ʧ��״̬  announce a failure
				break;

			case EXOSIP_CALL_CLOSED: //���н�������bye   a BYE was received for this call
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
			
			/**************************  �ͻ��˺ͷ���˶����ܳ��ֵ� **************************/
			/*************************** for both UAS & UAC events ***************************/
			case EXOSIP_CALL_RELEASED: //���е�context�������  call context is cleared
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

			/******************************* ���������Ϣ ************************************/
			/******************* events received for request outside calls *******************/
			case EXOSIP_MESSAGE_NEW: //���յ����������Ϣ  announce new incoming request
			{
				this->DealMessage(method, pEvt);
				break;
			}

			case EXOSIP_MESSAGE_PROCEEDING: //���յ����������Ϣ�Ĵ����� announce a 1xx for request
				break;

			case EXOSIP_MESSAGE_ANSWERED: //���յ����������Ϣ����Ӧ  announce a 200ok
			{
				this->DealMessageResponse(method, pEvt);
				break;
			}

			case EXOSIP_MESSAGE_REDIRECTED:		//ʧ��״̬ announce a failure
			case EXOSIP_MESSAGE_REQUESTFAILURE: //ʧ��״̬ announce a failure
			case EXOSIP_MESSAGE_SERVERFAILURE:	//ʧ��״̬ announce a failure
			case EXOSIP_MESSAGE_GLOBALFAILURE:	//ʧ��״̬ announce a failure
				break;

			/************************* ���ںͼ�ʱ��Ϣ���� *******************************/
			/********************** Presence and Instant Messaging **********************/
			case EXOSIP_SUBSCRIPTION_NOANSWER: //��Ϣ��ʱû����Ӧ announce no answer
			{
				this->DealSubscribeResponse(method, pEvt);
				break;
			}

			case EXOSIP_SUBSCRIPTION_PROCEEDING: //������ announce a 1xx 
				break;

			case EXOSIP_SUBSCRIPTION_ANSWERED: //���յ���Ӧ announce a 200ok
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

			case EXOSIP_SUBSCRIPTION_REDIRECTED: //�յ�һ��redirection  announce a redirection
				break;

			case EXOSIP_SUBSCRIPTION_REQUESTFAILURE: //����ʧ��   announce a request failure
			case EXOSIP_SUBSCRIPTION_SERVERFAILURE:	 //����ʧ��   announce a server failure 
			case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:	 //ȫ��ʧ��   announce a global failure
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

			case EXOSIP_IN_SUBSCRIPTION_NEW://�յ�SUBSCRIBE/REFER������   announce new incoming SUBSCRIBE/REFER
			{
				SipMsgInfo info;
				ParseEventInfo(info, evt);
				ParseSipMsgInfo(info, evt->request);

				if (m_pCb)
					m_pCb->OnRecvSubscribe(info);
				break;
			}

			/* ��ʱ������notification */
			case EXOSIP_NOTIFICATION_NOANSWER: //notify��ʱû���յ���Ӧ   announce no answer   
			case EXOSIP_NOTIFICATION_PROCEEDING: //������  announce a 1xx 
			case EXOSIP_NOTIFICATION_ANSWERED:  //���յ���Ӧ  announce a 200ok  
			case EXOSIP_NOTIFICATION_REDIRECTED:     //�յ�һ��redirection  announce a redirection
			case EXOSIP_NOTIFICATION_REQUESTFAILURE: //����ʧ��
			case EXOSIP_NOTIFICATION_SERVERFAILURE:	 //����ʧ��
			case EXOSIP_NOTIFICATION_GLOBALFAILURE:	 //ȫ��ʧ��
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
		//��Ҫ�ж���Ϣ����
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
		//������SUBSCIBE����NOTIFY
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
		//������SUBSCIBE����NOTIFY
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