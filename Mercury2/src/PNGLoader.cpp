#include <ImageLoader.h>
#include <png.h>
#include <MercuryUtil.h>

#include <assert.h>

void PNGRead( png_struct *png, png_byte *p, png_size_t size )
{
	MercuryFile * f = (MercuryFile*)png->io_ptr;

	int got = f->Read( p, size );
//	int got = fread(p, size, 1, f );

	if( got == -1 )
		png_error( png, "Error reading from file");
//	else if( got != size )
//		png_error( png, "Unexpected EOF" );
}

RawImageData* LoadPNG( MercuryFile * fp )
{
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep* row_pointers;
	png_byte color_type;
	png_byte bit_depth;
	RawImageData* image = 0;
	unsigned char header[8];	// 8 is the maximum size that can be checked

	//open file and test for it being a png 
	if (!fp)
		assert("[read_png_file] File %s could not be opened for reading");
	fp->Read(header, 8 );
//	fread(header, 8, 1, fp);
	if (png_sig_cmp(header, 0, 8))
		assert("[read_png_file] File %s is not recognized as a PNG file");


	//initialize stuff 
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		assert("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		assert("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		assert("[read_png_file] Error during init_io");

	png_set_sig_bytes(png_ptr, 8);
	png_set_read_fn( png_ptr, fp, PNGRead );

	png_read_info(png_ptr, info_ptr);

	image = new RawImageData;

	image->m_width = info_ptr->width;
	image->m_height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

//	if ( color_type & PNG_COLOR_MASK_PALETTE )
//	{
//		SAFE_DELETE(image);
//		assert("Cannot open paletted PNG files.");
//	}
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// read file 
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		SAFE_DELETE_ARRAY(image);
		assert("[read_png_file] Error during read_image");
	}

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image->m_height);
	unsigned int y;
	for ( y=0; y < (unsigned)image->m_height; y++)
		row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);

	png_read_image(png_ptr, row_pointers);

	png_read_end( png_ptr, info_ptr );
	png_destroy_read_struct( &png_ptr, &info_ptr, NULL );

	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);

	if (color_type & PNG_COLOR_MASK_ALPHA)
		image->m_ColorByteType = RGBA;	
	else
		image->m_ColorByteType = RGB;	

//	SAFE_DELETE(texture->m_data);
	
	image->m_data = new unsigned char[sizeof(unsigned char) * image->m_height * image->m_width *  4];

	switch (image->m_ColorByteType)
	{
		case RGBA:
			for ( y=0; y < (unsigned)image->m_height; ++y) {
				png_byte* row = row_pointers[y];
				for (unsigned long x = 0; x < image->m_width; ++x) {
					png_byte* ptr = &(row[x*4]);
					image->m_data[(x + y * image->m_width) * 4] = ptr[0];
					image->m_data[(x + y * image->m_width) * 4 + 1] = ptr[1];
					image->m_data[(x + y * image->m_width) * 4 + 2] = ptr[2];
					image->m_data[(x + y * image->m_width) * 4 + 3] = ptr[3];
				}	
			}
			break;
		case RGB:
			for ( y=0; y < (unsigned)image->m_height; y++) {
				png_byte* row = row_pointers[y];
				for (unsigned long x=0; x<image->m_width; x++) {
					png_byte* ptr = &(row[x * 3]);
					image->m_data[(x + y * image->m_width) * 3] = ptr[0];
					image->m_data[(x + y * image->m_width) * 3 + 1] = ptr[1];
					image->m_data[(x + y * image->m_width) * 3 + 2] = ptr[2];
				}	
			}
			break;
		default:
			printf("Invalid color byte type for PNG.\n");
			SAFE_DELETE_ARRAY( image );
			return false;
	}

	for ( y=0; y < (unsigned)image->m_height; y++)
		SAFE_FREE(row_pointers[y]);
	SAFE_FREE(row_pointers);

//	texture->CorrectSize();
//	texture->CreateCache(); 

	return image;
}

REGISTER_IMAGE_TYPE(‰PN, LoadPNG);

/* 
 * Copyright (c) 2004 Glenn Maynard
 * (c) 2008 Joshua Allen
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