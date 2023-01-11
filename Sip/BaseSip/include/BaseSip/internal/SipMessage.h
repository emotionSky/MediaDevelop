#ifndef __SIP_MESSAGE_H__
#define __SIP_MESSAGE_H__

#include <osipparser2/osip_message.h>
#include <osipparser2/sdp_message.h>
#include <osipparser2/osip_parser.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include "SipCommon.h"

namespace sip
{
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

	/*
	 * @brief 获取 from 
	 * @param[in] msg  sip消息结构体
	 * @return from，形如 alice@12.1.12.3:5060
	 */
	static inline std::string GetFrom(const osip_message_t* msg)
	{
		std::string res;
		osip_from_t* from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(from);
		if (from && uri)
		{
			if (uri->username)
			{
				res += uri->username;
				res += "@";
			}
			if (uri->host)
			{
				res += uri->host;
				res += ":";
			}
			if (uri->port)
			{
				res += uri->port;
			}
		}
		return res;
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

	/*
	 * @brief 获取 to
	 * @param[in] msg  sip消息结构体
	 * @return to，形如 alice@12.1.12.3:5060
	 */
	static inline std::string GetTo(const osip_message_t* msg)
	{
		std::string res;
		osip_from_t* to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(to);
		if (to && uri)
		{
			if (uri->username)
			{
				res += uri->username;
				res += "@";
			}
			if (uri->host)
			{
				res += uri->host;
				res += ":";
			}
			if (uri->port)
			{
				res += uri->port;
			}
		}
		return res;
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

	static inline const char* GetExpires(const osip_message_t* msg)
	{
		osip_header_t* expires = nullptr;
		osip_message_get_expires(msg, 0, &expires);
		if (expires)
		{
			return expires->hvalue;
		}
		return "";
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

	/*
	 * @brief 设置消息中的content type
	 * @param[in] msg    sip消息结构体
	 * @param[in] ctype  content_type，一般形如 application/xml
	 * @return void
	 */
	static inline void SetContentType(osip_message_t* msg, const char* ctype)
	{
		if (!msg || !ctype || ctype[0] == '\0')
			return;
		osip_message_set_content_type(msg, ctype);
	}

	/*
	 * @brief 设置消息中的encoding
	 * @param[in] msg       sip消息结构体
	 * @param[in] encoding  encoding，一般形如 zip
	 * @return void
	 */
	static inline void SetContentEncoding(osip_message_t* msg, const char* encoding)
	{
		if (!msg || !encoding || encoding[0] == '\0')
			return;

		osip_message_set_content_encoding(msg, encoding);
	}

	/*
	 * @brief 设置消息中的content
	 * @param[in] msg      sip消息结构体
	 * @param[in] content  消息体
	 * @param[in] len      消息体的长度
	 * @return void
	 */
	static inline void SetContent(osip_message_t* msg, const char* content, size_t len)
	{
		if (!msg || !content || content[0] == '\0' || len == 0)
			return;

		osip_body_t* body = nullptr;
		osip_message_get_body(msg, 0, &body);
		if (body)
		{
			osip_free(body->body);
			body->body = nullptr;
			osip_body_parse(body, content, len);
		}
		else
		{
			osip_message_set_body(msg, content, len);
		}
	}

	/*
	 * @brief 设置消息中的from字段
	 * @param[in] msg   sip消息结构体
	 * @param[in] from  from的用户名、IP和端口，形如 alice@12.1.12.3:5060
	 * @return void
	 */
	static inline void SetFrom(osip_message_t* msg, const char* from)
	{
		if (!msg || !from || from[0] == '\0')
			return;

		osip_contact_t* sip_from = osip_message_get_from(msg);
		osip_uri_t* uri = osip_from_get_url(sip_from);
		if (sip_from && uri)
		{
			char user[64], host[32], port[16];
			if (sscanf(from, "%[^@]@%[^:]:%s", user, host, port) == 3)
			{
				osip_free(uri->username);
				uri->username = osip_strdup(user);
				osip_free(uri->host);
				uri->host = osip_strdup(host);
				osip_free(uri->port);
				uri->port = osip_strdup(port);
			}
		}
	}
		
	/*
	 * @brief 设置消息中的to字段
	 * @param[in] msg   sip消息结构体
	 * @param[in] to    to的用户名、IP和端口，形如 alice@12.1.12.3:5060
	 * @return void
	 */
	static inline void SetTo(osip_message_t* msg, const char* to)
	{
		if (!msg || !to || to[0] == '\0')
			return;

		osip_contact_t* sip_to = osip_message_get_to(msg);
		osip_uri_t* uri = osip_from_get_url(sip_to);
		if (sip_to && uri)
		{
			char user[64], host[32], port[16];
			if (sscanf(to, "%[^@]@%[^:]:%s", user, host, port) == 3)
			{
				osip_free(uri->username);
				uri->username = osip_strdup(user);
				osip_free(uri->host);
				uri->host = osip_strdup(host);
				osip_free(uri->port);
				uri->port = osip_strdup(port);
			}
		}
	}
	
	/*
	 * @brief 设置消息中的via字段
	 * @param[in] msg  sip消息结构体
	 * @param[in] via  via的IP和端口，形如 12.1.12.3:5060
	 * @return void
	 */
	static inline void SetVia(osip_message_t* msg, const char* via)
	{
		if (!msg || !via || via[0] == '\0')
			return;

		osip_via_t* sip_via = nullptr;
		osip_message_get_via(msg, 0, &sip_via);
		if (sip_via)
		{
			char host[64], port[16];
			if (sscanf(via, "%s:%s", host, port) == 2)
			{
				osip_free(sip_via->host);
				sip_via->host = osip_strdup(host);
				osip_free(sip_via->port);
				sip_via->port = osip_strdup(port);
			}
		}
	}

	/*
	 * @brief 设置消息中的contact字段
	 * @param[in] msg      sip消息结构体
	 * @param[in] contact  contact的用户名、IP和端口，形如 alice@12.1.12.3:5060
	 * @return void
	 */
	static inline void SetContact(osip_message_t* msg, const char* contact)
	{
		if (!msg || !contact || contact[0] == '\0')
			return;

		osip_contact_t* sip_contact = nullptr;
		osip_message_get_contact(msg, 0, &sip_contact);
		osip_uri_t* uri = osip_contact_get_url(sip_contact);
		if (sip_contact && uri)
		{
			char user[64], host[32], port[16];
			if (sscanf(contact, "%[^@]@%[^:]:%s", user, host, port) == 3)
			{
				osip_free(uri->username);
				uri->username = osip_strdup(user);
				osip_free(uri->host);
				uri->host = osip_strdup(host);
				osip_free(uri->port);
				uri->port = osip_strdup(port);
			}
		}
	}

	/*
	 * @brief 设置 subject 
	 * @param[in] subject 多数情况下形如 <sip:alice@10.1.1.21:1234>
	 * @return void
	 */
	static inline void SetSubject(osip_message_t* msg, const char* subject)
	{
		if (!msg || !subject || subject[0] == '\0')
			return;
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
	 * @param[in] refresher   refresher的内容，形如 uac uas
	 * @param[in] expire      呼叫的截止时间
	 * @return void
	 */
	static inline void SetSessionExpires(osip_message_t* msg, const char* refresher, int expire)
	{
		char session_exp[32] = { 0 };
		sprintf(session_exp, "%d;refresher=%s", expire, refresher);
		osip_message_set_header(msg, "Session-Expires", session_exp);
	}

	/*
	 * @brief 设置route的uri
	 * @param[in] msg   sip消息结构体
	 * @param[in] uri   修改后的uri，形如 bob@12.1.12.3:5060
	 * @return void
	 */
	static inline void SetRequestUri(osip_message_t* msg, const char* uri)
	{
		if (!msg || !uri || uri[0] == '\0')
			return;

		char user[64], host[32], port[16];
		if (sscanf(uri, "%[^@]@%[^:]:%s", user, host, port) == 3)
		{
			osip_free(msg->req_uri->username);
			msg->req_uri->username = osip_strdup(user);
			osip_free(msg->req_uri->host);
			msg->req_uri->host = osip_strdup(host);
			osip_free(msg->req_uri->host);
			msg->req_uri->host = osip_strdup(host);
		}
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
				osip_free(bw->b_bandwidth);
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
				osip_free(bw->b_bandwidth);
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