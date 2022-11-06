#include "internal/SipSend.h"

namespace sip
{
	ESipRegisterParams::ESipRegisterParams()
	{
		m_pData = new SipRegisterParams();
	}

	ESipRegisterParams::~ESipRegisterParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipRegisterParams::SetFrom(const std::string& from)
	{
		m_pData->m_from = from;
	}

	void ESipRegisterParams::SetTo(const std::string& to)
	{
		m_pData->m_to = to;
	}

	void ESipRegisterParams::SetPassword(const std::string& pwd)
	{
		m_pData->m_pwd = pwd;
	}

	void ESipRegisterParams::SetExpires(int expires)
	{
		m_pData->m_expires = expires;
	}

	void ESipRegisterParams::SetVia(const std::string& host, int port)
	{
		m_pData->m_viaHost = host;
		m_pData->m_viaPort = port;
	}

	void ESipRegisterParams::SetContact(const std::string& contact)
	{
		m_pData->m_contact = contact;
	}

	const std::string& ESipRegisterParams::GetCallid() const
	{
		return m_pData->m_callid;
	}

	const int& ESipRegisterParams::GetRid() const
	{
		return m_pData->m_rid;
	}

	SipRegisterParams* ESipRegisterParams::GetData()
	{
		return m_pData;
	}
	
	ESipCallParams::ESipCallParams()
	{
		m_pData = new SipCallParams();
	}

	ESipCallParams::~ESipCallParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipCallParams::SetFrom(const std::string& from)
	{
		m_pData->m_from = from;
	}

	void ESipCallParams::SetTo(const std::string& to) 
	{
		m_pData->m_to = to;
	}

	void ESipCallParams::SetSubject(const std::string& subject)
	{
		m_pData->m_subject = subject;
	}

	void ESipCallParams::SetExpires(bool isUacRefresh, int expires)
	{
		m_pData->m_bIsUacRefresh = isUacRefresh;
		m_pData->m_expires = expires;
	}

	void ESipCallParams::SetSdp(const std::string& sdp)
	{
		m_pData->m_sdp = sdp;
	}

	void ESipCallParams::SetVia(const std::string& host, int port)
	{
		m_pData->m_viaHost = host;
		m_pData->m_viaPort = port;
	}

	void ESipCallParams::SetContact(const std::string& contact)
	{
		m_pData->m_contact = contact;
	}

	void ESipCallParams::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	const std::string& ESipCallParams::GetCallid() const
	{
		return m_pData->m_callid;
	}

	const int& ESipCallParams::GetCid() const
	{
		return m_pData->m_cid;
	}

	SipCallParams* ESipCallParams::GetData()
	{
		return m_pData;
	}

	ESipCallResponseParams::ESipCallResponseParams()
	{
		m_pData = new SipCallResponseParams();
	}

	ESipCallResponseParams::~ESipCallResponseParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipCallResponseParams::SetTid(int tid)
	{
		m_pData->m_tid = tid;
	}

	void ESipCallResponseParams::SetStatusCode(int code)
	{
		m_pData->m_statusCode = code;
	}

	void ESipCallResponseParams::SetVia(const std::string& via)
	{
		m_pData->m_via = via;
	}

	void ESipCallResponseParams::SetContact(const std::string& contact)
	{
		m_pData->m_contact = contact;
	}

	void ESipCallResponseParams::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	SipCallResponseParams* ESipCallResponseParams::GetData()
	{
		return m_pData;
	}

}