//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("fm_settings.cpp", fmSettings);
USEFORMNS("dm_base.pas", Dm_base, DataModuleBase); /* TDataModule: File Type */
USEFORM("fm_katsoja.cpp", fmSurveyMain);
USEFORMNS("fm_database.pas", Fm_database, fmDatabase);
USEFORM("fm_hosp_rep_main.cpp", frmHospRepMain);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		HWND h = FindWindowA("TfmSurveyMain",0);//"fmSurveyMain");
		if (h!=0) {
		   //TMsgDlgButtons mb; mb<<mbOK;
		   //MessageDlg("alr",mtConfirmation,mb,0);
           //ShowWindow(h,1);
		   return 0;
		}

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfmSurveyMain), &fmSurveyMain);
		Application->CreateForm(__classid(TfmSettings), &fmSettings);
		Application->CreateForm(__classid(TfmDatabase), &fmDatabase);
		Application->CreateForm(__classid(TDataModuleBase), &DataModuleBase);
		Application->CreateForm(__classid(TfrmHospRepMain), &frmHospRepMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
