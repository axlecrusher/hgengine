#ifndef MERCURYMEMORY_H
#define MERCURYMEMORY_H

#include <AlignedBuffer.h>
#include <list>
#include <MSemaphore.h>

#include <MStack.h>

///Memory holder for matrices
template <typename T>
class MercuryMemory
{
	/* Allocates float memory space as one contigous block
	to try to take advantage of data prefetching. Some matrix data should get a
	free ride into the CPU cache. */
	public:
		MercuryMemory(const uint32_t rows)
			:m_rows(rows)
		{
			AllocateMoreSpace(m_rows);
		}

		T* Allocate()
		{
			T* m = NULL;
			while ( !m_freeData.pop_get(m) ) AllocateMoreSpace(m_rows);
			return m;
		}

		inline void Free(T* m) { m_freeData.push(m); }

	private:

		void AllocateMoreSpace(const uint32_t rows)
		{
			AlignedBuffer<T>* d = new AlignedBuffer<T>();
			d->Allocate(rows,16);

			m_lock.Wait(); //required
			m_data.push_back(d);
			m_lock.UnLock(); //required

			for (unsigned int i = 0; i < rows;i++)
				m_freeData.push(d->Buffer()+i);
		}

		std::list< AlignedBuffer<T>* > m_data;
		MStack< T* > m_freeData;

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
