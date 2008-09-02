#ifndef MSCOPEDARRAY_H
#define MSCOPEDARRAY_H

#include <MercuryThreads.h>

#define SAFE_DELETE_ARRAY( x ) { if (x) { delete[] x; } x = NULL; }

///A safe container for dynamically allocated memory
///Does not require a special base class
template<typename T>
class MScopedArray
{
	public:
		MScopedArray(T* ptr)
		:m_ptr(ptr), m_count(NULL)
		{
			m_criticalSection.Wait();
			IncrementReference();
			m_criticalSection.Open();
		}
	
		MScopedArray()
		: m_ptr(NULL), m_count(NULL)
		{
		}
		
		MScopedArray(const MScopedArray<T>& autoPtr)
		{
			m_criticalSection.Wait();
			m_ptr = autoPtr.m_ptr;
			m_count = autoPtr.m_count;
			IncrementReference();
			m_criticalSection.Open();
		}
		
		inline ~MScopedArray()
		{
			m_criticalSection.Wait();
			DecrementReference();
			m_criticalSection.Open();
		}
		
		inline unsigned int Count()
		{
			unsigned int count = 0;
			m_criticalSection.Wait();
			if( m_ptr && m_count ) count = *m_count;
			m_criticalSection.Open();
			return count;
		}
		
		void Clear()
		{
			m_criticalSection.Wait();
			DecrementReference();
			m_ptr = NULL;
			m_criticalSection.Open();
		}
/*		
		void Forget()
		{
			m_criticalSection.Wait();
			if( m_ptr )
				if (m_ptr->m_count)
					if ((m_ptr->m_count) > 0) { --m_ptr->m_count; }
			m_ptr = NULL;
			m_criticalSection.Open();
		}
*/		
		//Comparative
		inline bool operator<(const MScopedArray& rhs) const { return m_ptr < rhs.m_ptr; }
		inline bool operator>(const MScopedArray& rhs) const { return m_ptr > rhs.m_ptr; }
		inline bool operator<=(const MScopedArray& rhs) const { return m_ptr <= rhs.m_ptr; }
		inline bool operator>=(const MScopedArray& rhs) const { return m_ptr >= rhs.m_ptr; }
		inline bool operator==(const MScopedArray& rhs) const { return m_ptr == rhs.m_ptr; }
		inline bool operator!=(const MScopedArray& rhs) const { return m_ptr != rhs.m_ptr; }

		inline bool operator<(const T* rhs) const { return m_ptr < rhs; }
		inline bool operator>(const T* rhs) const { return m_ptr > rhs; }
		inline bool operator<=(const T* rhs) const { return m_ptr <= rhs; }
		inline bool operator>=(const T* rhs) const { return m_ptr >= rhs; }
		inline bool operator==(const T* rhs) const { return m_ptr == rhs; }
		inline bool operator!=(const T* rhs) const { return m_ptr != rhs; }
		
		//Assignment
		MScopedArray& operator=(const MScopedArray<T>& rhs)
		{
			m_criticalSection.Wait();
			DecrementReference();
			m_ptr = rhs.m_ptr;
			IncrementReference();
			m_criticalSection.Open();
			return *this;
		}

		//-> operator
		inline const T* operator->() const { return m_ptr; }
		inline T* operator->() { return m_ptr; }
		
		//dereference operator
		inline const T& operator->*(MScopedArray<T>& autoPtr) const { return *m_ptr; }
		inline T& operator->*(MScopedArray<T>& autoPtr) { return *m_ptr; }
		
		//automatic conversion to pointer
		inline operator T*() { return m_ptr; }
		inline operator const T*() const { return m_ptr; }

		inline bool IsValid() { return m_ptr != NULL; }  //true if valid
	private:
		void IncrementReference() //not thread safe, must guard
		{
			if ( m_count == NULL ) m_count = new unsigned long;
			if( m_ptr ) ++(*m_count);
		}
		
		void DecrementReference()  //not thread safe, must guard
		{
			if( m_ptr )
			{
//				if ( !m_ptr->m_count ) return;
				if (*m_count > 0) --(*m_count);
				if (*m_count == 0)
				{
					SAFE_DELETE_ARRAY(m_ptr);
					SAFE_DELETE(m_count);
				}
			}
		}
		
		T* m_ptr;
		unsigned long* m_count;
		static MercuryMutex m_criticalSection;
};

template<typename T>
MercuryMutex MScopedArray<T>::m_criticalSection = MercuryMutex();

#endif

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *   axlecrusher@gmail.com   *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
