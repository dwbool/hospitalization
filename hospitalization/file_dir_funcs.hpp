// CodeGear C++Builder
// Copyright (c) 1995, 2015 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'file_dir_funcs.pas' rev: 29.00 (Windows)

#ifndef File_dir_funcsHPP
#define File_dir_funcsHPP

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
#include <Vcl.FileCtrl.hpp>
#include <Winapi.ShellAPI.hpp>

//-- user supplied -----------------------------------------------------------

namespace File_dir_funcs
{
//-- forward type declarations -----------------------------------------------
//-- type declarations -------------------------------------------------------
//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE System::TDateTime __fastcall OrbitaToDateTime(System::UnicodeString s);
extern DELPHI_PACKAGE bool __fastcall FilesContentIsSame_FieldsCompare(System::UnicodeString file1, System::UnicodeString file2);
extern DELPHI_PACKAGE bool __fastcall FilesContentIsSame(System::UnicodeString file1, System::UnicodeString file2);
extern DELPHI_PACKAGE void __fastcall SearchInDir_Subdirs_file(System::UnicodeString sDir, System::UnicodeString fname, System::Classes::TStrings* ssFilesWithPath);
extern DELPHI_PACKAGE void __fastcall SearchInDir_Subdirs(System::UnicodeString s, System::UnicodeString mask, System::Classes::TStrings* ss);
extern DELPHI_PACKAGE void __fastcall SearchInDir_files_only(System::UnicodeString s, System::UnicodeString mask, System::Classes::TStrings* ss);
extern DELPHI_PACKAGE void __fastcall SearchInDir_directories_only(System::UnicodeString s, System::UnicodeString mask, System::Classes::TStrings* ss);
extern DELPHI_PACKAGE int __fastcall packFilesInDirectory_rar(System::UnicodeString sDir, System::UnicodeString sRarFileName);
}	/* namespace File_dir_funcs */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_FILE_DIR_FUNCS)
using namespace File_dir_funcs;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// File_dir_funcsHPP
