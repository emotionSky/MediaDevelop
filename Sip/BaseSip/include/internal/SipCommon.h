#ifndef __SIP_COMMON_H__
#define __SIP_COMMON_H__

enum class SdpMediaMode
{
	NOT_DEFINE,
	RECV_ONLY,
	SEND_ONLY,
	SEND_RECV,
	INACTIVE,
};

SdpMediaMode ParseStringToSdpMediaMode(const char* str);
const char* ParseSdpMediaModeToString(SdpMediaMode mode);

enum class SdpPlayMode
{
	NOT_DEFINE,
	PLAY,
	CALL,
	PLAY_CALL,
};

SdpPlayMode ParseStringToSdpPlayMode(const char* str);
const char* ParseSdpPlayModeToString(SdpPlayMode mode);

enum class MediaCodec
{
	CODEC_NONE,

	/*  ”∆µ */
	CODEC_H264,
	CODEC_H265,
	CODEC_AVS2,

	/* “Ù∆µ */
	CODEC_AAC,
	CODEC_AAC_LATM_INBAND,
	CODEC_AAC_LATM_OUTBAND,
	CODEC_PCMA,
	CODEC_PCMU,
	CODEC_G729,
	CODEC_OPUS,
};


void GetH264FmtpParam(int width, int height, int& max_mbps, int& max_fs, int& profile_level_id);

#endif // !__SIP_COMMON_H__

