/*
 * Copyright (c) 2011-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCInvitationPayloadParser.h>
#include <Swiften/Parser/Tree/NullParserElement.h>

namespace Swift {

void MUCInvitationPayloadParser::handleTree(ParserElement::ref root) {
	MUCInvitationPayload::ref invite = getPayloadInternal();
	invite->setIsContinuation(root->getAttributes().getBoolAttribute("continue", false));
	invite->setJID(JID(root->getAttributes().getAttribute("jid")));
	invite->setPassword(root->getAttributes().getAttribute("password"));
	invite->setReason(root->getAttributes().getAttribute("reason"));
	invite->setThread(root->getAttributes().getAttribute("thread"));
	ParserElement::ref impromptuNode = root->getChild("impromptu", "http://swift.im/impromptu");
	invite->setIsImpromptu(!boost::dynamic_pointer_cast<NullParserElement>(impromptuNode));
}

}
