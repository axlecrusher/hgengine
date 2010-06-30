#ifndef MERCURYSTRINGUTIL_H
#define MERCURYSTRINGUTIL_H

#include <MercuryString.h>
#include <Mint.h>
#include <MercuryVector.h>

///Make a string all upper case
MString ToUpper(const MString & s);

///Make a string all lower case
MString ToLower(const MString & s);

///Make a string proper case (HELLO -> Hello, hello -> Hello)
MString ToProper(const MString & s);

float StrToFloat(const MString & s, float d = 0);
int32_t StrToInt(const MString & s, int32_t d = 0);
bool StrToBool( const MString & s, bool d = false );

///Split given string into other strings using delimiters from termin
void	SplitStrings( const MString & in, MVector < MString > & out, const char * termin, const char * whitespace, long termlen, long wslen );

///Convert string containing binary characters to C-style formatted string. 
MString	ConvertToCFormat( const MString & ncf );

///Convert a C-style formatted string into it's binary string equivalent.
MString ConvertToUnformatted( const MString & cf );

#endif


/* Copyright (c) 2010, Joshua Allen and Charles Lohr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *	-	Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the following disclaimer.
 *	-	Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the distribution.
 *	-	Neither the name of the Mercury Engine nor the names of its
 *		contributors may be used to endorse or promote products derived from
 *		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
