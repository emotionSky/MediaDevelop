#ifndef __E_SIP_SEND_H__
#define __E_SIP_SEND_H__

#include "SipDefine.h"
#include <string>

namespace sip
{
	class SipRegisterParams;
	class ESipRegisterParams
	{
	public:
		ESipRegisterParams();
		~ESipRegisterParams();

		/* 配置参数 */
		void SetFrom(const std::string& from);       //from:  alice:10.1.10.21:5060
		void SetTo(const std::string& to);           //to:  Bob:10.1.10.43:5060
		void SetPassword(const std::string& pwd);    //密码
		void SetExpires(int expires);                //注册的截止时间，单位为秒

		/* 可选配置参数 */
		void SetVia(const std::string& host, int port); //只配置via的host和端口
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060

		/* 获取结果数据 */
		const std::string& GetCallid() const; //获取注册事件中的callid
		const int& GetRid() const;            //获取注册id

		/* 限库内使用 */
		SipRegisterParams* GetData(); //外部调用无法使用

	private:
		SipRegisterParams* m_pData;
	};
	
	class SipCallParams;
	class ESipCallParams
	{
	public:
		ESipCallParams();
		~ESipCallParams();

		/* 配置参数 */
		void SetFrom(const std::string& from);       //from:  alice:10.1.10.21:5060
		void SetTo(const std::string& to);           //to:  Bob:10.1.10.43:5060
		void SetSubject(const std::string& subject); //subject: 形式比较任意，常用为 kevin@10.1.21.23:5060
		void SetExpires(bool isUacRefresh, int expires); //注册的截止时间，单位为秒
		void SetSdp(const std::string& sdp);         //sdp

		/* 可选配置参数 */
		void SetVia(const std::string& host, int port); //只配置via的host和端口
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060
		void AddHeaders(const std::string& key, const std::string& value);

		/* 获取结果数据 */
		const std::string& GetCallid() const; //获取注册事件中的callid
		const int& GetCid() const;            //获取注册id

		/* 限库内使用 */
		SipCallParams* GetData(); //外部调用无法使用

	private:
		SipCallParams* m_pData;
	};

	class SipCallResponseParams;
	class ESipCallResponseParams
	{
	public:
		ESipCallResponseParams();
		~ESipCallResponseParams();

		/* 配置参数 */
		void SetTid(int tid);
		void SetStatusCode(int code);

		/* invite的响应使用 */
		void SetSdp(const std::string& sdp);

		/* 可选配置参数 */
		void SetVia(const std::string& via);            //这里必须给via的所有字段了
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060
		void AddHeaders(const std::string& key, const std::string& value);

		/* 限库内使用 */
		SipCallResponseParams* GetData(); //外部调用无法使用

	private:
		SipCallResponseParams* m_pData;
	};
}

#endif //!__E_SIP_SEND_H__