#ifndef _MESSAGETASK_H_
#define _MESSAGETASK_H_

#include "talk/xmpp/xmppengine.h"
#include "talk/xmpp/xmpptask.h"
#include "talk/base/sigslot.h"

#include <string>

namespace buzz {

class TextMessage
{
public:
    std::string body;
    std::string from_jid;
};

class MessageTask : public XmppTask {

public:
  MessageTask(Task * parent) : XmppTask(parent, XmppEngine::HL_TYPE) {}
  virtual int ProcessStart();
  sigslot::signal1<const buzz::TextMessage &>SignalChatMessage;

protected:
  virtual bool HandleStanza(const XmlElement * stanza);
};

  
}

#endif
