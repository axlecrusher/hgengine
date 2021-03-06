#ifndef ALIGNEDBUFFER_H
#define ALIGNEDBUFFER_H

#include <MercuryUtil.h> 
#include <Mint.h>

template <typename T>
class AlignedBuffer
{
	public:
		AlignedBuffer()
			:m_length(0), m_data(NULL), m_mem(NULL)
		{};
		
		~AlignedBuffer()
		{
			Free();
		}
		
		void Allocate(unsigned long count, uint8_t alignment = 32)
		{
			if (m_mem!=0)
				free(m_mem);
			m_mem=0;

			void * m_memret;
			m_data = (T*)mmemalign(alignment, sizeof(T)*count, m_memret);
			m_mem = (T*)m_memret;
			m_length = count;
		}
		
		void Free()
		{
			if (m_mem!=0)
				free(m_mem);
			m_data = NULL;
			m_length = 0;
		}
		
		inline unsigned long Length() const { return m_length; }
		inline unsigned long LengthInBytes() const { return m_length*sizeof(T); }
		
		inline T* Buffer() { return m_data; }
		inline const T* Buffer() const { return m_data; }
		
		inline T& operator[](unsigned long x) { return m_data[x]; }
		inline const T& operator[](unsigned long x) const { return m_data[x]; }
		
	private:
		unsigned long m_length;
		T* m_data, *m_mem;
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
