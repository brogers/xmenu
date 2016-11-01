#include <ApplicationServices/ApplicationServices.h>
#include "draw.h"

#define INPUT_SPACE_FRACTION 0.3

typedef struct {
  CFStringRef str;
  CFAttributedStringRef attrStr;
  CTLineRef line;
  CGFloat w;
} TextGraphic;

static TextGraphic space;
static TextGraphic dots;

void initDraw(DrawCtx *drawCtx) {
  space.str = CFSTR(" ");
  dots.str = CFSTR("...");
  space.attrStr = mkAttrString(drawCtx, space.str, drawCtx->nfg);
  space.line = CTLineCreateWithAttributedString(space.attrStr);
  space.w = CTLineGetTypographicBounds(space.line, NULL, NULL, NULL);
  dots.attrStr = mkAttrString(drawCtx, dots.str, drawCtx->nfg);
  dots.line = CTLineCreateWithAttributedString(dots.attrStr);
  dots.w = CTLineGetTypographicBounds(dots.line, NULL, NULL, NULL);
}

bool drawText(CGContextRef ctx, DrawCtx *drawCtx, CFStringRef itemName, bool sel) {
  CGColorRef fg, bg;
  if (sel) {
    fg = drawCtx->sfg;
    bg = drawCtx->sbg;
  } else {
    fg = drawCtx->nfg;
    bg = drawCtx->nbg;
  }
  CFAttributedStringRef attrItemName = mkAttrString(drawCtx, itemName, fg);
  CTLineRef line = CTLineCreateWithAttributedString(attrItemName);
  CGFloat w = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
  if ((drawCtx->x + w + 2 * space.w) > drawCtx->w) {
    // off the right edge of the screen, stop drawing words
    return false;
  }
  CGContextSetFillColorWithColor(ctx, bg);
  CGContextFillRect(ctx, CGRectMake(drawCtx->x, 0, w + 2 * space.w, drawCtx->h));
  CGFloat y = (drawCtx->h - drawCtx->font_siz);
  CGContextSetTextPosition(ctx, drawCtx->x + space.w, y);
  CTLineDraw(line, ctx);
  drawCtx->x += w + 2 * space.w;
  CFRelease(line);
  CFRelease(attrItemName);
  return true;
}

void drawInput(CGContextRef ctx, DrawCtx *drawCtx, CFStringRef input) {
  CGFloat inputW = drawCtx->w * INPUT_SPACE_FRACTION;
  CGColorRef fg = drawCtx->nfg;
  CGColorRef bg = drawCtx->nbg;

  CFAttributedStringRef attrInput = mkAttrString(drawCtx, input, fg);
  CTLineRef line = CTLineCreateWithAttributedString(attrInput);
  CGFloat w = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
  CGContextSetFillColorWithColor(ctx, bg);
  CGContextFillRect(ctx, CGRectMake(drawCtx->x, 0, w + 2 * space.w, drawCtx->h));
  CGFloat y = (drawCtx->h - drawCtx->font_siz);
  CGContextSetTextPosition(ctx, drawCtx->x + space.w, y);
  CTLineDraw(line, ctx);
  CFRelease(line);
  CFRelease(attrInput);
  if (w + 2 * space.w <= inputW) {
    goto end;
  }
  CGContextSetFillColorWithColor(ctx, bg);
  CGContextFillRect(ctx, CGRectMake(drawCtx->x + inputW - (dots.w + space.w), 0,
                                    drawCtx->w - (drawCtx->x + w), drawCtx->h));
  CGContextSetTextPosition(ctx, drawCtx->x + inputW - (dots.w + space.w), y);
  CTLineDraw(dots.line, ctx);

end:
  drawCtx->x += inputW;
}

void clearRight(CGContextRef ctx, DrawCtx *drawCtx) {
  CGContextSetFillColorWithColor(ctx, drawCtx->nbg);
  CGContextFillRect(ctx, CGRectMake(drawCtx->x, 0, drawCtx->w - drawCtx->x, drawCtx->h));
}

CFAttributedStringRef mkAttrString(DrawCtx *drawCtx, CFStringRef str, CGColorRef color) {
  CFStringRef keys[] = {kCTFontAttributeName, kCTForegroundColorAttributeName};
  CFTypeRef values[] = {drawCtx->font, color};
  CFDictionaryRef attrs =
      CFDictionaryCreate(kCFAllocatorDefault, (const void **)&keys, (const void **)&values,
                         sizeof keys / sizeof(CFStringRef), &kCFTypeDictionaryKeyCallBacks,
                         &kCFTypeDictionaryValueCallBacks);
  return CFAttributedStringCreate(kCFAllocatorDefault, str, attrs);
}

CGColorRef mkColor(const char *hex_color) {
  if (!hex_color || *hex_color != '#') return NULL;
  hex_color++;
  CGFloat rgba[4];
  int t;
  switch (strlen(hex_color)) {
    case 3:
      if (1 != sscanf(hex_color, "%3x", &t)) return NULL;
      rgba[0] = (t >> 8) | ((t >> 4) & 0xF0);
      rgba[1] = ((t >> 4) & 0xF) | (t & 0xF0);
      rgba[2] = (t & 0xF) | ((t << 4) & 0xF0);
      break;
    case 6:
      if (1 != sscanf(hex_color, "%6x", &t)) return NULL;
      rgba[0] = (t >> 16) & 0xFF;
      rgba[1] = (t >> 8) & 0xFF;
      rgba[2] = t & 0xFF;
      break;
    default:
      return NULL;
  }
  rgba[0] /= 255.0;
  rgba[1] /= 255.0;
  rgba[2] /= 255.0;
  rgba[3] = 1.0;
  return CGColorCreate(CGColorSpaceCreateDeviceRGB(), rgba);
}
