//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#include <dateutils.hpp>

#pragma hdrstop

#include "fm_settings.h"
#include "fm_katsoja.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSettings *fmSettings;
//---------------------------------------------------------------------------
__fastcall TfmSettings::TfmSettings(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmSettings::FormShow(TObject *Sender)
{
   String sDir1 = ini->ReadString("settings","incoming_dir","c:\\develop\\income\\");
   String sDir2 = ini->ReadString("settings","processed_dir","c:\\develop\\income\\processed\\");
   String sDir3 = ini->ReadString("settings","outcoming_dir","c:\\develop\\outcome\\");
   String sDirIncomHosp = ini->ReadString("settings","incoming_dir_hosp","c:\\develop\\income_hosp\\");
   edIncomingDir->Text=sDir1;
   edProcessedDir->Text=sDir2;
   edOutcomingDir->Text=sDir3;
   edIncomingDirHosp->Text=   sDirIncomHosp;

   chSurveyRun->Checked=  ini->ReadBool("settings","run_survey", 0);
   chHospRun->Checked=    ini->ReadBool("settings","run_hosp", 0);

   chReport->Checked= ini->ReadInteger("report","make", 1 );
   dpReport->Time= ini->ReadTime("report","time",TimeOf(dpReport->Time));
}
//---------------------------------------------------------------------------

void __fastcall TfmSettings::BitBtn1Click(TObject *Sender)
{
   //
   ini->WriteString("settings","incoming_dir", edIncomingDir->Text);
   ini->WriteString("settings","processed_dir", edProcessedDir->Text);
   ini->WriteString("settings","outcoming_dir", edOutcomingDir->Text);
   ini->WriteString("settings","incoming_dir_hosp", edIncomingDirHosp->Text);

   ini->WriteBool("settings","run_survey", chSurveyRun->Checked);
   ini->WriteBool("settings","run_hosp", chHospRun->Checked);

   ini->WriteTime("report","time",TimeOf(dpReport->Time));
   ini->WriteInteger("report","make",chReport->Checked );
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmSettings::SpeedButton1Click(TObject *Sender)
{
  String sdir;
  TSelectDirOpts opt;
  memset(&opt,0,sizeof(opt));
  opt<<sdAllowCreate;
  if (SelectDirectory(sdir,opt,0)) {
	  edIncomingDir->Text=sdir;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSettings::SpeedButton2Click(TObject *Sender)
{
  String sdir;
  TSelectDirOpts opt;
  memset(&opt,0,sizeof(opt));
  opt<<sdAllowCreate;
  if (SelectDirectory(sdir,opt,0)) {
	  edProcessedDir->Text=sdir;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSettings::BitBtn2Click(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmSettings::SpeedButton3Click(TObject *Sender)
{
    String sdir;
  TSelectDirOpts opt;
  memset(&opt,0,sizeof(opt));
  opt<<sdAllowCreate;
  if (SelectDirectory(sdir,opt,0)) {
	  edIncomingDirHosp->Text=sdir;
  }
}
//---------------------------------------------------------------------------


