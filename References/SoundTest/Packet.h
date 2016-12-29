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

#if !defined(Packet_h_INCLUDED)
#define Packet_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "beat.h"

class Contributor
{
	friend class Packet;
public:
	int ContributorId();
	void ContributorId(int id);
	int TimeStamp();
	void TimeStamp(int timeStamp);
	~Contributor();
private:
	Contributor(int* id, int* timestamp);
	int* m_ContributorId;
	int* m_Timestamp;
};

class Packet
{
public:
	~Packet(void);
	bool IsCommand() const;
	bool IsData() const;
	int DataLength() const;
	int PacketLength() const;
	const char* GetPacket() const;
	const char* Data() const;
	char* RwData() const;
	unsigned short SequenceNumber() const;
	unsigned long Timestamp() const;
	void Timestamp(unsigned long TimeStamp);
	int SourceId() const;
	void SourceId(int sourceId);
	bool Silence() const;
	void Silence(bool silence);
	void SequenceNumber(unsigned short nSeq);
	Contributor* GetNewContributor(unsigned char number);
	static Packet* CreateNewDataPacket(int dataLength, unsigned char nContributors);
	static int GetDesiredDataPacketSize(int dataLength, unsigned char nContributors);
	unsigned char ContributorsCount() const;
	void RecieveTime(unsigned long timeStamp);
	unsigned long RecieveTime() const;
    static Packet* TmpGetPacketFromBuffer(char* buffer, int length);
    static int GetPacketsFromBuffer(char* buffer, int length, Packet** Packets);
protected:
	Packet(char *packet, int len, unsigned char nContributors);
	Packet(char *packet, int len);
	bool Padding() const;
	void Padding(bool);
	void ContributorsCount(unsigned char nContributors);
	void SetPacket(char* packet, int length);
	void DataLength(int dataLength);
	void IsData(bool isData);
	void IsCommand(bool isCommand);
	unsigned char *m_Packet;
	int m_BufLen;
	unsigned long m_RecieveTime;
	static int GetDataPacketHeaderSize();
	static int GetCommandPacketHeaderSize();
	static int GetPacketsFromBuffer(char* buffer, int length, Packet** Packets, int packetsCount);
};

class CommandPacket : public Packet
{
	// NOTE: don't use member variables here since a packet will be castet to a Data- or CommandPacket!
public:
	enum CommandType
	{
		Reserved = 0
		,RegisterUser
		,Keepalive
	};

	CommandType GetCommandType();
	void SetCommandType(CommandType type);
	void* GetCommandData(int* length);
private:
};

struct RTPHeader
{
	unsigned char Flags1;
	unsigned char Flags2;
	unsigned short SequenceNumber;
	unsigned long Timestamp;
	unsigned long SSRC;
};

#endif // !defined(Packet_h_INCLUDED)
