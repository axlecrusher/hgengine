#include <MSemaphore.h>
#include <MercuryThreads.h>

MSemaphore::MSemaphore()
	:m_lockCount(0), m_semaphore(0)
{
}
#ifndef WIN32

#define SYNC_OR_AND_FETCH(d,v) __sync_or_and_fetch(d,v)
#define COMPARE_AND_SWAP(d,o,n) __sync_val_compare_and_swap(d,o,n)
#define SYNC_AND_AND_FETCH(d,v) __sync_and_and_fetch(d,v)

unsigned long MSemaphore::ReadAndClear()
{
	return __sync_fetch_and_and(&m_semaphore, 0);
}

unsigned long MSemaphore::Decrement()
{
	return __sync_sub_and_fetch(&m_semaphore, 1);
}

unsigned long MSemaphore::Increment()
{
	return __sync_add_and_fetch(&m_semaphore, 1);
}

#else

FORCEINLINE
LONG
OrAndFetch (
    __inout LONG volatile *Destination,
    __in    LONG Value
    )
{
    LONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange(Destination,
                                          Old | Value,
                                          Old) != Old);

    return Old | Value;
}

//Function seem to be missing from x86 WinBase
FORCEINLINE
LONG
MyInterlockedAnd (
    __inout LONG volatile *Destination,
    __in    LONG Value
    )
{
    LONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange(Destination,
                                          Old & Value,
                                          Old) != Old);

    return Old;
}

FORCEINLINE
LONG
__sync_and_and_fetch (
    __inout LONG volatile *Destination,
    __in    LONG Value
    )
{
    LONG Old;

    do {
        Old = *Destination;
    } while (InterlockedCompareExchange(Destination,
                                          Old & Value,
                                          Old) != Old);

    return Old & Value;
}

#define SYNC_OR_AND_FETCH(d,v) ((uint32_t)OrAndFetch(d,v))
#define COMPARE_AND_SWAP(d,o,n) ((uint32_t)InterlockedCompareExchange(d, n, o))
#define SYNC_AND_AND_FETCH(d,v) ((uint32_t)__sync_and_and_fetch(d,v))

unsigned long MSemaphore::ReadAndClear()
{
	return MyInterlockedAnd(&m_semaphore, 0);
}

unsigned long MSemaphore::Decrement()
{
	return InterlockedDecrement(&m_semaphore);
}

unsigned long MSemaphore::Increment()
{
	return InterlockedIncrement(&m_semaphore);
}

#endif

unsigned long MSemaphore::Read()
{
	return SYNC_OR_AND_FETCH(&m_semaphore, 0);
}

void MSemaphore::WaitAndSet(uint32_t value, uint32_t newVal)
{
	while( COMPARE_AND_SWAP(&m_semaphore, value, newVal) != value );
}

void MSemaphore::Wait()
{
	uint32_t thread = MercuryThread::Current();
	if ( SYNC_OR_AND_FETCH(&m_semaphore, 0) == thread ) //recursive lock
	{
		++m_lockCount;
		return;
	}
	
	WaitAndSet(0,thread); //new lock
	++m_lockCount;
}

void MSemaphore::UnLock()
{
	uint32_t thread = MercuryThread::Current();
	if ( SYNC_OR_AND_FETCH(&m_semaphore, 0) == thread ) //unlock given from correct thread
	{
		--m_lockCount;
		if (m_lockCount == 0) SYNC_AND_AND_FETCH(&m_semaphore, 0 );
	}
}

MSemaphoreLock::MSemaphoreLock(MSemaphore* s)
	:m_s(s)
{
	m_s->Wait();
}

MSemaphoreLock::~MSemaphoreLock()
{
	m_s->UnLock();
}

MSemaphoreIncOnDestroy::MSemaphoreIncOnDestroy(MSemaphore* s)
	:m_s(s)
{
}

MSemaphoreIncOnDestroy::~MSemaphoreIncOnDestroy()
{
	m_s->Increment();
}

MSemaphoreDecOnDestroy::MSemaphoreDecOnDestroy(MSemaphore* s)
	:m_s(s)
{
}

MSemaphoreDecOnDestroy::~MSemaphoreDecOnDestroy()
{
	m_s->Decrement();
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
