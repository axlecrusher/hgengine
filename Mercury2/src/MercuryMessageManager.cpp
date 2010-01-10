#include <MercuryMessageManager.h>
#include <MercuryCTA.h>

MercuryCTA HolderAllocator( sizeof(MessageHolder), 8 );

/* MessageManager needs to be thread safe. Semaphores are used as locks (spin locks) to avoid OS rescheduling.
Locks need to be held for as little time as possible. Scoping is used to put locking classes on the stack
so that are ALWAYS released when out of scope.*/

MessageHolder::MessageHolder()
	:data(NULL),when(0)
{
}

bool MessageHolder::Compare( void * left, void * right )
{
	return (((MessageHolder*)left)->when) < (((MessageHolder*)right)->when);
}

void MercuryMessageManager::PostMessage(const MString& message, MessageData* data, float delay)
{
	MessageHolder * m = new(HolderAllocator.Malloc()) MessageHolder();
	m->message = message;
	m->data = data;
	
	{
		//scope the lock to a very small portion of code
		MSemaphoreLock lock(&m_queueLock);
		m->when = m_currTime + uint64_t(delay*1000000);
		m_messageQueue.Push( m );
	}
}

void MercuryMessageManager::PumpMessages(const uint64_t& currTime)
{
	{
		MSemaphoreLock lock(&m_queueLock);
		m_currTime = currTime;
	}
	
	for (MessageHolder* mh = GetNextMessageFromQueue(); mh; mh = GetNextMessageFromQueue())
	{
		FireOffMessage( *mh );
		SAFE_DELETE( mh->data );
		HolderAllocator.Free(mh);
	}
}

void MercuryMessageManager::BroadcastMessage( const MString & message, const MessageData * data )
{
	std::list< MessagePair > recipients;
	{
		//copy list first (quick lock)
		MSemaphoreLock lock(&m_recipientLock);
		std::list< MessagePair > * r = m_messageRecipients.get( message );
		if ( r ) recipients = *r;
	}

	if ( !recipients.empty() )
	{
		std::list< MessagePair >::iterator recipient = recipients.begin();
		for (; recipient != recipients.end(); ++recipient)
		{
			MessagePair & mp = *recipient;
			//Okay, the following lines look horrible.  Reason is we're using
			//a horrible horrible c++ construct from the anals of pointerdom.
			//The idea is we're using a delegate.  If we have a delegate, use it.
			//If you are receiving a delegate, you do not need the message name.
			//Otherwise, send a standard message through the old interface.
			if( mp.d )
				(mp.h->*(mp.d))( *(data) );
			else
				mp.h->HandleMessage(message, *(data) );
		}
	}
	
}

void MercuryMessageManager::UnRegisterForMessage(const MString& message, MessageHandler* ptr)
{
	MSemaphoreLock lock(&m_recipientLock);

	std::list< MessagePair >& subscriptions = m_messageRecipients[message];
	std::list< MessagePair >::iterator i = subscriptions.begin();
	
	for (;i != subscriptions.end(); ++i)
	{
		if ((*i).h == ptr)
		{
			subscriptions.erase( i );
			return;
		}
	}
	
}

void MercuryMessageManager::RegisterForMessage(const MString& message, MessageHandler* ptr,  MessageDelegate d )
{
	MSemaphoreLock lock(&m_recipientLock);
	m_messageRecipients[message].push_back(MessagePair(ptr, d));
	
}


void MercuryMessageManager::FireOffMessage( const MessageHolder & message )
{
	std::list< MessagePair > recipients;
	{
		//copy list first (quick lock)
		MSemaphoreLock lock(&m_recipientLock);
		std::list< MessagePair > * r = m_messageRecipients.get( message.message );
		if ( r ) recipients = *r;
	}

	if ( !recipients.empty() )
	{
		std::list< MessagePair >::iterator recipient = recipients.begin();
		for (; recipient != recipients.end(); ++recipient)
		{
			MessagePair & mp = *recipient;
			//Okay, the following lines look horrible.  Reason is we're using
			//a horrible horrible c++ construct from the anals of pointerdom.
			//The idea is we're using a delegate.  If we have a delegate, use it.
			//If you are receiving a delegate, you do not need the message name.
			//Otherwise, send a standard message through the old interface.
			if( mp.d )
				(mp.h->*(mp.d))( *(message.data) );
			else
				mp.h->HandleMessage(message.message, *(message.data) );
		}
	}
}

MessageHolder* MercuryMessageManager::GetNextMessageFromQueue()
{
	/* We need to ensure that viewing the queue and retrieving the message
	happens without the queue changing. */
	MSemaphoreLock lock(&m_queueLock);
	
	MessageHolder* mh = NULL;
	if ( !m_messageQueue.Empty() )
	{
		if ( ((MessageHolder *)m_messageQueue.Peek())->when > m_currTime ) return NULL;
		mh = (MessageHolder *)m_messageQueue.Pop();
	}

	return mh;
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
