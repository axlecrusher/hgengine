#ifndef MERCURYMEMORY_H
#define MERCURYMEMORY_H

#include <AlignedBuffer.h>
#include <list>
#include <MSemaphore.h>

///Memory holder for matrices
template <typename T>
class MercuryMemory
{
	/* Allocates float memory space as one contigous block
	to try to take advantage of data prefetching. Some matrix data should get a
	free ride into the CPU cache. */
	public:
		MercuryMemory(const uint32_t rows)
			:m_free(NULL),m_rows(rows)
		{
			MSemaphoreLock lock(&m_lock);
			AllocateMoreSpace(m_rows);
		}

		T* Allocate()
		{
			T* m = NULL;
			MemoryUnit* mu = NULL;
			{
				//keep really short lock
				MSemaphoreLock lock(&m_lock);
				if ( m_free == NULL ) AllocateMoreSpace(m_rows);
				mu = m_free;
				m_free=m_free->prev;
				if (m_free!=mu->prev) { char* a = NULL; *a=0; } //wtf happened here!?!?! FREE CHANGED DURING OUR LOCK
			}
			m = mu->mem;
			delete mu;
			if (m==NULL) { char* a = NULL; *a=0; } //no memory allocated??
			return m;
		}

		void Free(T* m)
		{
			MSemaphoreLock lock(&m_lock);
			for (MemoryUnit* mu=m_free;mu!=NULL;mu=mu->prev)
			{
				//probably could some some sorting here to get contigious free and used memory
				//if free memory is alwasy given out in order, then used memory will also be in order
				if (m==mu->mem) { char* a=NULL;*a=0;} //WTF DOUBLE FREE
			}
			m_free = new MemoryUnit(m_free,m);
		}

	private:
		struct MemoryUnit
		{
			MemoryUnit(MemoryUnit* p, T* m)
				:mem(m),prev(p)
			{
//				if (p!=NULL) p->next = this;
			}
			T* mem;
			MemoryUnit* prev;
		};

		void AllocateMoreSpace(const uint32_t rows)
		{
			AlignedBuffer<T>* d = new AlignedBuffer<T>();
			d->Allocate(rows,16);

			for (unsigned int i = 0; i < rows;i++)
				m_free = new MemoryUnit(m_free,(d->Buffer())+i);

			m_data.push_back(d);
		}

		std::list< AlignedBuffer<T>* > m_data;
		MemoryUnit* m_free;

		MSemaphore m_lock;
		unsigned long m_rows;
};

#endif

/****************************************************************************
 *   Copyright (C) 2010 by Joshua Allen                                     *
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
