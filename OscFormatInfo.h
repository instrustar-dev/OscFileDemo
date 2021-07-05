#pragma once
#include <assert.h>
#include <memory.h>
#include <fstream>

#define MAX_CHANNELS 2
#pragma pack(push,1)

typedef enum {
	errorSample = 0x00000000,
	int8Sample = 0x00010001,
	uint8Sample = 0x00010002,
	int16Sample = 0x00020001,
	uint16Sample = 0x00020002,
	int24Sample = 0x00030001,
	int32Sample = 0x0004000A,
	uint32Sample = 0x0004000B,
	floatSample = 0x0004000F,
	doubleSample = 0x0008000F
} sampleFormat;
#define SAMPLE_SIZE(mSampleFormat) (mSampleFormat>>16)

typedef enum SHIYUBX_SOURCE {
	SHIYUBX_SOURCE_ZERO = 0x00000000,
	SHIYUBX_SOURCE_CH1 = 0x00000001,
	SHIYUBX_SOURCE_CH2 = 0x00000002,
	SHIYUBX_SOURCE_REF = 0x00000004
} SHIYUBX_SOURCE;

//注意：定义此类结构体要考虑字节对齐
typedef struct tOscFormat
{
	char             type[6];         /* 类型 "OSC    "*/
    unsigned short  Version;         /* 版本 */
	unsigned int    nSamplesPerSec;  /* 采样率 */
    unsigned char   nChannels;       /* 通道数 */
	unsigned char   nBits;           /* bits */
    unsigned short  nBlockAlign;     /* 块字节数 */
	sampleFormat      nFormat;         /* 数据格式 */
	unsigned int    Trigger_point;    /* 2.0  触发点 */     

	explicit tOscFormat()
		: Version(2)   //1.0 mV  2.0 V
		, nSamplesPerSec(0)
		, nChannels(0)
		, nBits(0)
		, nBlockAlign(0)
		, nFormat(errorSample)
		, Trigger_point(0)
	{
		memcpy(type,"OSC    ",6);
	};

	tOscFormat(const unsigned int sample, const unsigned char channel, const unsigned char bit, const sampleFormat format)
		: Version(2)
		, nSamplesPerSec(sample)
		, nChannels(channel)
		, nBits(bit)
		, nBlockAlign(nChannels*nBits/8)
		, nFormat(format)
		, Trigger_point(0)
	{
		memcpy(type,"OSC    ",6);
		assert(nChannels%8==0);
	};

} OscFormat, *POscFormat;

//通道的放大倍数和偏置
typedef struct tOscZoomBias
{
	char            type[8];         /* 类型 "ZOOMBIAS"*/ 
	SHIYUBX_SOURCE         nBoxing;          //时域波形
	double          nZoom;           /* 放大倍数 */
	double          nBias;           /* 偏置 */

	explicit tOscZoomBias()
		: nBoxing(SHIYUBX_SOURCE_ZERO)
		, nZoom(0.0)
		, nBias(0.0)
	{
	}

	tOscZoomBias(const SHIYUBX_SOURCE boxing, const double Zoom, const double Bias)
		: nBoxing(boxing)
		, nZoom(Zoom)
		, nBias(Bias)
	{
		memcpy(type,"ZOOMBIAS",8);
	};

	void set(const SHIYUBX_SOURCE boxing, const double Zoom, const double Bias)
	{
		nBoxing=boxing;
		nZoom=Zoom;
		nBias=Bias;
	}
private:
	tOscZoomBias& operator=(const tOscZoomBias& copy);
} OscZoomBias;

//通道的放大倍数和偏置
typedef struct tOscData
{
	char            type[4];         /* 类型 "DATA"*/
	unsigned int    nDatasSize;      /* 数据字节长度 */

	explicit tOscData()
		: nDatasSize(0)
	{
		memcpy(type,"DATA",4);
	};

} OscData;

#pragma pack(pop)

typedef struct tOscFormatInfo
{
	explicit tOscFormatInfo()
		: oscfmt()
		, data()
	{
		assert(sizeof(tOscFormat) == 24);
		assert(sizeof(OscZoomBias) == 28);
		assert(sizeof(tOscData) == 8);

		for (int i = 0; i < MAX_CHANNELS; i++)
			zoompbias[i] = NULL;
	}

	~tOscFormatInfo()
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			if (zoompbias[i] != NULL)
				delete zoompbias[i];
		}
	}


	unsigned char GetChannels() const {return oscfmt.nChannels;};
	unsigned int GetSample() const {return oscfmt.nSamplesPerSec;};
	//unsigned char GetBits() const {return oscfmt.nBits;};
	sampleFormat GetSampleFormat() const {return oscfmt.nFormat;};
	unsigned int GetDataSize()const {return SAMPLE_SIZE(oscfmt.nFormat);};

	//获取data记录文件大小字节位置
	unsigned int GetDataSizeOffset() const {return sizeof(OscFormat)+oscfmt.nChannels*sizeof(OscZoomBias)+4;};
	//获取data记录开始的位置
	unsigned int GetDatasOffset() const {return sizeof(OscFormat)+oscfmt.nChannels*sizeof(OscZoomBias)+sizeof(OscData);};
	//获取数据总字节数
	unsigned int GetDatasByteSize() const {return data.nDatasSize;};

	//将头读入
	bool read(std::fstream& file)
	{
		memset((char*)(&oscfmt), 0, sizeof(OscFormat));
		file.read((char*)(&oscfmt), sizeof(OscFormat));
		if (strncmp(oscfmt.type, "OSC", 3) != 0)
			return false;

		if (zoompbias[0] == NULL)
			zoompbias[0] = new OscZoomBias();
		file.read((char*)(zoompbias[0]), sizeof(OscZoomBias));
		if (strncmp(zoompbias[0]->type, "ZOOMBIAS", 8) != 0)
			return false;

		if (oscfmt.nChannels == 2)
		{
			if (zoompbias[1] == NULL)
				zoompbias[1] = new OscZoomBias();
			file.read((char*)(zoompbias[1]), sizeof(OscZoomBias));
			if (strncmp(zoompbias[1]->type, "ZOOMBIAS", 8) != 0)
				return false;
		}

		file.read((char*)(&data), sizeof(OscData));
		if (strncmp(data.type, "DATA", 4) != 0)
			return false;
		return true;
	}

	OscFormat oscfmt;
	OscZoomBias *zoompbias[MAX_CHANNELS];
	OscData data;
} OscFormatInfo;
