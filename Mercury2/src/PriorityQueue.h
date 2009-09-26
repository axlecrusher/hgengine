#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <stdlib.h>

#define DEFAULTHEAPSIZE 8

class PriorityQueue
{
public:
	PriorityQueue( bool (*LessThanFn)( void*, void* ) )
	{
		Size = 0;
		Comp = LessThanFn;
		Reserved = DEFAULTHEAPSIZE;
		Heap = (void**)malloc( sizeof( void* ) * Reserved );
	}

	~PriorityQueue()
	{
		free( Heap );
	}

	void PushRaw( void * ptr )
	{
		if( Size >= Reserved )
		{
			Reserved *= 2;
			Heap = (void**)realloc( Heap, sizeof( void* ) * Reserved );
		}
		Heap[Size] = ptr;
		Size++;
	}

	void Push( void * ptr )
	{
		unsigned i;
		PushRaw( ptr );

		//Percolate up
		i = Size - 1;
		while( i )
		{
			int parent;
			void * tmp;
			parent = (i - 1)/2;

			if( Comp( Heap[parent], Heap[i] ) )
				break;

			tmp = Heap[parent];
			Heap[parent] = Heap[i];
			Heap[i] = tmp;

			i = parent;
		}
	}

	void Fixup()
	{
		int comps = 0;
		int swaps = 0;
		if( Size < 2 )
			return;

		unsigned i;

		//As strange as it is, doing it in this order is linear time
		//I don't fully understand the math behind it, but
		//doing it in reverse order results in more swaps and comps.
		//Comps should be close to but less than Size*2
		//Swaps should be close to but less than Size
		for( i = Size-1; i > 0; i-- )
		{
			void * swap;
			int ths = i;
			unsigned parent = (ths-1)/2;


			comps++;
			while( ths && Comp( Heap[ths], Heap[parent] ) )
			{
				swaps++; comps++;
				swap = Heap[parent];
				Heap[parent] = Heap[ths];
				Heap[ths] = swap;

				ths = parent;
				parent = (ths-1)/2;
			}
		}
	}

	void * Front( )
	{
		if( Size )
			return Heap[0];
		else
			return 0;
	}

	void * Peek( )
	{
		return Size?Heap[0]:0;
	}

	void * Pop( )
	{
		void * ret;
		int i;
		if( !Size )
			return 0;
		Size--;
		if( !Size )
			return Heap[0];

		ret = Heap[0];

		Heap[0] = Heap[Size];

		i = 0;
		//Now, heapify-down
		do
		{
			int smaller;
			int childL, childR;
			void * swap;

			childL = (i*2)+1;
			childR = (i*2)+2;

			//No Children?  Done!
			if( childL > (int)Size && childR > (int)Size )
				return ret;

			if( childR > (int)Size )
			{
				if( Comp( Heap[childL], Heap[i] ) )
				{
					swap = Heap[childL];
					Heap[childL] = Heap[i];
					Heap[i] = swap;
				}

				return ret;
			}

			smaller = Comp( Heap[childL], Heap[childR] )?childL:childR;

			//We're already smaller than the smallest child.
			if( Comp( Heap[i], Heap[smaller] ) )
				return ret;

			swap = Heap[smaller];
			Heap[smaller] = Heap[i];
			Heap[i] = swap;
			i = smaller;
		}
		while(true);
	}

	inline bool Empty() { return !Size; }
private:
	void ** Heap;
	unsigned int Reserved;
	unsigned int Size;
	bool (*Comp)( void*, void*);
};

#endif

/****************************************************************************
 *   Copyright (C) 2008-2009 by Joshua Allen                                *
 *                              Charles Lohr                                *
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
