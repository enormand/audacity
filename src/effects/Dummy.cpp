/**********************************************************************

  Audacity: A Digital Audio Editor

  Dummy.cpp

  Erwan Normand

*******************************************************************//**

\class EffectDummy
\brief Alter the sampling of the track.

*//*******************************************************************/

#include "../Audacity.h"
#include "Dummy.h"

#include <wx/choice.h>
#include <wx/valgen.h>

#include "../ShuttleGui.h"
#include "../widgets/valnum.h"

enum
{
   ID_SamplingPoints = 10000
};

enum kModes
{
   kAverage,
   kFirst,
   kNumModes
};

static const wxChar *kModeStrings[kNumModes] =
{
   XO("Average of the points"),
   XO("First point value")
};

// Define keys, defaults, minimums, and maximums for the effect parameters
//
//     Name             Type     Key                     Def      Min   Max            Scale
Param( SamplingPoints,  int,     XO("Sampling Points"),  1,       1,    1000,          1 );
Param( Modes,           int,     XO("Modes"),            kAverage,  0,    kNumModes - 1, 1 );

//
// EffectDummy
//

BEGIN_EVENT_TABLE(EffectDummy, wxEvtHandler)
   EVT_TEXT(ID_SamplingPoints, EffectDummy::OnSamplingPointsText)
   EVT_SLIDER(ID_SamplingPoints, EffectDummy::OnSamplingPointsSlider)
END_EVENT_TABLE()

EffectDummy::EffectDummy()
{
   mSamplingPoints = DEF_SamplingPoints;
   mModesIndex = DEF_Modes;

   for (int i = 0; i < kNumModes; i++)
   {
      mModes.Add(wxGetTranslation(kModeStrings[i]));
   }

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
   return XO("Alter the sampling of the track.");
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
   for (sampleCount subBlock = 0; subBlock < blockLen; subBlock += mSamplingPoints)
   {
      sampleCount subBlockLen = subBlock + mSamplingPoints;

      if (subBlockLen > blockLen) // Remaining less points than necessary
      {
         for (sampleCount i = subBlock; i < blockLen; i++)
         {
            outBlock[0][i] = inBlock[0][i];
         }
      }
      else if (mModesIndex == kFirst)
      {
         for (sampleCount i = subBlock; i <= subBlockLen; i++)
         {
            outBlock[0][i] = inBlock[0][subBlock];
         }
      }
      else if (mModesIndex == kAverage)
      {
         float newValue = 0.0f;
         for (sampleCount i = subBlock; i < subBlockLen; i++)
         {
            newValue += inBlock[0][i];
         }
         newValue /= mSamplingPoints;

         for (sampleCount i = subBlock; i < subBlockLen; i++)
         {
            outBlock[0][i] = newValue;
         }
      }
   }

   return blockLen;
}

bool EffectDummy::GetAutomationParameters(EffectAutomationParameters & parms)
{
   parms.Write(KEY_SamplingPoints, mSamplingPoints);
   parms.Write(KEY_Modes, kModeStrings[mModesIndex]);

   return true;
}

bool EffectDummy::SetAutomationParameters(EffectAutomationParameters & parms)
{
   ReadAndVerifyInt(SamplingPoints);
   ReadAndVerifyEnum(Modes, mModes);

   mSamplingPoints = SamplingPoints;

   return true;
}

// Effect implementation

bool EffectDummy::Init()
{
   return true;
}

void EffectDummy::Preview(bool dryOnly)
{
   int samplingPoints = mSamplingPoints;

   Effect::Preview(dryOnly);

   mSamplingPoints = samplingPoints;
}

void EffectDummy::PopulateOrExchange(ShuttleGui & S)
{
   wxASSERT(kNumModes == WXSIZEOF(kModeStrings));

   S.AddSpace(0, 5);

   S.StartVerticalLay();
   {
      // Sampling points text
      S.StartMultiColumn(2, wxCENTER);
      {
         IntegerValidator<int> vldSamplingPoints(&mSamplingPoints);
         vldSamplingPoints.SetRange(MIN_SamplingPoints, MAX_SamplingPoints);
         mSamplingPointsT = S.Id(ID_SamplingPoints).AddTextBox(_("Number of sampling points to merge:"), wxT(""), 10);
         mSamplingPointsT->SetValidator(vldSamplingPoints);
      }
      S.EndMultiColumn();

      // Sampling points slider
      S.StartHorizontalLay(wxEXPAND);
      {
         S.SetStyle(wxSL_HORIZONTAL);
         mSamplingPointsS = S.Id(ID_SamplingPoints).AddSlider(wxT(""), mSamplingPoints, MAX_SamplingPoints, MIN_SamplingPoints);
         mSamplingPointsS->SetName(_("Sampling Points"));
         mSamplingPointsS->SetValidator(wxGenericValidator(&mSamplingPoints));
      }
      S.EndHorizontalLay();

      // Mode
      S.StartMultiColumn(2, wxCENTER);
      {
         mModesC = S.AddChoice(_("Point value mode:"), wxT(""), &mModes);
         mModesC->SetValidator(wxGenericValidator(&mModesIndex));
      }
      S.EndMultiColumn();
   }
   S.EndVerticalLay();

   return;
}

bool EffectDummy::TransferDataToWindow()
{
   if (!mUIParent->TransferDataToWindow())
   {
      return false;
   }

   return true;
}

bool EffectDummy::TransferDataFromWindow()
{
   if (!mUIParent->Validate() || !mUIParent->TransferDataFromWindow())
   {
      return false;
   }

   return true;
}

// EffectDummy implementation

void EffectDummy::OnSamplingPointsText(wxCommandEvent & WXUNUSED(evt))
{
   mSamplingPointsT->GetValidator()->TransferFromWindow();
   mSamplingPointsS->GetValidator()->TransferToWindow();
}

void EffectDummy::OnSamplingPointsSlider(wxCommandEvent & WXUNUSED(evt))
{
   mSamplingPointsS->GetValidator()->TransferFromWindow();
   mSamplingPointsT->GetValidator()->TransferToWindow();
}
