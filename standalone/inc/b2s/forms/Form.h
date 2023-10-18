#pragma once

#include "../controls/Control.h"
#include "../../../common/Graphics.h"

class Form : public Control
{
public:
   Form();
   ~Form();

   void Show();
   void Hide();
   void SetTopMost(bool topMost) { m_topMost = topMost; }
   bool IsTopMost() const { return m_topMost; }

   void Render(VP::Graphics* pGraphics);

   SDL_Surface* GetBackgroundImage() const override { return m_pBackgroundImage; };
   void SetBackgroundImage(SDL_Surface* pBackgroundImage) override { if (m_pBackgroundImage != pBackgroundImage) { m_pBackgroundImage = pBackgroundImage; Invalidate(); } };

private:
   VP::Graphics* m_pGraphics;
   bool m_topMost;

   SDL_Surface* m_pBackgroundImage;
};
