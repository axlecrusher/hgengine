#include <RawImageData.h>
#include <MercuryUtil.h>
#include <ImageLoader.h>

using namespace std;

RawImageData* LoadBMP( FILE* file )
{
	int offset;
	char* tmp = new char[sizeof(int)];
	int compression = 0;
	int length;
	int rawlength = 0;
	int bitsapix = 0;
	unsigned char b[3];
	unsigned int res_x, res_y;
	
//	FILE* file = fopen(filename.c_str(), "rb");
	printf( "BMP Load Start\n" );
	if (file==NULL)
	{
		printf("Could not open BMP (null file pointer)\n");
		SAFE_DELETE_ARRAY(tmp);
		return NULL;
	}

	//Get the type of file and test
	memset(tmp, 0, 4);
//	file->Read(tmp, sizeof(char) * 2);
	fread(tmp, sizeof(char) * 2, 1, file);
	MString type(tmp);

	if (type != "BM")
	{
		printf("not a valid BMP\n");
		SAFE_DELETE_ARRAY(tmp);
		return NULL;
	}
	//Offset of bitmap data.
	fseek(file, 10, SEEK_SET);
//	file->Seek(10);
	fread(tmp, 4, 1, file);
//	file->Read(tmp, 4);
	memcpy(&offset, tmp, 4);
	TO_ENDIAN( offset );

	RawImageData* image = new RawImageData;

	//width & width
	fseek(file, 18, SEEK_SET);
//	file->Seek(18);
	fread(tmp, sizeof(int), 1, file);
//	file->Read(tmp, sizeof(int));
	memcpy(&image->m_width, tmp, sizeof(int));
	TO_ENDIAN( image->m_width );
	fread(tmp, sizeof(int), 1, file);
//	file->Read(tmp, sizeof(int));
	memcpy(&image->m_height, tmp, sizeof(int));
	TO_ENDIAN( image->m_height );

	//bits per pixel
	memset(tmp, 0, sizeof(int));
	fseek(file, 28, SEEK_SET);
//	file->Seek(28);
//	file->Read(tmp, sizeof(int));
	fread(tmp, sizeof(int), 1, file);
	memcpy(&bitsapix, tmp, sizeof(int));
	TO_ENDIAN( bitsapix );

	if (bitsapix != 24)
	{
		printf("is not 24b/pix\n");
		SAFE_DELETE_ARRAY(tmp);
		SAFE_DELETE_ARRAY(image);
		return NULL;
	}

	//compression
//	file->Seek(30);
	fseek(file, 30, SEEK_SET);
//	file->Read(tmp, sizeof(int));
	fread(tmp, sizeof(int), 1, file);
	memcpy(&compression, tmp, sizeof(int));
	TO_ENDIAN(compression);

	if (compression != 0)
	{
		printf("uses compression (not supported)\n");
		SAFE_DELETE_ARRAY(tmp);
		SAFE_DELETE_ARRAY(image);
		return NULL;
	}

	//pix/m X
	memset(tmp, 0, sizeof(int));
//	file->Seek(38);
	fseek(file, 38, SEEK_SET);
//	file->Read(tmp, sizeof(int));
	fread(tmp, sizeof(int), 1, file);
	memcpy(&res_x, tmp, sizeof(int));
	TO_ENDIAN(res_x);

	//pix/m Y
	memset(tmp, 0, sizeof(int));
	fseek(file, 42, SEEK_SET);
//	file->Seek(42);
//	file->Read(tmp, sizeof(int));
	fread(tmp, sizeof(int), 1, file);
	memcpy(&res_y, tmp, sizeof(int));
	TO_ENDIAN(res_y);

	if (res_x > 0)
	{
//		image->attrs.m_dpi_x = M2DPI(res_x);
	}

	if (res_y > 0)
	{
//		image->attrs.m_dpi_y = M2DPI(res_y);
	}

	//Get the file length
//	length = file->Length();
	fseek(file,0,SEEK_END);
	length = ftell(file);
	rawlength = (length) - (offset-1); //Remember to subtract 1 from the offset.

	//Allocate space
	SAFE_DELETE_ARRAY(image->m_data);
	image->m_data = new unsigned char[rawlength];
	
	memset(image->m_data, 0, rawlength);

	//Get raw data and convert BGR->RGB
//	file->Seek(offset);
	fseek(file, offset, SEEK_SET);
	
	image->m_ColorByteType = RGB;
	
	unsigned long row, pixel;
	unsigned char* rowPtr;
	
	for (unsigned int x = 0; !feof(file) && (x+3 < (unsigned)rawlength); x += 3)
	{
		memset(b, 0, sizeof(unsigned char) * 3);
//		file->Read((char*)&b, sizeof(unsigned char) * 3);
		fread(&b, sizeof(unsigned char) * 3, 1, file);
				
		row = image->m_height - (x/3)/image->m_width - 1; //current row
		pixel = (x/3)%image->m_width; //which pixel in the row
		rowPtr = image->m_data + (image->m_width * row * 3);
		rowPtr[pixel*3] = b[2];
		rowPtr[(pixel*3) + 1] = b[1];
		rowPtr[(pixel*3) + 2] = b[0];
	}
	
	SAFE_DELETE_ARRAY(tmp);
	printf( "BMP Load End\n" );
//	RID = image;
	return image;
}

//I think the fingerprint is actually just BM
REGISTER_IMAGE_TYPE(BM6, LoadBMP);
//REGISTER_IMAGE_TYPE(BM8, LoadBMP);

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
