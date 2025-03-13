// CodeGear C++Builder
// Copyright (c) 1995, 2015 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'ats_utils.pas' rev: 29.00 (Windows)

#ifndef Ats_utilsHPP
#define Ats_utilsHPP

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
#include <Vcl.FileCtrl.hpp>
#include <System.Win.ComObj.hpp>

//-- user supplied -----------------------------------------------------------

namespace Ats_utils
{
//-- forward type declarations -----------------------------------------------
struct TCallInfo;
//-- type declarations -------------------------------------------------------
typedef TCallInfo *PCallInfo;

struct DECLSPEC_DRECORD TCallInfo
{
public:
	System::TDateTime dtConnect;
	System::TDateTime dtDisconnect;
	System::UnicodeString callID;
	System::UnicodeString OwnerID;
	System::UnicodeString DialledPhone;
};


//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE int ColTime;
extern DELPHI_PACKAGE int ColEvent;
extern DELPHI_PACKAGE int ColDialed;
extern DELPHI_PACKAGE int ColCallID;
extern DELPHI_PACKAGE int ColOwnerID;
extern DELPHI_PACKAGE void __fastcall DestroyCallsInfo(System::Classes::TStringList* ssCalls);
extern DELPHI_PACKAGE System::TDateTime __fastcall FindFileDateTimeAndSize(System::UnicodeString fname, int &size);
extern DELPHI_PACKAGE System::TDateTime __fastcall FindFileDateTime(System::UnicodeString fname);
extern DELPHI_PACKAGE int __fastcall ReadCallsLaterThan(System::UnicodeString dir, System::TDateTime dtLastDisconnect, int iFileLast, System::Classes::TStringList* ssCalls);
extern DELPHI_PACKAGE void __fastcall CallsToFiles(System::Classes::TStringList* ssCalls, System::UnicodeString sDir, int iFileNameStart, System::TDateTime dtLastDisconnect);
extern DELPHI_PACKAGE System::Variant __fastcall OpenSheet(System::UnicodeString fname, System::Variant &ExcelFile);
extern DELPHI_PACKAGE System::Variant __fastcall CloseSheet(const System::Variant &ExcelFile);
extern DELPHI_PACKAGE int __fastcall ScanSheet(System::Classes::TStringList* ssCalls, const System::Variant &Sheet, System::TDateTime dtLastDisconnect);
extern DELPHI_PACKAGE int __fastcall SortDisconnectTimeAscend(System::Classes::TStrings* ss);
extern DELPHI_PACKAGE int __fastcall SortFileNameAscend(System::Classes::TStrings* ss);
extern DELPHI_PACKAGE System::TDateTime __fastcall LastFileDisconnectTime(System::UnicodeString dir);
extern DELPHI_PACKAGE System::TDateTime __fastcall FileDisconnectTime(System::UnicodeString sfile);
extern DELPHI_PACKAGE System::UnicodeString __fastcall DirPlusName(System::UnicodeString dir, System::UnicodeString name);
extern DELPHI_PACKAGE int __fastcall LastFileCallInfo(System::UnicodeString dir, TCallInfo &ci);
extern DELPHI_PACKAGE int __fastcall FileCallInfo(System::UnicodeString fname, TCallInfo &ci);
extern DELPHI_PACKAGE System::UnicodeString __fastcall IntFileNameToString(unsigned iname);
extern DELPHI_PACKAGE System::UnicodeString __fastcall LastFileNameAsString(System::UnicodeString dir);
extern DELPHI_PACKAGE System::UnicodeString __fastcall IntToFileName(int iname);
extern DELPHI_PACKAGE unsigned __fastcall FileNameToInt(System::UnicodeString name);
extern DELPHI_PACKAGE int __fastcall LastFileNameAsInt(System::UnicodeString dir);
extern DELPHI_PACKAGE System::UnicodeString __fastcall DateToATS(System::TDateTime dt);
extern DELPHI_PACKAGE System::UnicodeString __fastcall TimeToATS(System::TDateTime dt);
extern DELPHI_PACKAGE System::UnicodeString __fastcall CallsToText(System::Classes::TStrings* ss, System::UnicodeString LineDelimiter);
extern DELPHI_PACKAGE int __fastcall LogTextToCallInfo(System::UnicodeString S, TCallInfo &ci);
extern DELPHI_PACKAGE System::UnicodeString __fastcall CallInfoToText(PCallInfo ci);
extern DELPHI_PACKAGE System::UnicodeString __fastcall TrimAllUnneccessary(System::UnicodeString s);
extern DELPHI_PACKAGE System::TDateTime __fastcall ATStimeToDelphi(System::UnicodeString s);
}	/* namespace Ats_utils */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_ATS_UTILS)
using namespace Ats_utils;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Ats_utilsHPP
