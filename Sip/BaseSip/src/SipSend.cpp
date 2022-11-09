#include "BaseSip/internal/SipSend.h"

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

	void ESipCallParams::SetDid(int did)
	{
		m_pData->m_did = did;
	}

	void ESipCallParams::SetRoute(const std::string& route)
	{
		m_pData->m_route = route;
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

	void ESipCallResponseParams::SetSdp(const std::string& sdp)
	{
		m_pData->m_sdp = sdp;
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

	ESipMessageParams::ESipMessageParams()
	{
		m_pData = new SipMessageParams();
	}

	ESipMessageParams::~ESipMessageParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipMessageParams::SetFrom(const std::string& from)
	{
		m_pData->m_from = from;
	}

	void ESipMessageParams::SetTo(const std::string& to)
	{
		m_pData->m_to = to;
	}

	void ESipMessageParams::SetContent(const std::string& type, const std::string& encoding, const std::string& content)
	{
		m_pData->m_type = type;
		m_pData->m_endcoding = encoding;
		m_pData->m_content = content;
	}

	void ESipMessageParams::SetRoute(const std::string& route)
	{
		m_pData->m_route = route;
	}

	void ESipMessageParams::SetVia(const std::string& host, int port)
	{
		m_pData->m_viaHost = host;
		m_pData->m_viaPort = port;
	}

	void ESipMessageParams::SetContact(const std::string& contact)
	{
		m_pData->m_contact = contact;
	}

	void ESipMessageParams::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	SipMessageParams* ESipMessageParams::GetData()
	{
		return m_pData;
	}

	ESipCallMessageParams::ESipCallMessageParams()
	{
		m_pData = new SipCallMessageParams();
	}

	ESipCallMessageParams::~ESipCallMessageParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipCallMessageParams::SetDid(int did)
	{
		m_pData->m_did = did;
	}

	void ESipCallMessageParams::SetContent(const std::string& type, const std::string& encoding, const std::string& content)
	{
		m_pData->m_type = type;
		m_pData->m_endcoding = encoding;
		m_pData->m_content = content;
	}

	void ESipCallMessageParams::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	SipCallMessageParams* ESipCallMessageParams::GetData()
	{
		return m_pData;
	}

	ESipSuscribeParams::ESipSuscribeParams()
	{
		m_pData = new SipSuscribeParams();
	}

	ESipSuscribeParams::~ESipSuscribeParams()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = nullptr;
		}
	}

	void ESipSuscribeParams::SetEvent(const std::string& event)
	{
		m_pData->m_event = event;
	}

	void ESipSuscribeParams::SetFrom(const std::string& from)
	{
		m_pData->m_from = from;
	}

	void ESipSuscribeParams::SetTo(const std::string& to)
	{
		m_pData->m_to = to;
	}

	void ESipSuscribeParams::SetSubject(const std::string& subject)
	{
		m_pData->m_subject = subject;
	}

	void ESipSuscribeParams::SetExpires(int expires)
	{
		m_pData->m_expires = expires;
	}

	void ESipSuscribeParams::SetRoute(const std::string& route)
	{
		m_pData->m_route = route;
	}

	void ESipSuscribeParams::SetVia(const std::string& host, int port)
	{
		m_pData->m_viaHost = host;
		m_pData->m_viaPort = port;
	}

	void ESipSuscribeParams::SetContact(const std::string& contact)
	{
		m_pData->m_contact = contact;
	}

	void ESipSuscribeParams::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	const std::string& ESipSuscribeParams::GetCallid() const
	{
		return m_pData->m_callid;
	}

	const int& ESipSuscribeParams::GetSid() const
	{
		return m_pData->m_sid;
	}

	SipSuscribeParams* ESipSuscribeParams::GetData()
	{
		return m_pData;
	}
}