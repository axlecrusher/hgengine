#ifndef MSTACK_H
#define MSTACK_H

#include <MSemaphore.h>

template <typename T>
class MStack
{
	private:
		struct Container
		{
				Container()
					:m_next(NULL)
				{ }
				T m_data;
				volatile Container* m_next;
		};

		volatile Container* m_head;
		volatile Container* m_freeContainers;

		// use instead of calling new Container()
		Container* GetContainer()
		{
			Container* a = (Container*)m_freeContainers;
			if (a==NULL) return new Container(); //make new container, don't care if a has changed since read
			if ( CAS_PTR((volatile void**)&m_freeContainers,(void*)a->m_next,a)==a ) //true if swap made
				return a;
			return new Container();
		}

		//use instead of calling delete Container
		void SaveContainerForFuture(Container* c)
		{
			do
			{
				c->m_next = m_freeContainers;
			}
			while( CAS_PTR((volatile void**)&m_freeContainers,c,(void*)c->m_next)!=c->m_next );
		}

	public:
		MStack()
			:m_head(NULL), m_freeContainers(NULL)
		{
		}

		void push(const T& data)
		{
			Container* a = GetContainer();
			a->m_data = data;
			do
			{
				a->m_next = m_head;
			} while ( CAS_PTR((volatile void**)&m_head,a,(void*)a->m_next)!=a->m_next );
		}

		bool pop()
		{
			if (m_head==NULL) return false;
			Container* a = NULL;
			do
			{
				a = m_head;
			} while ( CAS_PTR((volatile void**)&m_head,(void*)a->m_next,a)!=a );
			SaveContainerForFuture(a);
			return true;
		}

		bool pop_get(T& d)
		{
			if (m_head==NULL) return false;
			Container* a = NULL;
			do
			{
				a = (Container*)m_head;
			} while ( CAS_PTR((volatile void**)&m_head,(void*)a->m_next,a)!=a );
			d = a->m_data;
			SaveContainerForFuture(a);
			return true;
		}

		inline bool empty() const { return m_head==NULL; }
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
