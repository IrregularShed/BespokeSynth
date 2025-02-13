/**
    bespoke synth, a software modular synthesizer
    Copyright (C) 2021 Ryan Challinor (contact: awwbees@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
/*
  ==============================================================================

    QuickSpawnMenu.cpp
    Created: 22 Oct 2017 7:49:17pm
    Author:  Ryan Challinor

  ==============================================================================
*/

#include "QuickSpawnMenu.h"
#include "ModularSynth.h"
#include "ModuleFactory.h"
#include "TitleBar.h"

QuickSpawnMenu* TheQuickSpawnMenu = nullptr;

namespace
{
   const int itemSpacing = 15;
   ofVec2f moduleGrabOffset(-40,10);
}

QuickSpawnMenu::QuickSpawnMenu()
: mLastHoverX(0)
, mLastHoverY(0)
{
   assert(TheQuickSpawnMenu == nullptr);
   TheQuickSpawnMenu = this;
}

QuickSpawnMenu::~QuickSpawnMenu()
{
   assert(TheQuickSpawnMenu == this);
   TheQuickSpawnMenu = nullptr;
}

void QuickSpawnMenu::Init()
{
   IDrawableModule::Init();
   SetShouldDrawOutline(false);
   SetShowing(false);
}

void QuickSpawnMenu::KeyPressed(int key, bool isRepeat)
{
   IDrawableModule::KeyPressed(key, isRepeat);
   if (key < CHAR_MAX && ((key >= 'a' && key <= 'z') || key == ';') && !isRepeat && GetKeyModifiers() == kModifier_None)
   {
      mElements = TheSynth->GetModuleFactory()->GetSpawnableModules((char)key);
      if (mElements.size() > 0)
      {
         mCurrentMenuChar = (char)key;

         float width = 150;
         for (auto element : mElements)
         {
            float elementWidth = GetStringWidth(element) + 10;
            if (elementWidth > width)
               width = elementWidth;
         }
         
         SetDimensions(width, (int)mElements.size() * itemSpacing);
         float minX = 5;
         float maxX = ofGetWidth() / GetOwningContainer()->GetDrawScale() - mWidth- 5;
         float minY = TheTitleBar->GetRect().height + 5;
         float maxY = ofGetHeight() / GetOwningContainer()->GetDrawScale() - mHeight - 5;
         SetPosition(ofClamp(TheSynth->GetMouseX(GetOwningContainer()) - mWidth / 2, minX, maxX),
                     ofClamp(TheSynth->GetMouseY(GetOwningContainer()) - mHeight / 2, minY, maxY));
         SetShowing(true);
      }
   }
}

void QuickSpawnMenu::KeyReleased(int key)
{
   if (key == mCurrentMenuChar)
   {
      SetShowing(false);
   }
}

void QuickSpawnMenu::MouseReleased()
{
   if (IsShowing())
   {
      SetShowing(false);
   }
}

void QuickSpawnMenu::DrawModule()
{
   ofPushStyle();
   
   int highlightIndex = -1;
   if (TheSynth->GetMouseY(GetOwningContainer()) > GetPosition().y)
      highlightIndex = (TheSynth->GetMouseY(GetOwningContainer()) - GetPosition().y)/itemSpacing;
   
   ofSetColor(50,50,50,100);
   ofFill();
   ofRect(-2,-2,mWidth+4,mHeight+4);
   for (int i=0; i<mElements.size(); ++i)
   {
      ofSetColor(IDrawableModule::GetColor(TheSynth->GetModuleFactory()->GetModuleType(mElements[i]))*(i==highlightIndex ? .7f : .5f), 255);
      ofRect(0, i*itemSpacing+1, mWidth, itemSpacing-1);
      if (i == highlightIndex)
         ofSetColor(255, 255, 0);
      else
         ofSetColor(255,255,255);
      DrawTextNormal(mElements[i], 1, i*itemSpacing+12);
   }
   
   ofPopStyle();
}

bool QuickSpawnMenu::MouseMoved(float x, float y)
{
   mLastHoverX = x;
   mLastHoverY = y;
   return false;
}

void QuickSpawnMenu::OnClicked(int x, int y, bool right)
{
   if (right)
      return;
   
   std::string moduleTypeName = GetModuleTypeNameAt(x, y);
   if (moduleTypeName != "")
   {
      IDrawableModule* module = TheSynth->SpawnModuleOnTheFly(moduleTypeName, TheSynth->GetMouseX(GetOwningContainer()) + moduleGrabOffset.x, TheSynth->GetMouseY(GetOwningContainer()) + moduleGrabOffset.y);
      TheSynth->SetMoveModule(module, moduleGrabOffset.x, moduleGrabOffset.y);
   }
   
   SetShowing(false);
}

std::string QuickSpawnMenu::GetHoveredModuleTypeName()
{
   return GetModuleTypeNameAt(mLastHoverX, mLastHoverY);
}

std::string QuickSpawnMenu::GetModuleTypeNameAt(int x, int y)
{
   int index = y / itemSpacing;
   if (index >= 0 && index < mElements.size())
      return mElements[index];

   return "";
}
