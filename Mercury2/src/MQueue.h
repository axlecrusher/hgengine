#ifndef MQUEUE_H
#define MQUEUE_H

#include <MercuryUtil.h>

template<typename T>
class MQueue
{
	public:
		MQueue()
			:m_head(0), m_tail(0), m_size(0), m_used(0)
		{}
		
		~MQueue()
		{
			SAFE_DELETE_ARRAY(m_elements);
		}
		
		bool empty() const { return (m_head==m_tail)&&(m_used==0); }
		
		T pop()
		{
			T t( m_elements[m_head] );
			m_head = (m_head+1)%m_size;
			--m_used;
			return t;
		}
		
		void push(T t)
		{
			AllocateSpace();
			m_elements[m_tail] = t;
			m_tail = (m_tail+1)%m_size;
			++m_used;
		}
	private:
		void AllocateSpace()
		{
			if (m_size == 0)
			{
				m_size = 2048;
				m_elements = new T[2048];
			}
			/*
			else
			{
				if (((m_tail)%m_size) == m_head)
				{
					T* tmp = m_elements;
					SAFE_DELETE_ARRAY(m_elements);
					m_elements = new T[m_size*2];
					memcpy(m_elements, tmp, sizeof(T)*m_size);
					m_size*=2;
				}
			}
			*/
		}
		
		T* m_elements;
		unsigned long m_head;//the first used spot
		unsigned long m_tail;//the first empty spot
		unsigned long m_size;
		unsigned long m_used;
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
