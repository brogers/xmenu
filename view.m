#import <Cocoa/Cocoa.h>
#import "draw.h"
#import "util.h"
#import "view.h"
#include <string.h>

#define promptCStr "$"

@implementation XmenuMainView

- (BOOL)acceptsFirstResponder { return YES; }

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame: frame];
	return self;
}

- (id)initWithFrame:(NSRect)frame items:(ItemList)itemList
{
	self = [super initWithFrame: frame];
	self.itemList = itemList;
	return self;
}

- (void)keyUp:(NSEvent*)event
{
	NSLog(@"Key released: %@", event);
}

- (void)keyDown:(NSEvent*)event
{
	switch ([event keyCode]) {
	case 126:
	case 125:
	case 124:
	case 123:
		NSLog(@"Arrow key pressed!");
		break;
	default:
		NSLog(@"Key pressed: %@", event);
		break;
	}
}

- (void)drawRect:(NSRect)rect
{
	DrawCtx drawCtx;
	drawCtx.nbg = mkColor("#ffffff");
	drawCtx.sbg = mkColor("#f00");
	drawCtx.nfg = mkColor("#0F0");
	drawCtx.sfg = mkColor("#00f");
	drawCtx.x = 0;
	drawCtx.font_siz = 14.0; // TODO: Fix shadows
	[[NSColor colorWithCGColor: drawCtx.nbg] set];
	NSRectFill(rect);

	CFStringRef promptStr = CFStringCreateWithCString(NULL, pad(promptCStr), kCFStringEncodingUTF8);
	CFStringRef psFont = CFStringCreateWithCString(NULL, "Consolas", kCFStringEncodingUTF8);
	CTFontDescriptorRef fontDesc = CTFontDescriptorCreateWithNameAndSize(psFont, drawCtx.font_siz);
	CTFontRef font = CTFontCreateWithFontDescriptor(fontDesc, 0.0, NULL);
	CFRelease(psFont);
	drawCtx.font = font;
	drawCtx.h = rect.size.height;
	drawCtx.w = rect.size.width;

	CGContextRef ctx = [[NSGraphicsContext currentContext] graphicsPort];

	drawText(ctx, &drawCtx, promptStr, true);
	drawInput(ctx, &drawCtx);
	ItemList list = self.itemList;
	for (int i = 0; i < list.len; i++) {
		Item *itemp = list.item+i;
		if (!drawText(ctx, &drawCtx, itemp->text, itemp->sel)) {
			break;
		}
	}
}

@end

@implementation BorderlessWindow

- (BOOL)canBecomeKeyWindow  { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }

@end
