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

#include "../ShuttleGui.h"
#include "../widgets/valnum.h"

enum
{
   ID_SamplingPoints
};

// Define keys, defaults, minimums, and maximums for the effect parameters
//
//     Name             Type     Key                     Def   Min   Max      Scale
Param( SamplingPoints,  float,   XO("Sampling Points"),  1,    1,    1000,    1);

//
// EffectDummy
//

BEGIN_EVENT_TABLE(EffectDummy, wxEvtHandler)
   EVT_TEXT(ID_SamplingPoints, EffectDummy::OnSamplingPointsText)
END_EVENT_TABLE()

EffectDummy::EffectDummy()
{
   mSamplingPoints = 1;

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
      float meanValue = 0.0f;

      for (sampleCount i = subBlock; i < subBlockLen; i++)
      {
         meanValue += inBlock[0][i];
      }
      meanValue /= mSamplingPoints;

     for (sampleCount i = subBlock; i < subBlockLen; i++)
      {
         outBlock[0][i] = meanValue;
      }
   }

   return blockLen;
}

bool EffectDummy::GetAutomationParameters(EffectAutomationParameters & parms)
{
   parms.Write(KEY_SamplingPoints, mSamplingPoints);

   return true;
}

bool EffectDummy::SetAutomationParameters(EffectAutomationParameters & parms)
{
   ReadAndVerifyInt(SamplingPoints);

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
   S.AddSpace(0, 5);

   S.StartVerticalLay(0);
   {
      // Sampling points
      S.StartMultiColumn(2, wxCENTER);
      {
         IntegerValidator<int> vldSamplingPoints(&mSamplingPoints);
         vldSamplingPoints.SetRange(MIN_SamplingPoints, MAX_SamplingPoints);
         mSamplingPointsT = S.Id(ID_SamplingPoints).AddTextBox(_("Number of sampling points to merge:"), wxT(""), 10);
         mSamplingPointsT->SetValidator(vldSamplingPoints);
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
}
