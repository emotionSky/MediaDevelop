#ifndef __BASE_SIP_INFOS_H__
#define __BASE_SIP_INFOS_H__

#include "SipDefine.h"
#include <map>
#include <string>

namespace sip
{
	enum SipInfoType
	{
		SIP_INFO_NONE = 0,

		/*************** ���������id ***************/
		SIP_INFO_TID = 1,   // �����ΨһID - unique id for transactions (to be used for answers)
		SIP_INFO_DID,       // һ���Ի���ΨһID - unique id for SIP dialogs
		SIP_INFO_RID,       // ע�������ΨһID - unique id for registration
		SIP_INFO_CID,       // һ���Ự��ΨһID - unique id for SIP calls (but multiple dialogs!)
		SIP_INFO_SID,       // �����ĵ�ΨһID - unique id for outgoing subscriptions
		SIP_INFO_NID,       // ���ն��ĵ�ΨһID - unique id for incoming subscriptions
		SIP_INFO_SS_STATUS, // ��ǰ�Ķ���״̬ - current Subscription-State for subscription
		SIP_INFO_SS_REASON, // ��ǰ�Ķ��Ľ��״̬ - current Reason status for subscription

		/*************** ��������ĳ�ʱ ***************/
		SIP_INFO_EXPIRES = 100, // ע��/����/���ĵĳ�ʱʱ��

		/*************** ���������״̬�� ***************/
		SIP_INFO_STATUS_CODE = 200, // ע�ᡢ���С����ĵȵ�״̬�룬����ͻظ�

		/*************** ���������ͨ���ֶ� ***************/
		SIP_INFO_FROM = 300, // from�ֶΣ����� alice@12.1.12.3:5060
		SIP_INFO_TO,         // to�ֶΣ����� bob@12.1.12.2:5060
		SIP_INFO_ROUTE,      // route�ֶΣ��������͵�Ŀ���ַ������ kevin@12.1.12.5:5060
		SIP_INFO_CONTACT,    // contact�ֶΣ�Ҳ�п����ж������ֻ��һ��ʱ���� yel@12.1.12.7:5060
		SIP_INFO_SUBJECT,    // subject�ֶΣ���ʽ���̶���������������� <sip:alice@10.1.1.21:1234>
		SIP_INFO_VIA,        // via�ֶΣ���ʾSIP��Ϣ�����ĵ�ַ����ֻ��һ������ʱ���� 12.1.12.6:5060


		/*************** ���������ͨ���ֶ� ***************/
		SIP_INFO_CONTENT_TYPE = 400, // ���������е�sdp���͡�message/info�е��������͵ȣ�����  application/sdp  application/command+xml
		SIP_INFO_CONTENT,            // ���������
		SIP_INFO_CALLID,             // �Ի��ı�ʶ�� callid
		SIP_INFO_REFRESHER,          // ���������ˢ�·���uac uas
		SIP_INFO_ENCODING,           // content�ı����ʽ������ zip

		/*************** ע��������ֶ� ***************/
		SIP_INFO_REGISTER_PWD = 500, // ע�������		

		/*************** ����������ֶ� ***************/
		SIP_INFO_SUBSCRIBE_EVENT = 600, //���ķ����е��¼�
		SIP_INFO_SUBSCRIBE_STATE,       //���ķ����е�״̬
	};

	class BaseSipInfosImp;
	class BASESIP_API BaseSipInfos
	{
	public:
		BaseSipInfos();
		~BaseSipInfos();

		void SetInfosParams(SipInfoType type, const std::string& param);
		const std::string& GetInfosParams(SipInfoType type, const std::string& def) const;

		void AddHeaders(const std::string& key, const std::string& value);
		const std::map<std::string, std::string>& GetHeaders() const;

	private:
		BaseSipInfosImp* m_pData;
	};
}

#endif //!__BASE_SIP_INFOS_H__