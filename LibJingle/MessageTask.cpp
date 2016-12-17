#include "MessageTask.h"
#include "talk/xmpp/constants.h"

namespace buzz {

bool
MessageTask::HandleStanza(const XmlElement * stanza) 
{
  if (stanza->Name() != QN_MESSAGE)
    return false;
  QueueStanza(stanza);
  return true;
}

int
MessageTask::ProcessStart() 
{
    const XmlElement * stanza = NextStanza();
    if (stanza == NULL)
        return STATE_BLOCKED;

    TextMessage msg; 
    msg.from_jid = stanza->Attr(QN_FROM);
    const XmlElement * body_element = stanza->FirstNamed(QN_BODY);
    if( body_element == NULL)
        return STATE_BLOCKED;

    msg.body = body_element->BodyText();
    SignalChatMessage(msg);

    return STATE_START;
}

}


