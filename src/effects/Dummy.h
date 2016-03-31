/**********************************************************************

  Audacity: A Digital Audio Editor

  Dummy.h

  Erwan Normand

  Alter the sampling of the track.

**********************************************************************/

#ifndef __AUDACITY_EFFECT_DUMMY__
#define __AUDACITY_EFFECT_DUMMY__

#include <wx/event.h>
#include <wx/slider.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "Effect.h"

#define DUMMY_PLUGIN_SYMBOL XO("Dummy")

class ShuttleGui;

class EffectDummy final : public Effect
{
public:
   EffectDummy();
   virtual ~EffectDummy();

   // IdentInterface implementation

   wxString GetSymbol() override;
   wxString GetDescription() override;

   // EffectIdentInterface implementation

   EffectType GetType() override;

   // EffectClientInterface implementation

   int GetAudioInCount() override;
   int GetAudioOutCount() override;
   sampleCount ProcessBlock(float **inBlock, float **outBlock, sampleCount blockLen) override;
   bool GetAutomationParameters(EffectAutomationParameters & parms) override;
   bool SetAutomationParameters(EffectAutomationParameters & parms) override;

   // Effect implementation

   bool Init() override;
   void Preview(bool dryOnly) override;
   void PopulateOrExchange(ShuttleGui & S) override;
   bool TransferDataToWindow() override;
   bool TransferDataFromWindow() override;

private:
   // EffectDummy implementation

   void OnSamplingPointsText(wxCommandEvent & evt);
   void OnSamplingPointsSlider(wxCommandEvent & evt);
   void OnSamplingPoints();

private:
   int mSamplingPoints;
   int mModesIndex;
   
   wxArrayString mModes;

   wxTextCtrl *mSamplingPointsT;
   wxSlider *mSamplingPointsS;
   wxChoice *mModesC;

   DECLARE_EVENT_TABLE();
};

#endif // __AUDACITY_EFFECT_DUMMY__
