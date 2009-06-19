#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif


FILE * fout;

int my_write_png( const char * fname, unsigned char * imagedata, int x, int y );
int my_read_font( const char * fname, unsigned char * imagedata, int xpp, int ypp, int xq, int yq );

int main( int argc, char ** argv )
{
	unsigned char imagedata[1024*1024*4];
	int x, y;

	if( argc != 3 )
	{
		fprintf( stderr, "Mercury Fonter\n" );
		fprintf( stderr, "Usage: %s [font file] [output png]\n", argv[0] );
		exit( -1 );
	}

	for( int x = 0; x < 1024; x++ )
		for( int y = 0; y < 1024; y++ )
		{
			imagedata[(x+y*1024)*2+0] = 255;	//red
			imagedata[(x+y*1024)*2+1] = 255;	//alpha
//			imagedata[(x+y*1024)*4+2] = 255;	//blue
//			imagedata[(x+y*1024)*4+3] = 255;	//alpha
		}

	my_read_font( argv[1], imagedata, 64, 64, 16, 16 );
	my_write_png( argv[2], imagedata, 1024, 1024 );
}

int my_read_font( const char * fname, unsigned char * imagedata, int xpp, int ypp, int xq, int yq )
{
	FT_Library library;
	FT_Face face;
	int error;
	int glyph_index;
	int x;
	int y;

	error = FT_Init_FreeType( &library );
	if( error )
	{
		fprintf( stderr, "Error.  Could not initialize freetype\n" );
		exit( -1 );
	}

	error = FT_New_Face( library, fname, 0, &face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		fprintf( stderr, "Error.  FT_Err_Unknown_File_Format\n" );
		exit( -1 );
	}
	else if ( error )
	{
		fprintf( stderr, "Error.  Something went wrong with FT_NewFace.\n" );
		exit( -1 );
	}

	error = FT_Set_Pixel_Sizes( face, 0, 64 );
	if( error )
	{
		fprintf( stderr, "Error with FT_Set_Pixel_Sizes\n" );
		exit( -3 );
	}

	glyph_index = FT_Get_Char_Index( face, 65 );

	error = FT_Load_Glyph( face,
		glyph_index, 0 ); //FT_LOAD_NO_BITMAP (try as last thing)

	if( error )
	{
		fprintf( stderr, "FT_Load_Glyph had a problem.\n" );
		exit( -4 );
	}

	error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
	if( error )
	{
		fprintf( stderr, "FT_Render_Glyph had a problem.\n" );
		exit( -4 );
	}

	FT_Bitmap l_bitmap = face->glyph->bitmap;

	for( int x = 0; x < 1024; x++ )
		for( int y = 0; y < 1024; y++ )
		{
			if( x > l_bitmap.width  ) continue;
			if( y > l_bitmap.rows ) continue;
			imagedata[(x+y*1024)*2+1] = face->glyph->bitmap.buffer[x+y*l_bitmap.width ];
		}

}

int my_write_png( const char * fname, unsigned char * imagedata, int width, int height )
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 k;
	png_bytep row_pointers[height];

	memset( &info_ptr, 0, sizeof( info_ptr ) );

	fout = fopen( fname, "wb" );
	if( !fout )
	{
		fprintf( stderr, "Error. Could not open output file.\n" );
		exit( -1 );
	}

	png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );

	if (png_ptr == NULL)
	{
		fclose(fout);
		fprintf( stderr, "png_create_write_struct error.\n" );
		exit( -1 );
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fout);
		fprintf( stderr, "png_create_info_struct error.\n" );
		png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
		exit( -2 );
	}

	png_set_IHDR( png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* If we get here, we had a problem reading the file */
		fclose(fout);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fprintf( stderr, "Something bad happened.\n" );
		exit( -3 );
	}

	png_init_io( png_ptr, fout );
	png_write_info( png_ptr, info_ptr );


	for (k = 0; k < height; k++)
		row_pointers[k] = imagedata + k*width*2;

	png_write_image( png_ptr, row_pointers );

	png_write_end( png_ptr, info_ptr );

	return 0;

}