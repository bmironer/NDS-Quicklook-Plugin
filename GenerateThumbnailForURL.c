#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "nds.h"

OSStatus GenerateThumbnailForURL(void *thisInterface, 
								 QLThumbnailRequestRef thumbnail,
								 CFURLRef url, CFStringRef contentTypeUTI, 
								 CFDictionaryRef options, CGSize maxSize)
{
	CGFloat minCartDrawSize = 64;
	CGFloat size = fmin(maxSize.width, maxSize.height);
	CGContextRef qlContext = NULL;
	if (size < minCartDrawSize) { 
		size = 32;
		qlContext = QLThumbnailRequestCreateContext(thumbnail, CGSizeMake(size,size), true, NULL);
	}
	else 
	{
		qlContext = QLThumbnailRequestCreateContext(thumbnail, CGSizeMake(size,size), false, NULL);
	}
	
	if (qlContext) {
		NDSHeader header;	
		NDSIcon icon;
		int pathlen = 2047;
		UInt8 path[pathlen+1];
		if (CFURLGetFileSystemRepresentation(url, true, path, pathlen)) { 
			
			if (size < minCartDrawSize) { // at smaller sizes we only draw the game icon at native size and let QL do the scaling
				parseNDSInfo(path, &header, &icon);
				CGImageRef image = CGImageCreateWithNDSIcon(&icon);
				if (image) {
					CGContextDrawImage(qlContext, CGRectMake(0, 0, size, size), image);
					CGImageRelease(image);
				}
			}
			else 
			{
				CGContextClearRect(qlContext, CGRectMake(0,0,size,size));
				
				// draw cartridge background
				CFURLRef bgURL = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(
																						 CFSTR("net.mironer.nds.quicklookgenerator")), CFSTR("background"), CFSTR("png"), NULL);
				if (bgURL) {
					CGDataProviderRef bgPNG = CGDataProviderCreateWithURL(bgURL);
					if (bgPNG) {
						CGImageRef bg = CGImageCreateWithPNGDataProvider(bgPNG, NULL, true, kCGRenderingIntentDefault);
						if (bg) {
							CGContextDrawImage(qlContext, CGRectMake(0, 0, size, size), bg);				
							CGImageRelease(bg);
						}
						CGDataProviderRelease(bgPNG);
					}
					CFRelease(bgURL);
				}			
				
				// draw game icon
				parseNDSInfo(path, &header, &icon);
				CGImageRef image = CGImageCreateWithNDSIcon(&icon);
				if (image) {
					CGContextDrawImage(qlContext, CGRectMake(size / 5, size / 5, size * 3 / 5, size * 3 / 5), image);
					CGImageRelease(image);
				}
				
				// draw cartridge overlay
				CFURLRef ovrURL = CFBundleCopyResourceURL( 
														  CFBundleGetBundleWithIdentifier(
																						  CFSTR("net.mironer.nds.quicklookgenerator")), CFSTR("overlay"), CFSTR("png"), NULL);
				if (ovrURL) {
					CGDataProviderRef ovrPNG = CGDataProviderCreateWithURL(ovrURL);
					if (ovrPNG) {
						CGImageRef ovr = CGImageCreateWithPNGDataProvider(ovrPNG, NULL, true, kCGRenderingIntentDefault);
						if (ovr) {
							CGContextDrawImage(qlContext, CGRectMake(0, 0, size, size), ovr);				
							CGImageRelease(ovr);
						}
						CGDataProviderRelease(ovrPNG);
					}
					CFRelease(ovrURL);
				}			
				
				// draw serial number
				CFStringRef serial = CFStringCreateWithSerialNumber(&header);
				CFAttributedStringRef aString = CFAttributedStringCreate(kCFAllocatorDefault, serial, NULL);
				CFMutableAttributedStringRef attrStr = CFAttributedStringCreateMutableCopy(kCFAllocatorDefault, 128, aString);
				CFIndex len =  CFAttributedStringGetLength(attrStr);
				CTFontRef font = CTFontCreateWithName(CFSTR("Lucida Sans"), size / 16, NULL);
				CFAttributedStringSetAttribute(attrStr, CFRangeMake(0, len), kCTFontAttributeName, font);		
				
				CTTextAlignment rightAlign = kCTRightTextAlignment;
				CTParagraphStyleSetting styleSettings[] = { {kCTParagraphStyleSpecifierAlignment, sizeof(rightAlign), &rightAlign} };
				CTParagraphStyleRef paragraphStyle = CTParagraphStyleCreate(styleSettings, 1);
				CFAttributedStringSetAttribute(attrStr, CFRangeMake(0, len), kCTParagraphStyleAttributeName, paragraphStyle);
				
				CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(attrStr);
				CFRelease(attrStr);
				CFRelease(aString);
				CFRelease(serial);
				CFRelease(font);
				
				CGMutablePathRef path = CGPathCreateMutable();
				CGPathAddRect(path, NULL, CGRectMake(-size * 1.55 / 8, -size * 6.1 / 8, size, size));			
				CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), path, NULL);
				CTFrameDraw(frame, qlContext);
				CFRelease(frame);
				CFRelease(framesetter); 
				CFRelease(paragraphStyle);
				CFRelease(path);
			}
		}
		QLThumbnailRequestFlushContext(thumbnail, qlContext);
        CFRelease(qlContext);
    }
    return noErr;
}

void CancelThumbnailGeneration(void* thisInterface, QLThumbnailRequestRef thumbnail)
{
    // implement only if supported
}
 