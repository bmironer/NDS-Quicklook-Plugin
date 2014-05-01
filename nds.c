#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "nds.h"

static const char regionKeys[] = { 'D', 'E', 'F', 'H', 'I', 'J', 'K', 'P', 'S', 'X' };

void parseNDSInfo(const unsigned char * ndspath, NDSHeader * header, NDSIcon * icon) {
	UInt8 off[4];
	FILE * fd = fopen((char*)ndspath, "r");
	if (header) {
		fread(header, sizeof(NDSHeader), 1, fd); 
	}
	fseek(fd, 0x68, SEEK_SET);
	fread(off, 1, 4, fd);
	UInt32 offset = off[0] | off[1] << 8 | off[2] << 16 | off[3] << 24;
	fseek(fd, offset, SEEK_SET);
	if (icon) {
		fread(icon, sizeof(NDSIcon), 1, fd);
	}
	fclose(fd);
}

CGImageRef CGImageCreateWithNDSIcon(const NDSIcon * icon) {
	UInt32 bitmap[32][32];
	for(int j=0; j<32; ++j) {
		for (int i=0; i<32; ++i) {
			char p = i % 2 ? 
			(icon->tiles[j/8][i/8][j%8][(i%8)/2] & 0xf0) >> 4 : 
			(icon->tiles[j/8][i/8][j%8][(i%8)/2] & 0x0f);
			short bgr555 = icon->palette[p];
			char a = ( bgr555 == icon->palette[0]) ? 0x00 : 0xff;
			char b = ((bgr555 >> 10 ) & 0x1f);
			char g = ((bgr555 >> 5  ) & 0x1f);
			char r = ((bgr555       ) & 0x1f); 
			bitmap[j][i] = a << 24 | b << 19 | g << 11 | r << 3; 
		}
	}		
	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGContextRef bitmapContext = CGBitmapContextCreate(bitmap, 32, 32, 8, 32 * 4, colorSpace, kCGImageAlphaPremultipliedLast);
	CGImageRef image = CGBitmapContextCreateImage(bitmapContext);
	CFRelease(bitmapContext);
	CFRelease(colorSpace);
	return image;
}

CFStringRef CFStringCreateWithSerialNumber(const NDSHeader * header) {
	char serialBuf[13] = "NTR-XXXX-XXX";
	serialBuf[4] = header->fourCC[0];
	serialBuf[5] = header->fourCC[1];
	serialBuf[6] = header->fourCC[2];
	serialBuf[7] = header->fourCC[3];
	switch (serialBuf[7]) {
			
		case 'E':
			serialBuf[9] = 'U';
			serialBuf[10]= 'S';
			serialBuf[11]= 'A';
			break;
			
		case 'J':
			serialBuf[9] = 'J';
			serialBuf[10]= 'P';
			serialBuf[11]= 'N';
			break;
			
		case 'P': 
			serialBuf[9] = 'E';
			serialBuf[10]= 'U';
			serialBuf[11]= 'R';
			break;
		
		case 'V':
			serialBuf[9] = 'A';
			serialBuf[10]= 'U';
			serialBuf[11]= 'S';
			break;
			
		default:
			break;
	}
	
	CFStringRef serialRef = CFStringCreateWithCString(kCFAllocatorDefault, serialBuf, kCFStringEncodingUTF8 );
	return serialRef;
}

