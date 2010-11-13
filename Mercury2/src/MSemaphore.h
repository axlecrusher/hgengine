#ifndef MSEMAPHORE_H
#define MSEMAPHORE_H

#include <global.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32

#define SYNC_OR_AND_FETCH(d,v) __sync_or_and_fetch(d,v)
#define COMPARE_AND_SWAP(d,o,n) __sync_val_compare_and_swap(d,o,n)
#define SYNC_AND_AND_FETCH(d,v) __sync_and_and_fetch(d,v)

#else

#define SYNC_OR_AND_FETCH(d,v) ((uint32_t)OrAndFetch(d,v))
#define COMPARE_AND_SWAP(d,o,n) ((uint32_t)InterlockedCompareExchange(d, n, o))
#define SYNC_AND_AND_FETCH(d,v) ((uint32_t)__sync_and_and_fetch(d,v))

#endif

class MSemaphore
{
	public:
		MSemaphore();
		
		unsigned long Read();
		unsigned long ReadAndClear();
		unsigned long Decrement();
		unsigned long Increment();
		void WaitAndSet(uint32_t value, uint32_t newVal);
		
		void Wait();
		void UnLock();
	private:

		//what exactly needs to be volatile?
		uint32_t m_lockCount;
#ifndef WIN32
		uint32_t m_semaphore;
#else
		volatile LONG m_semaphore; //align to 32bit boundary
#endif
};

class MSemaphoreLock
{
	public:
		MSemaphoreLock(MSemaphore* s);
		~MSemaphoreLock();
	private:
		MSemaphore* m_s;
};

class MSemaphoreIncOnDestroy
{
	public:
		MSemaphoreIncOnDestroy(MSemaphore* s);
		~MSemaphoreIncOnDestroy();
	private:
		MSemaphore* m_s;
};

class MSemaphoreDecOnDestroy
{
	public:
		MSemaphoreDecOnDestroy(MSemaphore* s);
		~MSemaphoreDecOnDestroy();
	private:
		MSemaphore* m_s;
};

#endif
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
