// OscFileDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "OscFormatInfo.h"

int main()
{
	OscFormatInfo formatinfo;
	std::fstream file(".\\6-21+12-0-49+1.osc", std::fstream::in | std::fstream::binary); 
	//std::fstream file(".\\4-28+13-25-40+01.osc", std::fstream::in | std::fstream::binary);
	if (formatinfo.read(file))
	{
		std::cout << "Read file success\n";

		std::cout << " type " << formatinfo.oscfmt.type << "\n";
		std::cout << " Version " << formatinfo.oscfmt.Version << "\n";
		std::cout << " nSamplesPerSec " << formatinfo.oscfmt.nSamplesPerSec << "\n";
		std::cout << " nChannels " << (unsigned int)formatinfo.oscfmt.nChannels << "\n";
		std::cout << " nBits " << (unsigned int)formatinfo.oscfmt.nBits << "\n";
		std::cout << " nBlockAlign " << formatinfo.oscfmt.nBlockAlign << "\n";
		std::cout << std::hex << " nFormat " << formatinfo.oscfmt.nFormat << "\n" ;
		std::cout << std::dec << " Trigger_point " << formatinfo.oscfmt.Trigger_point << "\n";
		std::cout << "\n";

		//zoompbias[0]
		std::cout << " zoompbias[0].type " << formatinfo.zoompbias[0]->type << "\n";
		std::cout << " zoompbias[0].nBoxing " << formatinfo.zoompbias[0]->nBoxing << "\n";
		std::cout << " zoompbias[0].nZoom " << formatinfo.zoompbias[0]->nZoom << "\n";
		std::cout << " zoompbias[0].nBias " << formatinfo.zoompbias[0]->nBias << "\n";
		std::cout << "\n";

		if (formatinfo.oscfmt.nChannels == 2)
		{
			std::cout << " zoompbias[1].type " << formatinfo.zoompbias[1]->type << "\n";
			std::cout << " zoompbias[1].nBoxing " << formatinfo.zoompbias[1]->nBoxing << "\n";
			std::cout << " zoompbias[1].nZoom " << formatinfo.zoompbias[1]->nZoom << "\n";
			std::cout << " zoompbias[1].nBias " << formatinfo.zoompbias[1]->nBias << "\n";
			std::cout << "\n";
		}

		//DATA
		std::cout << " data.type " << formatinfo.data.type << "\n";
		std::cout << " data.nDatasSize " << formatinfo.data.nDatasSize << "\n";

		//读取前10个数据
		const unsigned char channels = formatinfo.GetChannels();
		const unsigned char datasize = formatinfo.GetDataSize();
		unsigned int length = 10;
		char* buffer = new char[length * channels * datasize];

		unsigned int data_start = formatinfo.GetDatasOffset();
		file.seekg(formatinfo.GetDatasOffset() + data_start, std::ios_base::beg);
		file.read(buffer, length * channels * datasize);

		for (unsigned int h = 0; h < 10; h++)
		{
			if (formatinfo.oscfmt.nChannels == 2)
			{
				if (formatinfo.oscfmt.nFormat == uint8Sample)
				{
					std::cout << ((unsigned char*)buffer)[h * 2] * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << " ";
					std::cout << ((unsigned char*)buffer)[h * 2+1] * formatinfo.zoompbias[1]->nZoom + formatinfo.zoompbias[1]->nBias << "\n";
				}
				else if (formatinfo.oscfmt.nFormat == int8Sample)
				{
					std::cout << (int)(((char*)buffer)[h * 2]) * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << " ";
					std::cout << (int)(((char*)buffer)[h * 2 + 1]) * formatinfo.zoompbias[1]->nZoom + formatinfo.zoompbias[1]->nBias << "\n";
				}
				else if (formatinfo.oscfmt.nFormat == uint16Sample)
				{
					std::cout << ((unsigned short*)buffer)[h * 2] * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << " ";
					std::cout << ((unsigned short*)buffer)[h * 2 + 1] * formatinfo.zoompbias[1]->nZoom + formatinfo.zoompbias[1]->nBias << "\n";
				}
			}
			else
			{
				if (formatinfo.oscfmt.nFormat == uint8Sample)
				{
					std::cout << ((unsigned char*)buffer)[h] * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << "\n";
				}
				else if (formatinfo.oscfmt.nFormat == int8Sample)
				{
					std::cout << (int)(((char*)buffer)[h]) * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << "\n";
				}
				else if (formatinfo.oscfmt.nFormat == uint16Sample)
				{
					std::cout << ((unsigned short*)buffer)[h] * formatinfo.zoompbias[0]->nZoom + formatinfo.zoompbias[0]->nBias << "\n";
				}
			}
		}

		delete[] buffer;
	}
    else
		std::cout << "Read file failed\n";
}