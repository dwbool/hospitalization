// CodeGear C++Builder
// Copyright (c) 1995, 2015 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'fm_database.pas' rev: 29.00 (Windows)

#ifndef Fm_databaseHPP
#define Fm_databaseHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <Winapi.Windows.hpp>
#include <Winapi.Messages.hpp>
#include <System.SysUtils.hpp>
#include <System.Variants.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.StdCtrls.hpp>
#include <System.IniFiles.hpp>
#include <dm_base.hpp>
#include <Vcl.Buttons.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>

//-- user supplied -----------------------------------------------------------

namespace Fm_database
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TfmDatabase;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TfmDatabase : public Vcl::Forms::TForm
{
	typedef Vcl::Forms::TForm inherited;
	
__published:
	Vcl::Stdctrls::TLabel* Label1;
	Vcl::Stdctrls::TComboBox* edConn;
	Vcl::Stdctrls::TButton* btConnect;
	Vcl::Stdctrls::TButton* btDisconnect;
	Vcl::Stdctrls::TLabel* lbStatus;
	Vcl::Stdctrls::TButton* btExit;
	Vcl::Buttons::TSpeedButton* btBuild;
	Vcl::Stdctrls::TLabel* Label2;
	Vcl::Buttons::TSpeedButton* btRecreateTables;
	Data::Win::Adodb::TADOQuery* qCreate;
	Data::Db::TAutoIncField* qCreateID;
	Data::Db::TWideStringField* qCreateNumber;
	Data::Db::TWideStringField* qCreatename;
	Data::Db::TBCDField* qCreateSumm;
	Data::Db::TDateTimeField* qCreateCreatedDate;
	Data::Db::TAutoIncField* qCreateUser_ID;
	Vcl::Stdctrls::TLabel* Label3;
	void __fastcall btConnectClick(System::TObject* Sender);
	void __fastcall FormCreate(System::TObject* Sender);
	void __fastcall btDisconnectClick(System::TObject* Sender);
	void __fastcall FormShow(System::TObject* Sender);
	void __fastcall btUpdateControlsClick(System::TObject* Sender);
	void __fastcall btExitClick(System::TObject* Sender);
	void __fastcall btBuildClick(System::TObject* Sender);
	void __fastcall btRecreateTablesClick(System::TObject* Sender);
	
public:
	bool editAllowed;
public:
	/* TCustomForm.Create */ inline __fastcall virtual TfmDatabase(System::Classes::TComponent* AOwner) : Vcl::Forms::TForm(AOwner) { }
	/* TCustomForm.CreateNew */ inline __fastcall virtual TfmDatabase(System::Classes::TComponent* AOwner, int Dummy) : Vcl::Forms::TForm(AOwner, Dummy) { }
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TfmDatabase(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TfmDatabase(HWND ParentWindow) : Vcl::Forms::TForm(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE TfmDatabase* fmDatabase;
extern DELPHI_PACKAGE System::Inifiles::TIniFile* ini;
}	/* namespace Fm_database */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_FM_DATABASE)
using namespace Fm_database;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Fm_databaseHPP
