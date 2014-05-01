
struct nds_header {
	unsigned char gameTitle[12];
	unsigned char fourCC[4];
	unsigned char producerId[2];
};
typedef struct nds_header NDSHeader;

struct nds_icon
{
	unsigned short version;				// always 1 ?
	unsigned short crc;					
	unsigned char  reserved[28];		// always zero ?
	         char  tiles[4][4][8][4];	// 4x4 tiles of 8x8 4-bit indexed pixels
	unsigned short palette[16];			// 0RGB_1555
	unsigned short title_jp[128];		// 3 UTF-16 lines separated by 0x0a00
	unsigned short title_en[128];
	unsigned short title_fr[128];
	unsigned short title_de[128];
	unsigned short title_it[128];
	unsigned short title_es[128];
};

typedef struct nds_icon NDSIcon;

void parseNDSInfo(const unsigned char * ndspath, NDSHeader * header, NDSIcon * icon);

CGImageRef CGImageCreateWithNDSIcon(const NDSIcon * icon);

CFStringRef CFStringCreateWithSerialNumber(const NDSHeader * header);
	


