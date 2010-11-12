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
		{
			MSemaphoreLock lock(&m_lock);
			m_data.Allocate(rows,16);

			for (unsigned int i = 0; i < rows;i++)
				m_free.push_back( m_data.Buffer()+i );
		}

//		void Init();
//		static MercuryMatrixMemory& GetInstance();
		T* Allocate()
		{
			T* m = (T*)0x0;
			MSemaphoreLock lock(&m_lock);
			if ( m_free.begin() != m_free.end() )
			{
				m = m_free.front();
				m_free.pop_front();
			}
//			if (m==0x0) *m=*m; //crash if could not allocate
			return m;
		}

		void Free(T* m)
		{
			MSemaphoreLock lock(&m_lock);
			m_free.push_back(m);
		}

	private:
//		static const unsigned int rows = 1024; //1024 matrices * 64bytes each = 64kb
		AlignedBuffer<T> m_data;
		std::list< T* > m_free;
		MSemaphore m_lock;
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
