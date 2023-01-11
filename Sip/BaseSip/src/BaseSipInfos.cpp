#include "BaseSip/BaseSipInfos.h"

namespace sip
{
	class BaseSipInfosImp
	{
	public:
		BaseSipInfosImp() {}
		~BaseSipInfosImp() {}

		std::map<std::string, std::string> m_headers;
		std::map<SipInfoType, std::string> m_params;
	};

	BaseSipInfos::BaseSipInfos()
	{
		m_pData = new BaseSipInfosImp();
	}

	BaseSipInfos::~BaseSipInfos()
	{
		delete m_pData;
	}

	void BaseSipInfos::SetInfosParams(SipInfoType type, const std::string& param)
	{
		m_pData->m_params[type] = param;
	}

	const std::string& BaseSipInfos::GetInfosParams(SipInfoType type, const std::string& def) const
	{
		auto iter = m_pData->m_params.find(type);
		if (iter != m_pData->m_params.end())
			return iter->second;

		return def;
	}

	void BaseSipInfos::AddHeaders(const std::string& key, const std::string& value)
	{
		m_pData->m_headers[key] = value;
	}

	const std::map<std::string, std::string>& BaseSipInfos::GetHeaders() const
	{
		return m_pData->m_headers;
	}
}