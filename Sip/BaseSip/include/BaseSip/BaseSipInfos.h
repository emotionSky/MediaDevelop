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

		/*************** 各类事务的id ***************/
		SIP_INFO_TID = 1,   // 事务的唯一ID - unique id for transactions (to be used for answers)
		SIP_INFO_DID,       // 一个对话的唯一ID - unique id for SIP dialogs
		SIP_INFO_RID,       // 注册事务的唯一ID - unique id for registration
		SIP_INFO_CID,       // 一个会话的唯一ID - unique id for SIP calls (but multiple dialogs!)
		SIP_INFO_SID,       // 发起订阅的唯一ID - unique id for outgoing subscriptions
		SIP_INFO_NID,       // 接收订阅的唯一ID - unique id for incoming subscriptions
		SIP_INFO_SS_STATUS, // 当前的订阅状态 - current Subscription-State for subscription
		SIP_INFO_SS_REASON, // 当前的订阅结果状态 - current Reason status for subscription

		/*************** 各类事务的超时 ***************/
		SIP_INFO_EXPIRES = 100, // 注册/呼叫/订阅的超时时间

		/*************** 各类事务的状态码 ***************/
		SIP_INFO_STATUS_CODE = 200, // 注册、呼叫、订阅等的状态码，请求和回复

		/*************** 各类事务的通用字段 ***************/
		SIP_INFO_FROM = 300, // from字段，形如 alice@12.1.12.3:5060
		SIP_INFO_TO,         // to字段，形如 bob@12.1.12.2:5060
		SIP_INFO_ROUTE,      // route字段，真正发送的目标地址，形如 kevin@12.1.12.5:5060
		SIP_INFO_CONTACT,    // contact字段，也有可能有多个，当只有一个时形如 yel@12.1.12.7:5060
		SIP_INFO_SUBJECT,    // subject字段，形式不固定，多数情况下形如 <sip:alice@10.1.1.21:1234>
		SIP_INFO_VIA,        // via字段，表示SIP消息经过的地址，当只有一个经过时形如 12.1.12.6:5060


		/*************** 部分事务的通用字段 ***************/
		SIP_INFO_CONTENT_TYPE = 400, // 呼叫事务中的sdp类型、message/info中的内容类型等，形如  application/sdp  application/command+xml
		SIP_INFO_CONTENT,            // 具体的内容
		SIP_INFO_CALLID,             // 对话的标识符 callid
		SIP_INFO_REFRESHER,          // 呼叫事务的刷新方，uac uas
		SIP_INFO_ENCODING,           // content的编码格式，常有 zip

		/*************** 注册事务的字段 ***************/
		SIP_INFO_REGISTER_PWD = 500, // 注册的密码		

		/*************** 订阅事务的字段 ***************/
		SIP_INFO_SUBSCRIBE_EVENT = 600, //订阅方法中的事件
		SIP_INFO_SUBSCRIBE_STATE,       //订阅方法中的状态
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