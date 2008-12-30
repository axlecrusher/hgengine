#include <HGMDLMesh.h>

void HGMDLMesh::LoadFromFile(FILE* hgmdl)
{
	uint32_t nameLength;
	fread(&nameLength, sizeof(uint32_t), 1, hgmdl);
	
	if (nameLength > 0)
	{
		char* name = new char[nameLength+1];
		name[nameLength] = 0;
		fread(name, nameLength, 1, hgmdl);
		m_name = name;
	}
	
	fread(&m_cachable, sizeof(bool), 1, hgmdl);
	
	uint32_t dataLength;
	fread(&dataLength, sizeof(uint32_t), 1, hgmdl);
	if (dataLength > 0)
	{
		m_vertexData.Allocate( dataLength/sizeof(float) ); //they are all floats
		fread(m_vertexData.Buffer(), dataLength, 1, hgmdl);
	}
	
	uint16_t numIndices;
	fread(&numIndices, sizeof(uint16_t), 1, hgmdl);
	if (numIndices > 0)
	{
		m_indexData.Allocate( numIndices );
		fread(m_indexData.Buffer(), numIndices*sizeof(uint16_t), 1, hgmdl);
	}
}

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
