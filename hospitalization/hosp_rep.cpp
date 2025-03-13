//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORMNS("dm_base.pas", Dm_base, DataModuleBase); /* TDataModule: File Type */
USEFORMNS("fm_database.pas", Fm_database, fmDatabase);
USEFORM("fm_hosp_rep_main.cpp", frmHospRepMain);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfrmHospRepMain), &frmHospRepMain);
		Application->CreateForm(__classid(TDataModuleBase), &DataModuleBase);
		Application->CreateForm(__classid(TfmDatabase), &fmDatabase);
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
