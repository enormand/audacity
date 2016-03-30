/**********************************************************************

  Audacity: A Digital Audio Editor

  Dummy.cpp

  Erwan Normand

*******************************************************************//**

\class EffectDummy
\brief Short description.

  Description.

*//*******************************************************************/

#include "../Audacity.h"
#include "Dummy.h"

#include <math.h>
#include <float.h>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/log.h>

#include "../ShuttleGui.h"
#include "../WaveTrack.h"
#include "../widgets/valnum.h"


enum
{
   ID_Amp = 10000,
   ID_Peak,
   ID_Clip
};

// Define keys, defaults, minimums, and maximums for the effect parameters
//
//     Name       Type     Key                     Def         Min         Max            Scale
Param( Ratio,     float,   XO("Ratio"),            0.9f,       0.003162f,  316.227766f,   1.0f  );
Param( Amp,       float,   wxT(""),                -0.91515f,  -50.0f,     50.0f,         10.0f );

//
// EffectDummy
//

BEGIN_EVENT_TABLE(EffectDummy, wxEvtHandler)
   EVT_SLIDER(ID_Amp, EffectDummy::OnAmpSlider)
   EVT_TEXT(ID_Amp, EffectDummy::OnAmpText)
   EVT_TEXT(ID_Peak, EffectDummy::OnPeakText)
   EVT_CHECKBOX(ID_Clip, EffectDummy::OnClipCheckBox)
END_EVENT_TABLE()

EffectDummy::EffectDummy()
{
   mAmp = DEF_Amp;
   mRatio = DB_TO_LINEAR(mAmp);
   mRatioClip = 0.0;
   mCanClip = false;
   mPeak = 0.0;

   SetLinearEffectFlag(true);
}

EffectDummy::~EffectDummy()
{
}

// IdentInterface implementation

wxString EffectDummy::GetSymbol()
{
   return DUMMY_PLUGIN_SYMBOL;
}

wxString EffectDummy::GetDescription()
{
   return XO("Increases or decreases the volume of the audio you have selected");
}

// EffectIdentInterface implementation

EffectType EffectDummy::GetType()
{
   return EffectTypeProcess;
}

// EffectClientInterface implementation

int EffectDummy::GetAudioInCount()
{
   return 1;
}

int EffectDummy::GetAudioOutCount()
{
   return 1;
}

sampleCount EffectDummy::ProcessBlock(float **inBlock, float **outBlock, sampleCount blockLen)
{
   for (sampleCount i = 0; i < blockLen; i++)
   {
      outBlock[0][i] = inBlock[0][i] * mRatio;
   }

   return blockLen;
}

bool EffectDummy::GetAutomationParameters(EffectAutomationParameters & parms)
{
   parms.WriteFloat(KEY_Ratio, mRatio);

   return true;
}

bool EffectDummy::SetAutomationParameters(EffectAutomationParameters & parms)
{
   ReadAndVerifyFloat(Ratio);

   mRatio = Ratio;

   return true;
}

bool EffectDummy::LoadFactoryDefaults()
{
   Init();

   mRatioClip = 0.0;
   if (mPeak > 0.0)
   {
      mRatio = 1.0 / mPeak;
      mRatioClip = mRatio;
   }
   else
   {
      mRatio = 1.0;
   }
   mCanClip = false;

   return TransferDataToWindow();
}

// Effect implementation

bool EffectDummy::Init()
{
   mPeak = 0.0;

   SelectedTrackListOfKindIterator iter(Track::Wave, mTracks);

   for (Track *t = iter.First(); t; t = iter.Next())
   {
      float min, max;
      ((WaveTrack *)t)->GetMinMax(&min, &max, mT0, mT1);
      float newpeak = (fabs(min) > fabs(max) ? fabs(min) : fabs(max));

      if (newpeak > mPeak)
      {
         mPeak = newpeak;
      }
   }

   return true;
}

void EffectDummy::Preview(bool dryOnly)
{
   double ratio = mRatio;
   double peak = mPeak;

   Effect::Preview(dryOnly);

   mRatio = ratio;
   mPeak = peak;
}

void EffectDummy::PopulateOrExchange(ShuttleGui & S)
{
   if (IsBatchProcessing())
   {
      mPeak = 1.0;
   }
   else 
   {
      if (mPeak > 0.0)
      {
         mRatio = 1.0 / mPeak;
         mRatioClip = mRatio;
      }
      else
      {
         mRatio = 1.0;
      }
   }

   S.AddSpace(0, 5);

   S.StartVerticalLay(0);
   {
      int precission = 3; // allow (a generous) 3 decimal  places for Amplification (dB)
      // Amplitude
      S.StartMultiColumn(2, wxCENTER);
      {
         FloatingPointValidator<double> vldAmp(precission, &mAmp, NUM_VAL_ONE_TRAILING_ZERO);
         vldAmp.SetRange(MIN_Amp, MAX_Amp);
         mAmpT = S.Id(ID_Amp).AddTextBox(_("Amplification (dB):"), wxT(""), 12);
         mAmpT->SetValidator(vldAmp);
      }
      S.EndMultiColumn();

      // Amplitude
      S.StartHorizontalLay(wxEXPAND);
      {
         S.SetStyle(wxSL_HORIZONTAL);
         mAmpS = S.Id(ID_Amp).AddSlider(wxT(""), 0, MAX_Amp * SCL_Amp, MIN_Amp * SCL_Amp);
         mAmpS->SetName(_("Amplification dB"));
      }
      S.EndHorizontalLay();

      // Peak
      S.StartMultiColumn(2, wxCENTER);
      {
         // One extra decimal place so that rounding is visible to user (see: bug 958)
         FloatingPointValidator<double> vldNewPeak(precission + 1, &mNewPeak, NUM_VAL_ONE_TRAILING_ZERO);
         double minAmp = MIN_Amp + LINEAR_TO_DB(mPeak);
         double maxAmp = MAX_Amp + LINEAR_TO_DB(mPeak);

         // min and max need same precision as what we're validating (bug 963)
         minAmp = Internat::CompatibleToDouble(Internat::ToString(minAmp, precission));
         maxAmp = Internat::CompatibleToDouble(Internat::ToString(maxAmp, precission));

         vldNewPeak.SetRange(minAmp, maxAmp);
         mNewPeakT = S.Id(ID_Peak).AddTextBox(_("New Peak Amplitude (dB):"), wxT(""), 12);
         mNewPeakT->SetValidator(vldNewPeak);
      }
      S.EndMultiColumn();

      // Clipping
      S.StartHorizontalLay(wxCENTER);
      {
         mClip = S.Id(ID_Clip).AddCheckBox(_("Allow clipping"), wxT("false"));
         if (IsBatchProcessing())
         {
            mClip->Enable(false);
            mCanClip = true;
         }
      }
      S.EndHorizontalLay();
   }
   S.EndVerticalLay();

   return;
}

bool EffectDummy::TransferDataToWindow()
{
   // limit range of gain
   double dBInit = LINEAR_TO_DB(mRatio);
   double dB = TrapDouble(dBInit, MIN_Amp, MAX_Amp);
   if (dB != dBInit)
      mRatio = DB_TO_LINEAR(dB);

   mAmp = LINEAR_TO_DB(mRatio);
   mAmpT->GetValidator()->TransferToWindow();

   mAmpS->SetValue((int) (mAmp * SCL_Amp + 0.5f));

   mNewPeak = LINEAR_TO_DB(mRatio * mPeak);
   mNewPeakT->GetValidator()->TransferToWindow();

   mClip->SetValue(mCanClip);

   CheckClip();

   return true;
}

bool EffectDummy::TransferDataFromWindow()
{
   if (!mUIParent->Validate() || !mUIParent->TransferDataFromWindow())
   {
      return false;
   }

   mRatio = DB_TO_LINEAR(TrapDouble(mAmp * SCL_Amp, MIN_Amp * SCL_Amp, MAX_Amp * SCL_Amp) / SCL_Amp);

   mCanClip = mClip->GetValue();

   if (!mCanClip && mRatio * mPeak > 1.0)
   {
      mRatio = 1.0 / mPeak;
   }

   return true;
}

// EffectDummy implementation

void EffectDummy::CheckClip()
{
   EnableApply(mClip->GetValue() || (mPeak > 0.0 && mRatio <= mRatioClip));
}

void EffectDummy::OnAmpText(wxCommandEvent & WXUNUSED(evt))
{
   if (!mAmpT->GetValidator()->TransferFromWindow())
   {
      EnableApply(false);
      return;
   }

   mRatio = DB_TO_LINEAR(TrapDouble(mAmp * SCL_Amp, MIN_Amp * SCL_Amp, MAX_Amp * SCL_Amp) / SCL_Amp);

   mAmpS->SetValue((int) (LINEAR_TO_DB(mRatio) * SCL_Amp + 0.5));

   mNewPeak = LINEAR_TO_DB(mRatio * mPeak);
   mNewPeakT->GetValidator()->TransferToWindow();

   CheckClip();
}

void EffectDummy::OnPeakText(wxCommandEvent & WXUNUSED(evt))
{
   if (!mNewPeakT->GetValidator()->TransferFromWindow())
   {
      EnableApply(false);
      return;
   }

   if (mNewPeak == 0.0)
      mRatio = mRatioClip;
   else
      mRatio = DB_TO_LINEAR(mNewPeak) / mPeak;

   double ampInit = LINEAR_TO_DB(mRatio);
   mAmp = TrapDouble(ampInit, MIN_Amp, MAX_Amp);
   if (mAmp != ampInit)
      mRatio = DB_TO_LINEAR(mAmp);

   mAmpT->GetValidator()->TransferToWindow();

   mAmpS->SetValue((int) (mAmp * SCL_Amp + 0.5f));

   CheckClip();
}

void EffectDummy::OnAmpSlider(wxCommandEvent & evt)
{
   double dB = evt.GetInt() / SCL_Amp;
   mRatio = DB_TO_LINEAR(TrapDouble(dB, MIN_Amp, MAX_Amp));

   double dB2 = (evt.GetInt() - 1) / SCL_Amp;
   double ratio2 = DB_TO_LINEAR(TrapDouble(dB2, MIN_Amp, MAX_Amp));

   if (!mClip->GetValue() && mRatio * mPeak > 1.0 && ratio2 * mPeak < 1.0)
   {
      mRatio = 1.0 / mPeak;
   }

   mAmp = LINEAR_TO_DB(mRatio);
   mAmpT->GetValidator()->TransferToWindow();

   mNewPeak = LINEAR_TO_DB(mRatio * mPeak);
   mNewPeakT->GetValidator()->TransferToWindow();

   CheckClip();
}

void EffectDummy::OnClipCheckBox(wxCommandEvent & WXUNUSED(evt))
{
   CheckClip();
}
