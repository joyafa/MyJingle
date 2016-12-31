//----------------------------------------------------------------------//
//                                                                      //
// MyJingle is a GoogleTalk compatible VoIP Client based on Jingle      //
//                                                                      //
// Copyright (C) 2006  bluehands GmbH & Co.mmunication KG               //
//                                                                      //
// This program is free software; you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation; either version 2 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful, but  //
// WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     //
// General Public License for more details.                             //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA //
//                                                                      //
//----------------------------------------------------------------------//

#include "Packet.h"

#if defined (WIN32)
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <errno.h>
#endif

Contributor::Contributor(int* id, int* timestamp)
{
	m_ContributorId = id;
	m_Timestamp = timestamp;
}

Contributor::~Contributor()
{
}

int Contributor::TimeStamp()
{
	return ntohl(*((unsigned int*)m_Timestamp));
}

void Contributor::TimeStamp(int time)
{
	*m_Timestamp = htonl(time);
}

int Contributor::ContributorId()
{
	return ntohl(*((unsigned int*)m_ContributorId));
}

void Contributor::ContributorId(int id)
{
	*m_ContributorId = htonl(id);
}

Packet::Packet(char *packet, int len, unsigned char nContributors):
m_Packet(0)
,m_BufLen(0)
,m_RecieveTime(0)
{
	SetPacket(packet, len);
	ContributorsCount(nContributors);
}

Packet::Packet(char *packet, int len)
{
	m_Packet = (unsigned char *)packet;
	m_BufLen = len;
}

Packet::~Packet(void)
{
	delete m_Packet;
}

bool Packet::IsCommand() const
{
	return !IsData();
}

const char* Packet::GetPacket() const
{
	return (char*)m_Packet;
}

bool Packet::IsData() const
{
	bool isData = true; // hack
	RTPHeader* header = (RTPHeader*)m_Packet;
	if((header->Flags2 & 64) == 64) //01000000
	{
		isData = true;
	}
	return isData;
}

int Packet::DataLength() const
{
	int length = m_BufLen;
	if(IsData())
	{
		length -= GetDataPacketHeaderSize();
	}
	if(IsCommand())
	{
		length -= GetCommandPacketHeaderSize();
	}
	if(Padding())
	{
		length -= (m_Packet[length-1])*2;
		length -= 1;
	}
	return length;
}

const char* Packet::Data() const
{
	return RwData();
}

int Packet::RwDataLength() const
{
	int headerSize=0;
	char* data = (char *)(m_Packet);
	if(IsData())
	{
		headerSize = GetDataPacketHeaderSize();
	}
	if(IsCommand())
	{
		headerSize = GetCommandPacketHeaderSize();
	}

	return m_BufLen - headerSize;
}

char* Packet::RwData() const
{
	int headerSize=0;
	char* data = (char *)(m_Packet);
#if 0
	if(IsData())
	{
		headerSize = GetDataPacketHeaderSize();
	}
	if(IsCommand())
	{
		headerSize = GetCommandPacketHeaderSize();
	}
	unsigned char contCount = ContributorsCount();
	data += contCount * 4;
	data += headerSize;
#endif
	return data;
}

int Packet::PacketLength() const
{
	return m_BufLen;
}

unsigned long Packet::RecieveTime() const
{
	return m_RecieveTime;
}

void Packet::RecieveTime(unsigned long timestamp)
{
	m_RecieveTime = timestamp;
}

unsigned short Packet::SequenceNumber() const
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	return ntohs( header->SequenceNumber );
}


void Packet::SequenceNumber(unsigned short nSeq)
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	header->SequenceNumber = htons(nSeq);
}

unsigned char Packet::ContributorsCount() const
{
	unsigned char contributors = 0;
	unsigned char myContrib = m_Packet[0];
	for(int i = 0; i<=3; i++)
	{
		if((myContrib & 1) == 1)
		{
			short square = 1;
			for(int j = 0; j < i; j++)
			{
				square *= 2;
			}
			contributors += square;
		}
		myContrib = myContrib >> 1;
	}
	return contributors;
}

void Packet::ContributorsCount(unsigned char nContributors)
{
	if(nContributors == 0)
	{
		Padding(false);
	}
	else
	{
		Padding(true);
	}

	m_Packet[0] = m_Packet[0] & 240;	// 11110000
	nContributors = nContributors & 15;	// 00001111
	m_Packet[0] = m_Packet[0] | nContributors;
	m_Packet[m_BufLen-1] = nContributors * 4;
}

void Packet::Timestamp(unsigned long timeStamp)
{
	*((unsigned long *)(&m_Packet[4])) = htonl(timeStamp);
}

unsigned long Packet::Timestamp() const
{
	return ntohl((unsigned long)(&m_Packet[4]));
}
void Packet::Silence(bool)
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	header->Flags2 = header->Flags2 | 128;  // 10000000
}

bool Packet::Silence() const
{
	bool silence = true;
	RTPHeader* header = (RTPHeader*)m_Packet;
	if((header->Flags2 & 128) == 0) // 10000000
	{
		silence = false;
	}
	return silence;
}

int Packet::SourceId() const
{
	return (int)(&m_Packet[8]);
}

void Packet::SourceId(int sourceId)
{
	*((int *)(&m_Packet[8])) = sourceId;
}

void Packet::SetPacket(char* packet, int length)
{
	m_Packet = (unsigned char*)packet;
	m_BufLen = length;
	m_Packet[0] = 128; // 10000000
	m_Packet[1] = 0;  // 0
}

void Packet::IsData(bool isData)
{
	IsCommand(!isData);
}

void Packet::IsCommand(bool isCommand)
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	if(isCommand)
	{
		header->Flags2 = (header->Flags2 & 128) | 64; // x1000000
	}
	else
	{
		header->Flags2 = (header->Flags2 & 128); // x0000000
	}
}

void Packet::Padding(bool padding)
{
	if(padding)
	{
		m_Packet[0] = m_Packet[0] | 32;		// 00100000
	}
	else
	{
		m_Packet[0] = m_Packet[0] & 223;	// 11011111
	}
}

bool Packet::Padding() const
{
	bool pad = false;
	if((m_Packet[0] & 32) == 32) // 00100000
	{
		pad = true;
	}
	return pad;
}

int Packet::GetDataPacketHeaderSize()
{
	return sizeof(RTPHeader);
}

int Packet::GetCommandPacketHeaderSize()
{
	return 64;
}

Contributor* Packet::GetNewContributor(unsigned char number)
{
	number = number % 16;
	unsigned char nContributors = ContributorsCount();
	if(number == 0 || number > nContributors)
	{
		return 0;
	}
	int* id = ((int*)(&m_Packet[8])) + number;
	int* timestamp = ((int*)(&m_Packet[m_BufLen-5])) - nContributors + number;
	return new Contributor(id, timestamp);
}


int Packet::GetDesiredDataPacketSize(int dataLength, unsigned char nContributors)
{
	int size = dataLength;
	size += GetDataPacketHeaderSize();
	nContributors = nContributors%16;
	if(nContributors != 0)
	{
		size += 1;
		size += nContributors * 8;
	}
	return size;
}
Packet* Packet::CreateNewDataPacket(int dataLength, unsigned char nContributors)
{
	int size = GetDesiredDataPacketSize(dataLength, nContributors);
	char* buffer = new char[size];
	Packet* packet = new Packet(buffer, size, nContributors);
	packet->IsData(true);
	return packet;
}


int Packet::GetPacketsFromBuffer(char* buffer, int length, Packet** packets)
{
	return GetPacketsFromBuffer(buffer, length, packets, 0);
}

Packet* Packet::TmpGetPacketFromBuffer(char* buffer, int length)
{
	return new Packet(buffer, length);
}


int Packet::GetPacketsFromBuffer(char* buffer, int length, Packet** packets, int packetsCount)
{
	//Packet* packet = new Packet(buffer, length);
	//
	//if(packet->IsValid())
	//{
	//	packetsCount++;
	//	int packetLength = packet->Length();
	//	if(packetLength < length)
	//	{
	//		int myPos = packetsCount;
	//		packetsCount = GetPacketsFromBuffer(
	//			buffer + packetLength,
	//			length - packetLength,
	//			packets,
	//			packetsCount);
	//		Packet* myPackets = *packets;
	//		myPackets[myPos] = *packet;
	//
	//	}
	//	else
	//	{
	//		//*packets = new Packet[packetsCount];
	//		//Packet* myPackets = *packets;
	//		//myPackets[packetsCount] = *packet;
	//	}
	//}
	//return packetsCount;
	return 0;
}

CommandPacket::CommandType CommandPacket::GetCommandType()
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	return (CommandType)(header->Flags2 & 127); //ignore first bit and cast the rest to CommandType
}

void CommandPacket::SetCommandType(CommandPacket::CommandType type)
{
	RTPHeader* header = (RTPHeader*)m_Packet;
	header->Flags2 = (header->Flags2 & 128) | ((char) type);
}

void* CommandPacket::GetCommandData(int* length)
{
	*length = this->DataLength();
	return this->RwData();
}