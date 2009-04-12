#ifndef MERCURYLIST_H
#define MERCURYLIST_H

template<typename T>
class MercuryList
{
	public:
		class Entry
		{
			friend class MercuryList;
			public:
				Entry()
				:m_previous(NULL), m_next(NULL)
				{
				}
				
				Entry(T& data, Entry* previous = NULL, Entry* next = NULL)
				:m_previous(previous), m_next(next), m_data(data)
				{
				}
		
				Entry* Previous() const { return m_previous; }
				Entry* Next() const { return m_next; }
				T& Data() { return m_data; }
		
				//dereference operator
				const T& operator->*(Entry& p) const { return p.m_data; }
				T& operator->*(Entry& p) { return p.m_data; }
				
				Entry& operator=(const Entry& rhs)
				{
					this->m_previous = rhs.m_previous;
					this->m_next = rhs.m_next;
					this->m_data = rhs.m_data;
					return *this;
				}

				bool operator==(const Entry& rhs) const
				{
					return (rhs.m_previous == m_previous) && (rhs.m_next == m_next) && (rhs.m_data == m_data);
				}
				
				
				bool operator!=(const Entry& rhs) const
				{
					return !(rhs==*this);
				}
				
			private:
				Entry* m_previous;
				Entry* m_next;
				T m_data;
		};
		
		MercuryList()
			:m_head(NULL), m_tail(NULL), m_entries(NULL)
		{
		}
		
		void push_back(T& data)
		{
			Entry* e = new Entry(data, m_tail, NULL);
			
			if ( m_tail ) m_tail->m_next = e; //set previous's next
			m_tail = e;
			
			if ( !m_head ) m_head = e;
			if ( !m_entries ) m_entries = e;			
		}
		
		Entry* Head() const { return m_head; }
		Entry* Tail() const { return m_tail; }

		void clear()
		{
			if (!m_entries) return;
			for (Entry* i = m_tail; i != NULL; i = i->Previous() )
			{
				delete i;
				i = NULL;
			}
			m_entries = m_head = m_tail = NULL;
		}

		bool isEmpty() const
		{
			return !(m_head || m_tail);
		}
		
		void erase(Entry* n)
		{
			if (m_head == n) m_head = n->Next();
			if (m_tail == n) m_tail = n->Previous();
			if (m_entries == n) m_entries = n->Next();
			if (n->Previous()) n->Previous()->m_next = n->Next();
			if (n->Next()) n->Next()->m_previous = n->Previous();
			delete n;
		}
		
	private:
		Entry* m_head;
		Entry* m_tail; //always last entry
		Entry* m_entries; //just a single entry
};

#endif

/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen                                     *
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
