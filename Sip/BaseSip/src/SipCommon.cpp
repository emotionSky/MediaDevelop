#include <cstring>
#include <osipparser2/osip_port.h>
#include "internal/SipCommon.h"

SdpMediaMode ParseStringToSdpMediaMode(const char* str)
{
	if (osip_strcasecmp(str, "recvonly") == 0)
	{
		return SdpMediaMode::RECV_ONLY;
	}
	else if (osip_strcasecmp(str, "sendonly") == 0)
	{
		return SdpMediaMode::SEND_ONLY;
	}
	else if (osip_strcasecmp(str, "sendrecv") == 0)
	{
		return SdpMediaMode::SEND_RECV;
	}
	else if (osip_strcasecmp(str, "inavtive") == 0)
	{
		return SdpMediaMode::INACTIVE;
	}
	return SdpMediaMode::NOT_DEFINE;
}

const char* ParseSdpMediaModeToString(SdpMediaMode mode)
{
	switch (mode)
	{
	case SdpMediaMode::RECV_ONLY:
		return "recvonly";
		break;
	case SdpMediaMode::SEND_ONLY:
		return "sendonly";
		break;
	case SdpMediaMode::SEND_RECV:
		return "sendrecv";
		break;
	case SdpMediaMode::INACTIVE:
		return "inavtive";
		break;
	default:
		break;
	}
	return "inavtive"; //return "none";
}

SdpPlayMode ParseStringToSdpPlayMode(const char* str)
{
	if (osip_strcasecmp(str, "play") == 0)
	{
		return SdpPlayMode::PLAY;
	}
	else if (osip_strcasecmp(str, "call") == 0)
	{
		return SdpPlayMode::CALL;
	}
	else if (osip_strcasecmp(str, "play-call") == 0)
	{
		return SdpPlayMode::PLAY_CALL;
	}
	return SdpPlayMode::NOT_DEFINE;
}

const char* ParseSdpPlayModeToString(SdpPlayMode mode)
{
	switch (mode)
	{
	case SdpPlayMode::PLAY:
		return "play";
		break;
	case SdpPlayMode::CALL:
		return "call";
		break;
	case SdpPlayMode::PLAY_CALL:
		return "play-call";
		break;
	default:
		break;
	}

	return "play"; //return "none";
}

typedef struct h264Resolution2Fmtp
{
	int width;
	int height;
	int profile;
	int max_mbps;
	int max_fs;
	int profile_level_id;
} h264Resolution2Fmtp_s;

static h264Resolution2Fmtp_s h264FmptMap[] = 
{
	{3840, 2160, 51, 972000, 32400, 0x640033},
	{1920, 1200, 50, 270000, 9000, 0x640032},
	{1920, 1080, 40, 243000, 8100, 0x640028},
	{1280, 720, 31, 108000, 3600, 0x64001F},
	{720, 576, 31, 48600, 1620, 0x64001F},
	{704, 576, 31, 47520, 1584, 0x64001F},
	{352, 288, 20, 11880, 396, 0x420014},
	{176, 144, 11, 2970, 99, 0x42000B} 
};

void GetH264FmtpParam(int width, int height, int& max_mbps, int& max_fs, int& profile_level_id)
{
	if (width <= 176)
	{
		width = 176;
		height = 144;
	}
	else if (width <= 352)
	{
		width = 352;
		height = 288;
	}
	else if (width <= 704)
	{
		width = 704;
		height = 576;
	}
	else if (width <= 720)
	{
		width = 720;
		height = 576;
	}
	else if (width <= 1024)
	{
		width = 1024;
		height = 768;
	}
	else if (width <= 1280)
	{
		width = 1280;
		height = 720;
	}
	else if (width <= 1920)
	{
		width = 1920;
		if (height <= 1080)
		{
			height = 1080;
		}
		else
		{
			height = 1200;
		}
	}
	else
	{
		width = 3840;
		height = 2160;
	}

	const int count = sizeof(h264FmptMap) / sizeof(h264Resolution2Fmtp_s);
	int index = 0;
	for (; index < count; index++)
	{
		h264Resolution2Fmtp_s& fmtp = h264FmptMap[index];
		if (fmtp.width == width && fmtp.height == height)
		{
			max_mbps = fmtp.max_mbps;
			max_fs = fmtp.max_fs;
			profile_level_id = fmtp.profile_level_id;
			break;
		}
	}
}
