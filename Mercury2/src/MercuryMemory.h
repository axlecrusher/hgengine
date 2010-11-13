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
			:m_freeMem(NULL),m_freeUnit(NULL),m_rows(rows)
		{
//			m_lock.Wait();
			AllocateMoreSpace(m_rows);
//			m_lock.UnLock();
		}

		T* Allocate()
		{
			T* m = NULL;
			MemoryUnit* mu = NULL;
			//keep really short lock
			m_lock.Wait();
			if ( m_freeMem == NULL ) m_freeMem = AllocateMoreSpace(m_rows);

			//get record of free memory spot
			mu = m_freeMem;
			m_freeMem=m_freeMem->prev;
			m = (T*)mu->mem;

			SaveFreeUnit(mu);
			m_lock.UnLock();
//			delete mu;

//			if (m==NULL) { char* a = NULL; *a=0; } //no memory allocated??
			return m;
		}

		void Free(T* m)
		{
			m_lock.Wait();
//			for (MemoryUnit* mu=m_freeMem;mu!=NULL;mu=mu->prev)
//			{
				//probably could some some sorting here to get contigious free and used memory
				//if free memory is alwasy given out in order, then used memory will also be in order
//				if (m==mu->mem) { char* a=NULL;*a=0;} //WTF DOUBLE FREE
//			}
//			m_freeMem = new MemoryUnit(m_freeMem,m);

/*
			MemoryUnit* mu = m_freeUnit;
			m_freeUnit = m_freeUnit->prev; //set top free unit to next free unit
			//setup unit and add it to the top of the free memory
			mu->prev = m_freeMem;
			mu->mem = m;
*/			TakeFreeUnit(m);

			m_lock.UnLock();
		}

	private:
		struct MemoryUnit
		{
			MemoryUnit(MemoryUnit* p, void* m)
				:mem(m),prev(p)
			{
//				if (p!=NULL) p->next = this;
			}
//			T* mem;
			void* mem;
			MemoryUnit* prev;
		};

		MemoryUnit* AllocateMoreSpace(const uint32_t rows)
		{
			MemoryUnit* mu = NULL;

			AlignedBuffer<T>* d = new AlignedBuffer<T>();
			d->Allocate(rows,16);

			m_lock.Wait();
			m_data.push_back(d);
			m_lock.UnLock();

			for (unsigned int i = 0; i < rows;i++)
				mu = new MemoryUnit(mu,(d->Buffer())+i);
			return mu;
		}

		inline void TakeFreeUnit(T* m)
		{
			MemoryUnit* mu = m_freeUnit;
			m_freeUnit = m_freeUnit->prev; //set top free unit to next free unit
			//setup unit and add it to the top of the free memory
			mu->prev = m_freeMem;
			mu->mem = m;
			m_freeMem = mu;
		}

		void SaveFreeUnit(MemoryUnit* mu)
		{
			mu->mem = NULL;
			mu->prev = m_freeUnit;
			m_freeUnit = mu;
		}

		std::list< AlignedBuffer<T>* > m_data;
		MemoryUnit* m_freeMem; //Free template memory
		MemoryUnit* m_freeUnit; //Free MemoryUnits, for when 

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
