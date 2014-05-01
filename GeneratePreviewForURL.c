#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "nds.h"


OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{	
	NDSHeader header;	
	NDSIcon icon;
	int pathlen = 2047;
	UInt8 path[pathlen+1];
	CFMutableDictionaryRef properties = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, NULL, NULL);
	if (CFURLGetFileSystemRepresentation(url, true, path, pathlen)) {
		parseNDSInfo(path, &header, &icon);
		CFDictionarySetValue(properties, kQLPreviewPropertyDisplayNameKey, CFStringCreateWithBytes(kCFAllocatorDefault, (unsigned char*)icon.title_en, 256, kCFStringEncodingUTF16, false));
	}
	CGContextRef qlContext = QLPreviewRequestCreateContext(preview, CGSizeMake(512, 512), false, properties);
	if (qlContext) {
		CFRelease(qlContext);
	}
    return noErr;
}

void CancelPreviewGeneration(void* thisInterface, QLPreviewRequestRef preview)
{
    // implement only if supported
}
