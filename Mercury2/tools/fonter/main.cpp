#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#define FONTSIZE 64

FILE * fontfile;

int my_write_png( const char * fname, unsigned char * imagedata, int x, int y );
int my_read_font( const char * fname, unsigned char * imagedata, int xpp, int ypp, int xq, int yq );

int main( int argc, char ** argv )
{
	unsigned char imagedata[1024*1024*4];
	int x, y;

	if( argc != 3 )
	{
		fprintf( stderr, "Mercury Fonter\n" );
		fprintf( stderr, "Usage: %s [font file] [font name]\n", argv[0] );
		exit( -1 );
	}

	for( int x = 0; x < 1024; x++ )
		for( int y = 0; y < 1024; y++ )
		{
			imagedata[(x+y*1024)*2+0] = 255;	//red
			imagedata[(x+y*1024)*2+1] = 0;	//alpha
//			imagedata[(x+y*1024)*4+2] = 255;	//blue
//			imagedata[(x+y*1024)*4+3] = 255;	//alpha
		}

	char fname[1024];
	char pngname[1024];

	sprintf( fname, "%s.%s", argv[2], "hgfont" );
	sprintf( pngname, "%s.%s", argv[2], "png" );

	fontfile = fopen( fname, "w" );
	if( !fontfile )
	{
		fprintf( stderr, "Could not open font output file: %s\n", fname );
		exit( -1 );
	}
	fprintf( fontfile, "%s\n%d %f %f %f\n", pngname, FONTSIZE, 4.f, 15.f, 80.f );

	my_read_font( argv[1], imagedata, 64, 64, 16, 16 );
	my_write_png( pngname, imagedata, 1024, 1024 );
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

	error = FT_Set_Pixel_Sizes( face, 0, FONTSIZE );
	if( error )
	{
		fprintf( stderr, "Error with FT_Set_Pixel_Sizes\n" );
		exit( -3 );
	}

	int actualchar = 0;
	for( int ch = 0; ch < xq * yq; ch++ )
	{
		glyph_index = FT_Get_Char_Index( face, actualchar );
	
		error = FT_Load_Glyph( face, glyph_index, 0 ); //FT_LOAD_NO_BITMAP (try as last parameter)
	
		if( error )
		{
			fprintf( stderr, "FT_Load_Glyph had a problem.\n" );
			exit( -4 );
		}
	
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ); //try FT_LOAD_NO_HINTING 
		if( error )
		{
			fprintf( stderr, "FT_Render_Glyph had a problem.\n" );
			exit( -4 );
		}

		FT_Bitmap l_bitmap = face->glyph->bitmap;

		int goffx = (xpp*(ch%xq));
		int goffy = (ypp*(ch/xq));
		for( int x = 0; x < xpp; x++ )
			for( int y = 0; y < ypp; y++ )
			{
				int offx = x+goffx;
				int offy = y+goffy;

				if( x >= l_bitmap.width  ) continue;
				if( y >= l_bitmap.rows ) continue;

				if( offx < 0 || offx >= xq*xpp ) continue;
				if( offy < 0 || offy >= yq*ypp ) continue;

				imagedata[(offx + offy*(xq*xpp))*2+1] = l_bitmap.buffer[x+y*l_bitmap.pitch ];
				imagedata[(offx + offy*(xq*xpp))*2+0] = 255;

			}

		float loffx = float(goffx)/float(xpp*xq);
		float loffy = float(goffy)/float(ypp*yq);
		float loffxe = float(goffx+l_bitmap.width)/float(xpp*xq);
		float loffye = float(goffy+l_bitmap.rows)/float(ypp*yq);

		fprintf( fontfile, "%d %f %f %f %f  %d %d %d %d\n", actualchar, loffx, loffy, loffxe, loffye,
			l_bitmap.width, l_bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top );

		actualchar++;
	}


/* Draw grid
	for( int x = 0; x < 1024; x+=64 )
	for( int y = 0; y < 1024; y++ )
	{
		imagedata[(x+y*(xq*xpp))*2+0] = 0;
		imagedata[(x+y*(xq*xpp))*2+1] = 255;
	}
	for( int x = 0; x < 1024; x++ )
	for( int y = 0; y < 1024; y+=64 )
	{
		imagedata[(x+y*(xq*xpp))*2+0] = 0;
		imagedata[(x+y*(xq*xpp))*2+1] = 255;
	}
*/
}

int my_write_png( const char * fname, unsigned char * imagedata, int width, int height )
{
	FILE * fout;
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
