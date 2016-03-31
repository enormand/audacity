/**********************************************************************

  Audacity: A Digital Audio Editor

  Dummy.h

  Erwan Normand

  Description.

**********************************************************************/

#ifndef __AUDACITY_EFFECT_DUMMY__
#define __AUDACITY_EFFECT_DUMMY__

#include <wx/checkbox.h>
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
   bool LoadFactoryDefaults() override;

   // Effect implementation

   bool Init() override;
   void Preview(bool dryOnly) override;
   void PopulateOrExchange(ShuttleGui & S) override;
   bool TransferDataToWindow() override;
   bool TransferDataFromWindow() override;

private:
   // EffectDummy implementation

   void OnAmpText(wxCommandEvent & evt);
   void OnPeakText(wxCommandEvent & evt);
   void OnAmpSlider(wxCommandEvent & evt);
   void OnClipCheckBox(wxCommandEvent & evt);
   void OnSamplingPointsText(wxCommandEvent & evt);
   void CheckClip();

private:
   int mSamplingPoints;

   double mPeak;

   double mRatio;
   double mRatioClip;   // maximum value of mRatio which does not cause clipping
   double mAmp;
   double mNewPeak;
   bool mCanClip;

   wxSlider *mAmpS;
   wxTextCtrl *mAmpT;
   wxTextCtrl *mNewPeakT;
   wxCheckBox *mClip;
   wxTextCtrl *mSamplingPointsT;

   DECLARE_EVENT_TABLE();
};

#endif // __AUDACITY_EFFECT_DUMMY__
