/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @copydoc TestMTControl
 */

#include "IControl.h"

 /** Control to test multi touch
  *   @ingroup TestControls */
class TestMTControl : public IControl
{
public:
  TestMTControl(IRECT bounds)
   : IControl(bounds)
  {
  }

  void Draw(IGraphics& g) override
  {
    g.FillRect(COLOR_ORANGE, mRECT);

    for (auto p : mPoints)
    {
      if (p.down)
        g.FillCircle(p.color, mRECT.L + p.x * mRECT.W(), mRECT.T + p.y * mRECT.H(), p.radius);
    }

    g.DrawRect(COLOR_BLACK, mRECT);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    mPoints[mod.P].down = true;
    OnMouseDrag(x, y, 0., 0., mod);
  }

  void OnMouseUp(float x, float y, const IMouseMod& mod) override
  {
    mPoints[mod.P].down = false;
    SetDirty();
  }

  void OnMouseDrag(float x, float y, float dx, float dy, const IMouseMod& mod) override
  {
    mPoints[mod.P].radius = mod.radius;
    mPoints[mod.P].x = (x - mRECT.L) / mRECT.W();
    mPoints[mod.P].y = (y - mRECT.T) / mRECT.H();
    SetDirty();
  }

public:
  struct point
  {
    float x = 0.;
    float y = 0.;
    float radius = 1.;
    bool down = false;
    IColor color = IColor::GetRandomColor();
  };

  std::array<point, 8> mPoints;
};