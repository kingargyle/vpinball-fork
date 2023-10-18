#pragma once

#include "Graphics.h"

namespace VP {

class Window
{
public:
   Window(const string& name, int x, int y, int w, int h, int frameSkip = 0);
   ~Window();

   void Enable(bool enable);
   bool ShouldRender();

   VP::Graphics* GetGraphics() { return m_pGraphics; }

private:
   SDL_Window* m_pWindow;
   SDL_Renderer* m_pRenderer;
   bool m_enabled;
   int m_frameSkip;
   int m_currentFrame;
   VP::Graphics* m_pGraphics;
};

}
