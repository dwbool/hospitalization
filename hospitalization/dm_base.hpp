// CodeGear C++Builder
// Copyright (c) 1995, 2015 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'dm_base.pas' rev: 29.00 (Windows)

#ifndef Dm_baseHPP
#define Dm_baseHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>

//-- user supplied -----------------------------------------------------------

namespace Dm_base
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TDataModuleBase;
//-- type declarations -------------------------------------------------------
class PASCALIMPLEMENTATION TDataModuleBase : public System::Classes::TDataModule
{
	typedef System::Classes::TDataModule inherited;
	
__published:
	Data::Win::Adodb::TADOConnection* ADOConnection;
public:
	/* TDataModule.Create */ inline __fastcall virtual TDataModuleBase(System::Classes::TComponent* AOwner) : System::Classes::TDataModule(AOwner) { }
	/* TDataModule.CreateNew */ inline __fastcall virtual TDataModuleBase(System::Classes::TComponent* AOwner, int Dummy) : System::Classes::TDataModule(AOwner, Dummy) { }
	/* TDataModule.Destroy */ inline __fastcall virtual ~TDataModuleBase(void) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE TDataModuleBase* DataModuleBase;
}	/* namespace Dm_base */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_DM_BASE)
using namespace Dm_base;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Dm_baseHPP
