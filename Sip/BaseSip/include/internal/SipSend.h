#ifndef __SIP_SEND_H__
#define __SIP_SEND_H__

#include "ESipSend.h"
#include <map>

namespace sip
{
	class SipRegisterParams
	{
	public:
		SipRegisterParams()
		{
			m_expires = 0;
			m_viaPort = 0;
			m_rid = 0;
		}

		bool IsValid()
		{
			if (m_from.empty() || m_to.empty() || m_pwd.empty())
				return false;

			if (m_expires <= 0)
				return false;

			return true;
		}
	
	public:
		/* in */
		std::string m_from;
		std::string m_to;
		std::string m_contact;
		std::string m_pwd;
		std::string m_viaHost;	
		int m_expires;
		int m_viaPort;

		/* out */
		int m_rid;
		std::string m_callid;
	};

	class SipCallParams
	{
	public:
		SipCallParams()
		{
			m_bIsUacRefresh = true;
			m_expires = 0;
			m_viaPort = 0;
			m_cid = 0;
		}

		bool IsValid()
		{
			if (m_from.empty() || m_to.empty() || m_subject.empty())
				return false;

			if (m_expires <= 0)
				return false;

			return true;
		}

		/* in */
		bool m_bIsUacRefresh;
		std::string m_from;
		std::string m_to;
		std::string m_route;
		std::string m_subject;
		std::string m_contact;
		std::string m_viaHost;
		std::string m_sdp;
		int m_expires;
		int m_viaPort;
		std::map<std::string, std::string> m_headers;

		/* out */
		int m_cid;
		std::string m_callid;
	};

	class SipCallResponseParams
	{
	public:
		SipCallResponseParams()
		{
			m_tid = 0;
			m_statusCode = 0;
		}

		bool IsValid()
		{
			if (m_tid <= 0 || m_statusCode <= 0 )
				return false;

			return true;
		}

		/* in */
		int m_tid;
		int m_statusCode;
		std::string m_contact;
		std::string m_sdp;
		std::string m_via;
		std::map<std::string, std::string> m_headers;
	};

	class SipMessageParams
	{
	public:
		SipMessageParams()
		{
			m_viaPort = 0;
		}

		bool IsValid()
		{
			if (m_from.empty() || m_to.empty())
				return false;

			return true;
		}

		/* in */
		std::string m_from;
		std::string m_to;
		std::string m_route;
		std::string m_contact;
		std::string m_type;
		std::string m_content;
		std::string m_viaHost;
		int m_viaPort;
		std::map<std::string, std::string> m_headers;
	};
}

#endif //!__SIP_SEND_H__