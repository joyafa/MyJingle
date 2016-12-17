// TestMsg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "talk/base/thread.h"
using namespace cricket;

class HelpData : public MessageData
{
public:
	std::string info_;
};

class Police : public MessageHandler
{
public:
	enum {
		MSG_HELP,
	};

	void Help(const std::string& info) {
		HelpData* data = new HelpData;
		data->info_ = info;
		Thread::Current()->Post(this, MSG_HELP, data);
	}

	virtual void OnMessage(Message* msg) {
		switch (msg->message_id)
		{
		case MSG_HELP:
			HelpData* data = (HelpData*)msg->pdata;
			std::cout << "MSG_HELP : " << data->info_ << std::endl;
			break;
		}
	}
};

int main(int argc, char** argv)
{
	Police p;
	p.Help("Please help me!");
	Thread::Current()->Loop();
	return 0;
}
