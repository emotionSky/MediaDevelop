#include <eXosip2/eXosip.h>
#include "internal/SipMessage.h"

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
		BaseData() : m_bRunning(false), m_pThread(nullptr), m_pCtx(nullptr) {}
		bool             m_bRunning;
		std::thread*     m_pThread;
		struct eXosip_t* m_pCtx;
	};

	BaseSip::BaseSip()
	{
		m_pData = new BaseData();
	}

	BaseSip::~BaseSip()
	{
		StopSip();
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	bool BaseSip::Init(const char* host, int port)
	{
		return Init(host, port, AF_INET, true, false);
	}

	bool BaseSip::Init(const char* host, int port, int family, bool bIsUdp, bool bIsSecure)
	{
		if (m_pData->m_pCtx != nullptr)
		{
			log_printf(LOG_NOTICE, "Init BaseSip while it is inited.");
			return true;
		}

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
		while (m_pData->m_bRunning)
		{

		}
	}
}