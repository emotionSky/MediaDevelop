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

		/* ���ò��� */
		void SetFrom(const std::string& from);       //from:  alice:10.1.10.21:5060
		void SetTo(const std::string& to);           //to:  Bob:10.1.10.43:5060
		void SetPassword(const std::string& pwd);    //����
		void SetExpires(int expires);                //ע��Ľ�ֹʱ�䣬��λΪ��

		/* ��ѡ���ò��� */
		void SetVia(const std::string& host, int port); //ֻ����via��host�Ͷ˿�
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060

		/* ��ȡ������� */
		const std::string& GetCallid() const; //��ȡע���¼��е�callid
		const int& GetRid() const;            //��ȡע��id

		/* �޿���ʹ�� */
		SipRegisterParams* GetData(); //�ⲿ�����޷�ʹ��

	private:
		SipRegisterParams* m_pData;
	};
	
	class SipCallParams;
	class ESipCallParams
	{
	public:
		ESipCallParams();
		~ESipCallParams();

		/* ���ò��� */
		void SetFrom(const std::string& from);       //from:  alice:10.1.10.21:5060
		void SetTo(const std::string& to);           //to:  Bob:10.1.10.43:5060
		void SetSubject(const std::string& subject); //subject: ��ʽ�Ƚ����⣬����Ϊ kevin@10.1.21.23:5060
		void SetExpires(bool isUacRefresh, int expires); //ע��Ľ�ֹʱ�䣬��λΪ��
		void SetSdp(const std::string& sdp);         //sdp

		/* ��ѡ���ò��� */
		void SetVia(const std::string& host, int port); //ֻ����via��host�Ͷ˿�
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060
		void AddHeaders(const std::string& key, const std::string& value);

		/* ��ȡ������� */
		const std::string& GetCallid() const; //��ȡע���¼��е�callid
		const int& GetCid() const;            //��ȡע��id

		/* �޿���ʹ�� */
		SipCallParams* GetData(); //�ⲿ�����޷�ʹ��

	private:
		SipCallParams* m_pData;
	};

	class SipCallResponseParams;
	class ESipCallResponseParams
	{
	public:
		ESipCallResponseParams();
		~ESipCallResponseParams();

		/* ���ò��� */
		void SetTid(int tid);
		void SetStatusCode(int code);

		/* invite����Ӧʹ�� */
		void SetSdp(const std::string& sdp);

		/* ��ѡ���ò��� */
		void SetVia(const std::string& via);            //��������via�������ֶ���
		void SetContact(const std::string& contact);    //contact:  alice:10.1.10.23:5060
		void AddHeaders(const std::string& key, const std::string& value);

		/* �޿���ʹ�� */
		SipCallResponseParams* GetData(); //�ⲿ�����޷�ʹ��

	private:
		SipCallResponseParams* m_pData;
	};
}

#endif //!__E_SIP_SEND_H__