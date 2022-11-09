#ifndef __SIP_MESSAGE_H__
#define __SIP_MESSAGE_H__

#include <osipparser2/osip_message.h>
#include <osipparser2/sdp_message.h>
#include <osipparser2/osip_parser.h>
#include <cstring>
#include <cstdlib>
#include <malloc.h>
#include "SipCommon.h"

namespace sip
{
	/**************************************** Memory operation ****************************************/
	void* SipMalloc(int len);
	void SipFree(void* buf);


	/**************************************** Get message info ****************************************/

	/*
	 * @brief 获取 content-type
	 * @param[in] msg  sip消息结构体
	 * @return content-type
	 * - nullptr  说明没有找到
	 * - pointer  有内容，需要调用函数进行释放
	 */
	static inline char* GetContentType(const osip_message_t* msg)
	{
		const osip_content_type_t* ctype = msg->content_type;
		if (ctype)
		{
			char* strType = nullptr;
			if (osip_content_type_to_str(ctype, &strType) == OSIP_SUCCESS)
				return strType;
		}
		return nullptr;
	}

	static inline const char* GetContent(const osip_message_t* msg, size_t& len)
	{
		len = 0;
		osip_body_t* body = nullptr;
		osip_message_get_body(msg, 0, &body);
		if (body && body->body)
		{
			len = body->length;
			return body->body;
		}
		return nullptr;
	}

	static inline const char* GetContentEncoding(const osip_message_t* msg)
	{
		osip_content_encoding_t* encoding = nullptr;
		osip_message_get_content_encoding(msg, 0, &encoding);
		if (encoding && encoding->value)
		{
			return encoding->value;
		}
		return nullptr;
	}

	/*
	 * @brief 获取 subject
	 * @param[in] msg  sip消息结构体
	 * @return subject，形如 <sip:alice@10.1.1.21:1234>
	 */
	static inline const char* GetSubject(const osip_message_t* msg)
	{
		osip_header_t* header = nullptr;
		osip_message_get_subject(msg, 0, &header);
		if (header)
		{
			return header->hvalue;
		}
		return nullptr;
	}
	
	static inline const char* GetCallidNumber(const osip_message_t* msg)
	{
		osip_call_id_t* callid = osip_message_get_call_id(msg);
		if (callid)
		{
			return callid->number;
		}
		return nullptr;
	}

	static inline const char* GetCallidHost(const osip_message_t* msg)
	{
		osip_call_id_t* callid = osip_message_get_call_id(msg);
		if (callid)
		{
			return callid->host;
		}
		return nullptr;
	}

	static inline const char* GetFromUser(const osip_message_t* msg)
	{
		osip_from_t* from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from);
		if (uri && from)
		{
			return uri->username;
		}
		return nullptr;
	}

	static inline const char* GetFromPassword(const osip_message_t* msg)
	{
		osip_from_t* from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from);
		if (uri && from)
		{
			return uri->password;
		}
		return nullptr;
	}

	static inline const char* GetFromHost(const osip_message_t* msg)
	{
		osip_from_t* from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from);
		if (uri && from)
		{
			return uri->host;
		}

		return nullptr;
	}

	static inline int GetFromPort(const osip_message_t* msg)
	{
		osip_from_t* from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from);
		if (uri && from)
		{
			return atoi(uri->port);
		}

		return 0;
	}

	static inline const char* GetFromTag(const osip_message_t* msg)
	{
		osip_from_t* from = osip_message_get_from(msg);
		if (from)
		{
			osip_generic_param_t* tag_param = NULL;
			osip_from_get_tag(from, &tag_param);
			if (tag_param)
			{
				return tag_param->gvalue;
			}
		}
		return nullptr;
	}

	static inline const char* GetToUser(const osip_message_t* msg)
	{
		osip_from_t* to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to);
		if (uri && to)
		{
			return uri->username;
		}
		return nullptr;
	}

	static inline const char* GetToPassword(const osip_message_t* msg)
	{
		osip_from_t* to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to);
		if (uri && to)
		{
			return uri->password;
		}
		return nullptr;
	}

	static inline const char* GetToHost(const osip_message_t* msg)
	{
		osip_from_t* to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to);
		if (uri && to)
		{
			return uri->host;
		}

		return nullptr;
	}

	static inline int GetToPort(const osip_message_t* msg)
	{
		osip_from_t* to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to);
		if (uri && to)
		{
			return osip_atoi(uri->port);
		}

		return 0;
	}

	static inline const char* GetToTag(const osip_message_t* msg)
	{
		osip_to_t* to = osip_message_get_to(msg);
		if (to)
		{
			osip_generic_param_t* tag_param = NULL;
			osip_to_get_tag(to, &tag_param);
			if (tag_param)
			{
				return tag_param->gvalue;
			}
		}
		return nullptr;
	}

	/*
	 * @brief 获取 method
	 * @param[in] msg  sip消息结构体
	 * @return method，形如 REGIST INVITE UPDATE
	 */
	static inline const char* GetMethod(const osip_message_t* msg)
	{
		osip_cseq_t* cseq = osip_message_get_cseq(msg);
		if (cseq)
		{
			return cseq->method;
		}

		return nullptr;
	}

	static inline int GetCseqNumber(const osip_message_t* msg)
	{
		osip_cseq_t* cseq = osip_message_get_cseq(msg);
		if (cseq)
		{
			return osip_atoi(cseq->number);
		}

		return 0 ;
	}
	
	static inline char* GetVia(const osip_message_t* msg)
	{
		char* buf = nullptr;
		osip_via_t* via = nullptr;
		osip_message_get_via(msg, 0, &via);
		if (via)
		{
			osip_via_to_str(via, &buf);
		}
		return buf;
	}
	
	static inline int GetStatusCode(const osip_message_t* msg)
	{
		osip_message_get_status_code(msg);
	}

	static inline int GetExpires(const osip_message_t* msg)
	{
		osip_header_t* expires = nullptr;
		osip_message_get_expires(msg, 0, &expires);
		if (expires)
		{
			return osip_atoi(expires->hvalue);
		}
		return 0;
	}
	
	static inline int GetSessionExpires(const osip_message_t* msg, char* refresher)
	{
		int expires = 0;
		osip_header_t* se_exp = nullptr;
		osip_message_header_get_byname(msg, "Session-Expires", 0, &se_exp);
		if (se_exp)
		{
			sscanf(se_exp->hvalue, "%i;refresher=%s", &expires, refresher);
		}
		return expires;
	}

	/**************************************** Set message info ****************************************/

	static inline bool SetContentType(osip_message_t* msg, const char* ctype)
	{
		if (msg && ctype)
			return osip_message_set_content_type(msg, ctype) == OSIP_SUCCESS;

		return false;
	}

	static inline void SetContentEncoding(osip_message_t* msg, const char* encoding)
	{
		osip_message_set_content_encoding(msg, encoding);
	}

	static inline void SetContent(osip_message_t* msg, const char* content, size_t len)
	{
		if (!content || len == 0)
			return;

		osip_body_t* body = nullptr;
		osip_message_get_body(msg, 0, &body);
		if (body)
		{
			if (body->body)
				SipFree(body->body);

			osip_body_parse(body, content, len);
		}
		else
		{
			osip_message_set_body(msg, content, len);
		}
	}
	
	static inline void SetFromUser(osip_message_t* msg, const char* user)
	{
		osip_from_t* from_url = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from_url);
		if (from_url && uri)
		{
			osip_uri_set_username(uri, osip_strdup(user));
		}
	}

	static inline void SetFromHost(osip_message_t* msg, const char* host)
	{
		osip_from_t* from_url = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from_url);
		if (from_url && uri)
		{
			osip_uri_set_host(uri, osip_strdup(host));
		}
	}

	static inline void SetFromPort(osip_message_t* msg, int port)
	{
		osip_from_t* from_url = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from_url);
		if (from_url && uri)
		{
			char p[10] = { 0 };
			sprintf(p, "%d", port);
			osip_uri_set_host(uri, osip_strdup(p));
		}
	}

	static inline void SetToUser(osip_message_t* msg, const char* user)
	{
		osip_from_t* to_url = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to_url);
		if (to_url && uri)
		{
			osip_uri_set_username(uri, osip_strdup(user));
		}
	}

	static inline void SetToHost(osip_message_t* msg, const char* host)
	{
		osip_from_t* to_url = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to_url);
		if (to_url && uri)
		{
			osip_uri_set_host(uri, osip_strdup(host));
		}
	}

	static inline void SetToPort(osip_message_t* msg, int port)
	{
		osip_from_t* to_url = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to_url);
		if (to_url && uri)
		{
			char p[10] = { 0 };
			sprintf(p, "%d", port);
			osip_uri_set_host(uri, osip_strdup(p));
		}
	}

	static inline void SetVia(osip_message_t* msg, const char* via)
	{
		osip_via_t* sip_via = NULL;
		osip_message_get_via(msg, 0, &sip_via);
		if (sip_via)
		{
			osip_via_parse(sip_via, via);
		}
	}

	static inline void SetViaHost(osip_message_t* msg, const char* host)
	{
		osip_via_t* via = NULL;
		osip_message_get_via(msg, 0, &via);
		if (via)
		{
			osip_via_set_host(via, osip_strdup(host));
		}
	}

	static inline void SetViaPort(osip_message_t* msg, int port)
	{
		osip_via_t* via = NULL;
		osip_message_get_via(msg, 0, &via);
		if (via)
		{
			char p[10] = { 0 };
			sprintf(p, "%d", port);
			osip_via_set_port(via, osip_strdup(p));
		}
	}

	static inline void SetContact(osip_message_t* msg, const char* str)
	{
		if (str)
		{
			char user[64], host[32], port[16];
			int ret = sscanf(str, "%[^@]@%[^:]:%s", user, host, port);
			if (ret == 3)
			{
				osip_contact_t* contact = NULL;
				osip_message_get_contact(msg, 0, &contact);
				osip_uri_t* uri = osip_contact_get_url(contact);
				if (contact && uri)
				{
					osip_uri_set_username(uri, osip_strdup(user));
					osip_uri_set_host(uri, osip_strdup(host));
					osip_uri_set_host(uri, osip_strdup(port));
				}
			}
		}
	}

	static inline void SetContactUser(osip_message_t* msg, const char* user)
	{
		osip_contact_t* contact = NULL;
		osip_message_get_contact(msg, 0, &contact);
		osip_uri_t* uri = osip_contact_get_url(contact);
		if (contact && uri)
		{
			osip_uri_set_username(uri, osip_strdup(user));
		}
	}

	static inline void SetContactHost(osip_message_t* msg, const char* host)
	{
		osip_contact_t* contact = NULL;
		osip_message_get_contact(msg, 0, &contact);
		osip_uri_t* uri = osip_contact_get_url(contact);
		if (contact && uri)
		{
			osip_uri_set_host(uri, osip_strdup(host));
		}
	}

	static inline void SetContactPort(osip_message_t* msg, int port)
	{
		osip_contact_t* contact = NULL;
		osip_message_get_contact(msg, 0, &contact);
		osip_uri_t* uri = osip_contact_get_url(contact);
		if (contact && uri)
		{
			char p[10] = { 0 };
			sprintf(p, "%d", port);
			osip_uri_set_host(uri, osip_strdup(p));
		}
	}

	/*
	 * @brief 设置 subject 
	 * @param[in] subject 形如 <sip:alice@10.1.1.21:1234>
	 * @return void
	 */
	static inline void SetSubject(osip_message_t* msg, const char* subject)
	{
		osip_message_set_subject(msg, subject);
	}

	static inline void SetSubscribeState(osip_message_t* msg, const char* state)
	{
		osip_message_replace_header(msg, "Subscription-state", state);
	}

	static inline void SetSubscribeEvent(osip_message_t* msg, const char* event)
	{
		osip_message_replace_header(msg, "Event", event);
	}
	
	static inline void SetCseqNumber(osip_message_t* msg, int num)
	{
		osip_cseq_t* sip_cseq = osip_message_get_cseq(msg);
		if (sip_cseq)
		{
			char buf[64];
			sprintf(buf, "%d", num);
			osip_cseq_set_number(sip_cseq, osip_strdup(buf));
		}
	}
	
	/*
	 * @brief 设置会话的 expires
	 * @param[in] msg         sip消息结构体
	 * @param[in] session_exp expires的内容，形如 60;refresher=uas    60;refresher=uac
	 */
	static inline void SetSessionExpires(osip_message_t* msg, bool isUac, int expire)
	{
		char session_exp[32] = { 0 };
		sprintf(session_exp, "%d;refresher=%s", expire, isUac ? "uac" : "uas");
		osip_message_set_header(msg, "Session-Expires", session_exp);
	}

	static inline void SetRequestUri(osip_message_t* msg, const char* uri)
	{
		char user[64], host[32], port[16];
		int ret = sscanf(uri, "%[^@]@%[^:]:%s", user, host, port);
		if (ret == 3)
		{
			char* p = nullptr;
			p = osip_uri_get_username(msg->req_uri);
			if (p)
				SipFree(p);
			osip_uri_set_username(msg->req_uri, osip_strdup(user));

			p = osip_uri_get_host(msg->req_uri);
			if (p)
				SipFree(p);
			osip_uri_set_host(msg->req_uri, osip_strdup(host));

			p = osip_uri_get_port(msg->req_uri);
			if (p)
				SipFree(p);
			osip_uri_set_host(msg->req_uri, osip_strdup(port));
		}
	}
	
	static inline void SetRequestUriUser(osip_message_t* msg, const char* user)
	{
		char* oldNum = osip_uri_get_username(msg->req_uri);
		if (oldNum)
		{
			SipFree(oldNum);
		}
		osip_uri_set_username(msg->req_uri, osip_strdup(user));
	}

	static inline void SetRequestUriHost(osip_message_t* msg, const char* host)
	{
		char* oldHost = osip_uri_get_host(msg->req_uri);
		if (oldHost)
		{
			SipFree(oldHost);
		}
		osip_uri_set_host(msg->req_uri, osip_strdup(host));
	}

	static inline void SetRequestUriPort(osip_message_t* msg, int port)
	{
		char* oldPort = osip_uri_get_port(msg->req_uri);
		if (oldPort)
		{
			SipFree(oldPort);
		}
		char buf[16];
		sprintf(buf, "%d", port);
		osip_uri_set_host(msg->req_uri, osip_strdup(buf));
	}

	/**************************************** Get sdp info ****************************************/

	static inline int GetSdpAttrIndex(sdp_message_t* sdp, int media_pos, const char* attrName)
	{
		int attr_pos = 0;
		while (true)
		{
			char* attr = sdp_message_a_att_field_get(sdp, media_pos, attr_pos);
			if (attr)
			{
				if(osip_strcasecmp(attr, attrName) == 0)
					return attr_pos;
			}
			else
			{
				break;
			}
			attr_pos++;
		}
		return -1;
	}

	static inline int GetSdpMediaIndex(sdp_message_t* sdp, const char* media)
	{
		int media_pos = 0;
		char* media_name = NULL;
		while (true)
		{
			media_name = sdp_message_m_media_get(sdp, media_pos);
			if (media_name)
			{
				if (!osip_strcasecmp(media_name, media))
					return media_pos;
			}
			else
			{
				break;
			}
			media_pos++;
		};
		return -1;
	}

	static inline SdpMediaMode GetSdpMediaMode(sdp_message_t* sdp, const char* media)
	{
		int media_pos = GetSdpMediaIndex(sdp, media);
		if (media_pos >= 0)
		{
			int attr_pos = 0;
			while (true)
			{
				char* attr = sdp_message_a_att_field_get(sdp, media_pos, attr_pos++);
				if (attr)
					return ParseStringToSdpMediaMode(attr);
				else
					break;
			}
		}
		return SdpMediaMode::INACTIVE;
	}

	static inline SdpMediaMode GetSdpVideoMode(sdp_message_t* sdp)
	{
		return GetSdpMediaMode(sdp, "video");
	}

	static inline SdpMediaMode GetSdpAduioMode(sdp_message_t* sdp)
	{
		return GetSdpMediaMode(sdp, "audio");
	}

	static inline const char* GetSdpPlayModeString(sdp_message_t* sdp)
	{
		return sdp_message_s_name_get(sdp);
	}

	static inline SdpPlayMode GetSdpPlayMode(sdp_message_t* sdp)
	{
		char* s_name = sdp_message_s_name_get(sdp);
		if (s_name)
			return ParseStringToSdpPlayMode(s_name);

		return SdpPlayMode::NOT_DEFINE;
	}

	static inline int GetSdpVideoPort(sdp_message_t* sdp)
	{
		int n_port = 0;
		int media_pos = GetSdpMediaIndex(sdp, "video");
		if (media_pos >= 0)
		{
			char* s_port = sdp_message_m_port_get(sdp, media_pos);
			n_port = osip_atoi(s_port);
		}
		return n_port;
	}

	static inline bool GetSdpMuxInfo(sdp_message_t* sdp, unsigned int& muxid, int& muxPort)
	{
		int pos = GetSdpAttrIndex(sdp, -1, "rtpport-mux");
		if (pos >= 0)
		{
			int id_pos = GetSdpAttrIndex(sdp, -1, "muxid");
			if (id_pos >= 0)
			{
				char* val = sdp_message_a_att_value_get(sdp, -1, id_pos);
				if (val != nullptr)
					sscanf(val, "%u", &muxid);
			}
			muxPort = GetSdpVideoPort(sdp);
			return true;
		}
		return false;
	}

	static inline const char* GetSdpConnectionHost(sdp_message_t* sdp)
	{
		char* host = sdp_message_c_addr_get(sdp, -1, 0);
		if (host)
		{
			return host;
		}
		return nullptr;
	}

	static inline int GetSdpAudioPayloadType(sdp_message_t* sdp)
	{
		int media_pos = GetSdpMediaIndex(sdp, "audio");
		if (media_pos >= 0)
		{
			char* payloadType = sdp_message_m_payload_get(sdp, media_pos, 0);
			return osip_atoi(payloadType);
		}
		return -1;
	}

	static inline const char* GetSdpOldCallidWhileRecover(sdp_message_t* sdp)
	{
		int recover_pos = GetSdpAttrIndex(sdp, -1, "recover");
		if (recover_pos >= 0)
		{
			int callid_pos = GetSdpAttrIndex(sdp, -1, "old_callid");
			if (callid_pos >= 0)
			{
				return sdp_message_a_att_field_get(sdp, -1, callid_pos);
			}
		}
		return nullptr;
	}


	/**************************************** Set sdp info ****************************************/
	static inline sdp_message_t* GenerateDefaultSdp()
	{
		sdp_message_t* sdp = nullptr;
		sdp_message_init(&sdp);
		sdp_message_v_version_set(sdp, osip_strdup("0"));
		sdp_message_m_media_add(sdp, osip_strdup("video"), osip_strdup("2000"), nullptr, osip_strdup("RTP/AVP"));
		sdp_message_m_media_add(sdp, osip_strdup("audio"), osip_strdup("2002"), nullptr, osip_strdup("RTP/AVP"));
		sdp_message_t_time_descr_add(sdp, osip_strdup("0"), osip_strdup("0"));
		return sdp;
	}

	static inline sdp_message_t* GenerateSdpByMessage(const osip_message_t* msg)
	{
		size_t len = 0;
		const char* content = GetContent(msg, len);
		if (content && len > 0)
		{
			sdp_message_t* sdp = nullptr;
			sdp_message_init(&sdp);
			sdp_message_parse(sdp, content);
			return sdp;
		}
		return nullptr;
	}

	static inline void RemoveSdpModuleAttr(sdp_message_t* sdp, int mediaIndex, const char* val)
	{
		int attr_pos = GetSdpAttrIndex(sdp, mediaIndex, val);
		if (attr_pos >= 0)
		{
			sdp_message_a_attribute_del_at_index(sdp, mediaIndex, (char*)val, -1);
		}
	}

	static inline void SetSdpMediaMode(sdp_message_t* sdp, const char* media, SdpMediaMode mode)
	{
		int media_pos = GetSdpMediaIndex(sdp, media);
		if (media_pos >= 0)
		{
			SdpMediaMode oldMode = GetSdpVideoMode(sdp);
			if (oldMode != mode)
			{
				const char* oldModeStr = ParseSdpMediaModeToString(oldMode);
				const char* modeStr = ParseSdpMediaModeToString(mode);
				RemoveSdpModuleAttr(sdp, media_pos, oldModeStr);
				sdp_message_a_attribute_add(sdp, media_pos, osip_strdup(modeStr), nullptr);
			}
		}
	}

	static inline void SetSdpVideoMode(sdp_message_t* sdp, SdpMediaMode mode)
	{
		SetSdpMediaMode(sdp, "video", mode);
	}

	static inline void SetAudioMode(sdp_message_t* sdp, SdpMediaMode mode)
	{
		SetSdpMediaMode(sdp, "audio", mode);
	}

	static inline void SetSdpPlayType(sdp_message_t* sdp, SdpPlayMode mode)
	{
		const char* str = ParseSdpPlayModeToString(mode);
		sdp_message_s_name_set(sdp, osip_strdup(str));
	}

	static inline void SetSdpVideoParam(sdp_message_t* sdp, MediaCodec codec, int width, int height, int payloadType, int bitrate)
	{
		int media_pos = GetSdpMediaIndex(sdp, "video");
		if (media_pos >= 0)
		{
			char buf[64];
			sprintf(buf, "%d", bitrate ? 2000000 : bitrate);
			int max_mbps, max_fs, profile_level_id;
			GetH264FmtpParam(width, height, max_mbps, max_fs, profile_level_id);
			sdp_bandwidth_t* bw = sdp_message_bandwidth_get(sdp, media_pos, 0);
			if (bw)
			{
				SipFree(bw->b_bandwidth);
				bw->b_bandwidth = osip_strdup(buf);
			}
			else
			{
				sdp_message_b_bandwidth_add(sdp, media_pos, osip_strdup("TIAS"), osip_strdup(buf));
			}
			sdp_message_m_payload_del(sdp, media_pos, 0);
			sprintf(buf, "%d", payloadType);
			sdp_message_m_payload_add(sdp, media_pos, osip_strdup(buf));

			sprintf(buf, "%d %s/%d", payloadType, "H264", 90000);
			RemoveSdpModuleAttr(sdp, media_pos, "rtpmap");
			sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("rtpmap"), osip_strdup(buf));

			sprintf(buf, "%d profile-level-id=%x;max-mbps=%d;max-fs=%d", payloadType, profile_level_id, max_mbps, max_fs);
			RemoveSdpModuleAttr(sdp, media_pos, "fmtp");
			sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("fmtp"), osip_strdup(buf));
		}
	}

	static inline void SetSdpAudioParam(sdp_message_t* sdp, MediaCodec codec, int payloadType, int bitrate)
	{
		int media_pos = GetSdpMediaIndex(sdp, "audio");
		if (media_pos >= 0)
		{
			char buf[264];
			sprintf(buf, "%d", bitrate ? bitrate : 64000);
			sdp_bandwidth_t* bw = sdp_message_bandwidth_get(sdp, media_pos, 0);
			if (bw)
			{
				SipFree(bw->b_bandwidth);
				bw->b_bandwidth = osip_strdup(buf);
			}
			else
			{
				sdp_message_b_bandwidth_add(sdp, media_pos, osip_strdup("TIAS"), osip_strdup(buf));
			}
			sdp_message_m_payload_del(sdp, media_pos, 0);
			sprintf(buf, "%d", payloadType);
			sdp_message_m_payload_add(sdp, media_pos, osip_strdup(buf));

			if (codec == MediaCodec::CODEC_AAC)
			{
				sprintf(buf, "%d MP4A-LATM/48000", payloadType);
				RemoveSdpModuleAttr(sdp, media_pos, "rtpmap");
				sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("rtpmap"), osip_strdup(buf));
				sprintf(buf, "%d streamtype=5; profile-level-id=1; mode=AAC-hbr; sizeLength=13; indexLength=3; indexDeltaLength=3; cpresent=1", payloadType);
				RemoveSdpModuleAttr(sdp, media_pos, "fmtp");
				sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("fmtp"), osip_strdup(buf));
			}
			else
			{
				if (codec == MediaCodec::CODEC_PCMA)
				{
					sprintf(buf, "%d %s/%d", payloadType, "pcma", 8000);
				}
				else if (codec == MediaCodec::CODEC_PCMU)
				{
					sprintf(buf, "%d %s/%d", payloadType, "pcmu", 8000);
				}
				RemoveSdpModuleAttr(sdp, media_pos, "rtpmap");
				sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("rtpmap"), osip_strdup(buf));
				RemoveSdpModuleAttr(sdp, media_pos, "ptime");
				sdp_message_a_attribute_add(sdp, media_pos, osip_strdup("ptime"), osip_strdup("20"));
			}
		}
	}
	
	static inline void SetSdpVideoPort(sdp_message_t* sdp, int port)
	{
		int media_pos = GetSdpMediaIndex(sdp, "video");
		if (media_pos >= 0)
		{
			char buf[10] = { 0 };
			sprintf(buf, "%d", port);
			sdp_message_m_port_set(sdp, media_pos, osip_strdup(buf));
		}
	}

	static inline void SetSdpAudioPort(sdp_message_t* sdp, int port)
	{
		int media_pos = GetSdpMediaIndex(sdp, "audio");
		if (media_pos >= 0)
		{
			char buf[10] = { 0 };
			sprintf(buf, "%d", port);
			sdp_message_m_port_set(sdp, media_pos, osip_strdup(buf));
		}
	}

	static inline void SetSdpMuxInfo(sdp_message_t* sdp, bool bMuxFlag, unsigned int muxid, int muxPort)
	{
		RemoveSdpModuleAttr(sdp, -1, "rtpport-mux");
		RemoveSdpModuleAttr(sdp, -1, "muxid");
		if (bMuxFlag)
		{
			char buf[64];
			sprintf(buf, "%u", muxid);
			sdp_message_a_attribute_add(sdp, -1, osip_strdup("rtpport-mux"), nullptr);
			sdp_message_a_attribute_add(sdp, -1, osip_strdup("muxid"), osip_strdup(buf));
			SetSdpVideoPort(sdp, muxPort);
			SetSdpAudioPort(sdp, muxPort);
		}
	}

	static inline void SetSdpConnection(sdp_message_t* sdp, const char* devID, const char* host)
	{
		sdp_message_c_connection_add(sdp, -1, osip_strdup("IN"), osip_strdup("IP4"), osip_strdup(host), nullptr, nullptr);
		sdp_message_o_origin_set(sdp, osip_strdup(devID), osip_strdup(devID), osip_strdup(devID), osip_strdup("IN"), osip_strdup("IP4"), osip_strdup(host));
	}

	static inline void SetSdpRecoverFlag(sdp_message_t* sdp, bool recover, const char* old_callid)
	{
		if (recover)
		{
			sdp_message_a_attribute_add(sdp, -1, osip_strdup("recover"), NULL);
			sdp_message_a_attribute_add(sdp, -1, osip_strdup("old_callid"), osip_strdup(old_callid));
		}
	}
}

#endif // !__SIP_MESSAGE_H__