/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#ifndef NO_IGRAPHICS

#import "IGraphicsIOS_view.h"
#include "IControl.h"
#include "IPlugParameter.h"
#include <vector>

@implementation IGraphicsIOS_View

- (id) initWithIGraphics: (IGraphicsIOS*) pGraphics
{
  TRACE;

  mGraphics = pGraphics;
  CGRect r;
  r.origin.x = r.origin.y = 0.0f;
  r.size.width = (float) pGraphics->WindowWidth();
  r.size.height = (float) pGraphics->WindowHeight();
  self = [super initWithFrame:r];

  self.layer.opaque = YES;
  self.layer.contentsScale = [UIScreen mainScreen].scale;
  
  self.multipleTouchEnabled = YES;
  
  return self;
}

- (void)setFrame:(CGRect)frame
{
  [super setFrame:frame];
  
  // During the first layout pass, we will not be in a view hierarchy, so we guess our scale
  CGFloat scale = [UIScreen mainScreen].scale;
  
  // If we've moved to a window by the time our frame is being set, we can take its scale as our own
  if (self.window)
    scale = self.window.screen.scale;
  
  CGSize drawableSize = self.bounds.size;
  
  // Since drawable size is in pixels, we need to multiply by the scale to move from points to pixels
  drawableSize.width *= scale;
  drawableSize.height *= scale;
  
  self.metalLayer.drawableSize = drawableSize;
}

//- (void) getTouchXY: (CGPoint) pt x: (float*) pX y: (float*) pY
//{
//  if (mGraphics)
//  {
//    *pX = pt.x / mGraphics->GetDrawScale();
//    *pY = pt.y / mGraphics->GetDrawScale();
//  }
//}

- (void) OnTouchEvent:(ITouchEvent::ETouchType)type withTouches:(NSSet*)touches withEvent:(UIEvent*)event
{
  ITouchEvent te;
  te.type = type;
  te.time = std::chrono::high_resolution_clock::now();
  NSEnumerator* pEnumerator = [[event allTouches] objectEnumerator];
  UITouch* pTouch;
  
  while ((pTouch = [pEnumerator nextObject]))
  {
    if ((te.numTouches + 1) < ITouchEvent::kMaxNumPoints)
    {
      CGPoint pos = [pTouch locationInView:pTouch.view];
      ITouchEvent::point& pt = te.points[te.numTouches++];
      pt.identifier = static_cast<void*>(pTouch);
      pt.x = pos.x;
      pt.y = pos.y;
      CGPoint posPrev = [pTouch previousLocationInView: self];
      pt.dx = pos.x - posPrev.x;
      pt.dy = pos.y - posPrev.y;
      pt.isChanged = [touches containsObject:pTouch];
      pt.radius = [pTouch majorRadius];
    }
  }
  
  std::vector<IMouseInfo> list;

  for (int i=0; i<te.numTouches; i++)
  {
    IMouseInfo e;
    e.ms.L = true;
    e.x = te.points[i].x / mGraphics->GetDrawScale();
    e.y = te.points[i].y / mGraphics->GetDrawScale();
    e.dx = te.points[i].dx / mGraphics->GetDrawScale();
    e.dy = te.points[i].dy / mGraphics->GetDrawScale();
    e.ms.P = i;
    e.ms.identifier = te.points[i].identifier;
    e.ms.changed = te.points[i].isChanged;
    e.ms.radius = te.points[i].radius;
    
    list.push_back(e);
    
    DBGMSG("%i - %f %f %i\n", i,  e.x, e.y, te.points[i].isChanged);
  }

  if(te.type == ITouchEvent::began)
    mGraphics->OnMouseDown(list);
  
  if(te.type == ITouchEvent::ended)
    mGraphics->OnMouseUp(list);
  
  if(te.type == ITouchEvent::moved)
    mGraphics->OnMouseDrag(list);
  
//  iosInputMgrPtr->onITouchEvent(newEvent);
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self OnTouchEvent:ITouchEvent::began withTouches:touches withEvent:event];
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self OnTouchEvent:ITouchEvent::moved withTouches:touches withEvent:event];
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self OnTouchEvent:ITouchEvent::ended withTouches:touches withEvent:event];
}

- (void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self OnTouchEvent:ITouchEvent::cancelled withTouches:touches withEvent:event];
}

//- (void) touchesBegan: (NSSet*) pTouches withEvent: (UIEvent*) pEvent
//{
//  NSArray* pArray = [pTouches allObjects];
//  std::vector<IMouseInfo> list;
//
//  for (int i=0; i<[pTouches count]; i++)
//  {
//    UITouch* pTouch = pArray[i];
//    CGPoint pt = [pTouch locationInView: self];
//
//    IMouseInfo e;
//    e.ms.L = true;
//    [self getTouchXY:pt x:&e.x y:&e.y];
//
//    list.push_back(e);
//  }
//
//  mGraphics->OnMouseDown(list);
//}
//
//- (void) touchesMoved: (NSSet*) pTouches withEvent: (UIEvent*) pEvent
//{
//  NSArray* pArray = [pTouches allObjects];
//  std::vector<IMouseInfo> list;
//
//  for (int i=0; i<[pTouches count]; i++)
//  {
//    UITouch* pTouch = pArray[i];
//    CGPoint pt = [pTouch locationInView: self];
//    CGPoint ptPrev = [pTouch previousLocationInView: self];
//
//    IMouseInfo e;
//    [self getTouchXY:pt x:&e.x y:&e.y];
//    float prevX, prevY;
//    [self getTouchXY:ptPrev x:&prevX y:&prevY];
//
//    e.dx = e.x - prevX;
//    e.dy = e.y - prevY;
//
//    list.push_back(e);
//  }
//
//  mGraphics->OnMouseDrag(list);
//}
//
//- (void) touchesEnded: (NSSet*) pTouches withEvent: (UIEvent*) pEvent
//{
//  NSArray* pArray = [pTouches allObjects];
//  std::vector<IMouseInfo> list;
//
//  for (int i=0; i<[pTouches count]; i++)
//  {
//    UITouch* pTouch = pArray[i];
//    CGPoint pt = [pTouch locationInView: self];
//    CGPoint ptPrev = [pTouch previousLocationInView: self];
//
//    IMouseInfo e;
//    [self getTouchXY:pt x:&e.x y:&e.y];
//
//    list.push_back(e);
//  }
//
//  mGraphics->OnMouseUp(list);
//}
//
//- (void) touchesCancelled: (NSSet*) pTouches withEvent: (UIEvent*) pEvent
//{
//  //  [self pTouchesEnded: pTouches withEvent: event];
//}

- (CAMetalLayer*) metalLayer
{
  return (CAMetalLayer *)self.layer;
}

- (void)dealloc
{
  [_displayLink invalidate];
  
  [super dealloc];
}

- (void)didMoveToSuperview
{
  [super didMoveToSuperview];
  if (self.superview)
  {
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(redraw:)];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
  }
  else
  {
    [self.displayLink invalidate];
    self.displayLink = nil;
  }
}

- (void)redraw:(CADisplayLink*) displayLink
{
  IRECTList rects;
  
  if (mGraphics->IsDirty(rects))
  {
    mGraphics->SetAllControlsClean();
    mGraphics->Draw(rects);
  }
}

- (BOOL) isOpaque
{
  return YES;
}

- (BOOL) acceptsFirstResponder
{
  return YES;
}

- (BOOL)canBecomeFirstResponder
{
  return YES;
}

- (void) removeFromSuperview
{
  [self.displayLink invalidate];
  self.displayLink = nil;
}

- (void) controlTextDidEndEditing: (NSNotification*) aNotification
{
}

- (IPopupMenu*) createPopupMenu: (const IPopupMenu&) menu : (CGRect) bounds;
{
  return nullptr;
}

- (void) createTextEntry: (IControl&) control : (const IText&) text : (const char*) str : (CGRect) areaRect;
{
 
}

- (void) endUserInput
{
}

+ (Class)layerClass
{
  return [CAMetalLayer class];
}

@end

#endif //NO_IGRAPHICS
