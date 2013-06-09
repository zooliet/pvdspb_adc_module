#ifndef	_VT_BMP_H_
#define	_VT_BMP_H_

#define	RGB16(r,g,b) (unsigned short)((((unsigned short)(r&0xF8))<<8)+(((unsigned short)(g&0xFC))<<3)+(((unsigned short)(b&0xF8))>>3))

#define	RGB24(r,g,b)	\
						(unsigned int)(\
						(((unsigned int)(r&0xFF)<<16))|\
						(((unsigned int)(g&0xFF)<<8))|\
						((unsigned int)(b&0xFF))\
						)


#define	GET_RED(c)		((unsigned char)((((unsigned int)(c))&0xFF0000)>>16))
#define	GET_GREEN(c)	((unsigned char)((((unsigned int)(c))&0xFF00)>>8))
#define	GET_BLUE(c)		((unsigned char)(((unsigned int)(c))&0xFF))

#define	GET16_RED(c)	((unsigned char)(((c)&0xF800)>>8))
#define	GET16_GREEN(c)	((unsigned char)(((c)&0x07E0)>>3))
#define	GET16_BLUE(c)	((unsigned char)((c)&0x1F)<<3)

#define CONV_24TO16(c)	\
						((unsigned short)\
						RGB16(\
						GET_RED(c),\
						GET_GREEN(c),\
						GET_BLUE(c)\
						))
						

#define CONV_16TO24(c)	\
						((unsigned int)\
						RGB24(\
						GET16_RED(c),\
						GET16_GREEN(c),\
						GET16_BLUE(c)\
						))

/*
Bit map file header
bfType            2    // "BM"... ... ...
bfSize            4    // ... ... .. ..
bfReserved1        2    // ....(0.. ...)
bfReserved2        2    // ....(0.. ...)
bfOffBits        4    // .... ... .... .. ..

bitmap info header
biSize            4    // .... ..
biWidth            4    // .... .. ..
biHeight        4    // .... .. ..
biPlanes        2    // Plane.(1. ...)
biBitCount        2    // . ... ...
biCompression    4    // .. .. ...
biSizeImage        4    // .. .... ..
biXPelsPerMeter 4    // . ... .. ..
biYPelsPerMeter 4    // . ... .. ..
biClrUsed        4    // .... .. .... .. .
biClrImportant    4    // .... .... ..
//==============================================================================
biSize            4    This is the size of the BMPINFOHEADER structure.
biWidth            4    The width of the bitmap, in pixels.
biHeight        4    The height of the bitmap, in pixels.
biPlanes        2    Set to 1.
biBitCount        2    The bit depth of the bitmap. For 24-bit bitmaps, this is 24.
biCompression    4    Our bitmaps are uncompressed, so this field is set to 0.
biSizeImage        4    The size of the padded image, in bytes.
biXPelsPerMeter 4    Horizontal resolution, in pixels per meter, of device displaying bitmap. Thisnumber is not significant for us, and will be set to 0.
biYPelsPerMeter 4    Vertical resolution, in pixels per meter, of device displaying bitmap. Thisnumber is not significant for us, and will be set to 0.
biClrUsed        4    This number does not apply to 24-bit bitmaps. Set to zero.
biClrImportant    4    This number does not apply to 24-bit bitmaps. Set to zero
 */

typedef struct tagBITMAPFILEHEADER {	// 14 = 2+4+2+2+4 (byte)
	unsigned short	bfType;				// "BM"... ... ...
	unsigned int	bfSize;				// ... ... .. ..
	unsigned short	bfReserved1;		// ....(0.. ...)
	unsigned short	bfReserved2;		// ....(0.. ...)
	unsigned int	bfOffBits;			// .... ... .... .. ..
} BITMAPFILEHEADER, ST_BMP_FILE_HEADER;

typedef struct tagBITMAPINFOHEADER {	// 40 = 4+4+4+2+2+4+4+4+4+4+4 (byte)
	unsigned int	biSize;				// .... ..
	unsigned int	biWidth;			// .... .. ..
	unsigned int	biHeight;			// .... .. ..
	unsigned short	biPlanes;			// Plane.(1. ...)
	unsigned short	biBitCount;			// . ... ...
	unsigned int	biCompression;		// .. .. ...
	unsigned int	biSizeImage;		// .. .... ..
	unsigned int	biXPelsPerMeter;	// . ... .. ..
	unsigned int	biYPelsPerMeter;	// . ... .. ..
	unsigned int	biClrUsed;			// .... .. .... .. .
	unsigned int	biClrImportant;		// .... .... ..
} BITMAPINFOHEADER, ST_BMP_INFO_HEADER;

typedef struct tagRGBQUAD {				// 4
	unsigned char	rgbBlue;			// B.. (..)
	unsigned char	rgbGreen;			// G.. (..)
	unsigned char	rgbRed;				// R.. (..)
	unsigned char	rgbReserved;
} RGBQUAD, ST_RGB_QUAD;

typedef struct tagBITMAPINFO {
	BITMAPFILEHEADER	bmfHeader;
	BITMAPINFOHEADER	bmiHeader;
	unsigned char		data[1];
} BITMAPINFO, ST_BMP_INFO;



#endif	//#ifndef	_VT_BMP_H_
