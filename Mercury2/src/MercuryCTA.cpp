#include "MercuryCTA.h"
#include <stdlib.h>
#include <stdio.h>

MercuryCTA::MercuryCTA( int inElementSize, int iDefaultSize )
{
	iElementSize = inElementSize;
	iCurSize = iDefaultSize;

	ibmFree = (void**)::malloc( iCurSize * sizeof( void * ) );
	ibmHead = &ibmFree[iCurSize-1];

	void * vData = ::malloc( iDefaultSize * iElementSize );

	pHeadToFree = new PtrPair( vData, 0 );

	//TODO: Test performance of reverse numbering, so the first blocks are allocated before the later ones.
	for( void ** i = ibmFree; i != (ibmHead + 1); ++i, vData = ((char*)vData) + iElementSize )
		*i = vData;
}

MercuryCTA::~MercuryCTA()
{
	for( ; pHeadToFree != 0; )
	{
		::free( pHeadToFree->data );
		PtrPair * next = pHeadToFree->next;
		delete pHeadToFree;
		pHeadToFree = next;
	}
	::free( ibmFree );
}

void * MercuryCTA::Malloc()
{
	//Out of space
	if( ibmHead < ibmFree )
	{
		int iNumOfAddedElements = iCurSize; //iCurSize * 2 - iCurSize
		iCurSize *= 2;

		::free( ibmFree );
		ibmFree = (void**)::malloc( iCurSize * sizeof( void * ) );

		ibmHead = &ibmFree[iNumOfAddedElements-1];

		void * vData = ::malloc( iNumOfAddedElements * iElementSize );

		pHeadToFree = new PtrPair( vData, pHeadToFree );

		//TODO: Test performance of reverse numbering, so the first blocks are allocated before the later ones.
		for( void** i = ibmFree; i != (ibmHead + 1); i++, vData = ((char*)vData) + iElementSize )
			*i = vData;
	}
	return *(ibmHead--);
}

void MercuryCTA::Free( void * data )
{
	ibmHead++;
	*ibmHead = data;
}


/****************************************************************************
 *   Copyright (C) 2009 by Charles Lohr                                     *
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
