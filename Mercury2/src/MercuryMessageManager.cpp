#include <MercuryMessageManager.h>

MessageHolder::MessageHolder()
	:data(NULL)
{
}

void MercuryMessageManager::PostMessage(const MString& message, MessageData* data, float delay)
{
	MessageHolder m;
	m.message = message;
	m.data = data;
	uint64_t fireTime = m_currTime + uint64_t(delay*1000000);
	m_messageQueue.Insert(fireTime, m);
}

void MercuryMessageManager::PumpMessages(const uint64_t& currTime)
{
	m_currTime = currTime;
	while ( !m_messageQueue.empty() )
	{
		if ( m_messageQueue.PeekNextPriority() > m_currTime ) return;
		
		MessageHolder& message = m_messageQueue.GetNext();
		FireOffMessage( message );
		SAFE_DELETE( message.data );
		m_messageQueue.PopNext();
	}
}

void MercuryMessageManager::RegisterForMessage(const MString& message, MessageHandler* ptr)
{
	m_messageRecipients[message].push_back(ptr);
}

void MercuryMessageManager::FireOffMessage(const MessageHolder& message)
{
	std::map< MString, std::list< MessageHandler* > >::iterator i = m_messageRecipients.find(message.message);
	
	if ( i != m_messageRecipients.end() )
	{
		std::list< MessageHandler* >::iterator recipients = i->second.begin();
	
		for (; recipients != i->second.end(); ++recipients)
			(*recipients)->HandleMessage(message.message, message.data);
	}
}

MercuryMessageManager& MercuryMessageManager::GetInstance()
{
	static MercuryMessageManager *instance = NULL;
	if (!instance)
		instance = new MercuryMessageManager();
	return *instance;
}

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
 *                                                                          *
 *                                                                          *
 *   All rights reserved.                                                   *
 *                                                                          *
 *   Redistribution and use in source and binary forms, with or without     *
 *   modification, are permitted provided that the following conditions     *
 *   are met:                                                               *
 *     * Redistributions of source code must retain the above copyright     *
 *      notice, this list of conditions and the following disclaimer.       *
 *     * Redistributions in binary form must reproduce the above            *
 *      copyright notice, this list of conditions and the following         *
 *      disclaimer in the documentation and/or other materials provided     *
 *      with the distribution.                                              *
 *     * Neither the name of the Mercury Engine nor the names of its        *
 *      contributors may be used to endorse or promote products derived     *
 *      from this software without specific prior written permission.       *
 *                                                                          *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 ***************************************************************************/
