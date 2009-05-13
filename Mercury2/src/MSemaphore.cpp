#include <MSemaphore.h>

MSemaphore::MSemaphore()
	:m_counter(0)
{
}
#ifndef WIN32

unsigned long MSemaphore::Read()
{
	return __sync_or_and_fetch(&m_counter, 0);
}

unsigned long MSemaphore::ReadAndClear()
{
	return __sync_fetch_and_and(&m_counter, 0);
}

unsigned long MSemaphore::Decrement()
{
	return __sync_sub_and_fetch(&m_counter, 1);
}

unsigned long MSemaphore::Increment()
{
	return __sync_add_and_fetch(&m_counter, 1);
}

void MSemaphore::WaitAndSet(unsigned long value, unsigned long newVal)
{
	while ( !__sync_bool_compare_and_swap(&m_counter, value, newVal) );
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

unsigned long MSemaphore::Read()
{
	return OrAndFetch(&m_counter, 0);
//	return MyInterlockedOr(&m_counter, 0);
}

unsigned long MSemaphore::ReadAndClear()
{
	return MyInterlockedAnd(&m_counter, 0);
}

unsigned long MSemaphore::Decrement()
{
	return InterlockedDecrement(&m_counter);
}

unsigned long MSemaphore::Increment()
{
	return InterlockedIncrement(&m_counter);
}

void MSemaphore::WaitAndSet(unsigned long value, unsigned long newVal)
{
	InterlockedCompareExchange(&m_counter, newVal, value);
//	while ( !__sync_bool_compare_and_swap(&m_counter, value, newVal) );
}

#endif

MSemaphoreLock::MSemaphoreLock(MSemaphore* s)
	:m_s(s)
{
	m_s->WaitAndSet(0,1);
}

MSemaphoreLock::~MSemaphoreLock()
{
	m_s->WaitAndSet(1,0);
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
