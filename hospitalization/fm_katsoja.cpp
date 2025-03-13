//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#include <XMLDoc.hpp>
#include <XMLIntf.hpp>
#include <ComObj.hpp>
#include <dateutils.hpp>
#include <stdio.h>
#include <vector>
#include <inifiles.hpp>

#pragma hdrstop

#include "fm_katsoja.h"
#include "fm_settings.h"
#include "fm_hosp_rep_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSurveyMain *fmSurveyMain;
TDirectoryObserver  obs;
TDirectoryObserver  obsHosp;
TIniFile * iniSet;
String sDirProcessed="c:\\develop\\income\\processed\\";
String sDirOutcome = "c:\\develop\\outcome\\";
//---------------------------------------------------------------------------
__fastcall TfmSurveyMain::TfmSurveyMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------



int findIDphysicalLast(TADOQuery * q)
{
  fmSurveyMain->m->Lines->Add("Line 30");
  String sql= "select max(ID_physical) as imax from persons";
  q->Close();
  fmSurveyMain->m->Lines->Add("Line 31");
  q->SQL->Text=sql;
  fmSurveyMain->m->Lines->Add("Line 32");
  q->Open();
  fmSurveyMain->m->Lines->Add("Line 33");
  //int imax= q->FieldValues["imax"];       //@@error 2020may15

  int imax =
  //String s=(q->FieldList[0].Values[0]);
  q->Fields->Fields[0]->AsInteger;
  //q->FieldValues[0];
  //q->FieldList->Values[0];
  //Fields->FieldByNumber(0)->Value;

  fmSurveyMain->m->Lines->Add("Line 34");
  return imax;
}
   /*
int scopeIdentity(TADOQuery * q)
{
  String sql= "select scope_identity() as sc";
  q->Close();
  q->SQL->Text=sql;
  q->Open();
  int x= q->FieldValues["sc"];
  return x;
}
	 */

int compare_file_added(TStrings *ssOld, TStrings *ssNew)
{
	//
	for (int i = 0; i < ssNew->Count; i++) {
		String s= ssNew->Strings[i];
		int ntx = ssOld->IndexOf(s);
		if (ntx==-1) {
			//new file added
			return i;
		}
	}
	return -1;

}

 TStringList *ss=0;
 TStringList* ssNew=0;
 String sDirScan="";

void __fastcall TfmSurveyMain::Button1Click(TObject *Sender)
{
  //TStringList *
  /*
  ssNew=new TStringList;
  ss=new TStringList;
  sDirScan=   "h:\\develop\\hist_tester\\DATA\\2011-06-01\\10-50";
  SearchInDir_files_only(sDirScan,"*.*",  ss);
  TTreeNode *ndMain = tv->Items->AddChild(0,"main");
  for (int i = 0; i <ss->Count ; i++) {
	TSearchRec * r = (TSearchRec * )ss->Objects[i];
	m->Lines->Add(ss->Strings[i]+" : "+ IntToStr(  r->Size));

	tv->Items->AddChildObject(ndMain,ss->Strings[i],0);
	//FILETIME ft =          r->FindData.ftCreationTime;
	//int fti;
	//memcpy(&fti,&ft,4);
	//fti=&ft;

//	m->Lines->Add(DateTimeToStr(FileTimeToSystemTime()((int)(fti))));
//	FileDateToDateTime()
  }
    */
//  dout->Drive='H';
// dout->Directory="H:\\develop\\hist_tester\\DATA\\";

// SearchInDir_Subdirs_file("H:\\develop\\hist_tester\\DATA\\","*.*",  m->Lines);
 //H:\develop\hist_tester\DATA\2011-06-01\10-50\

}
//---------------------------------------------------------------------------
void __fastcall TfmSurveyMain::Button2Click(TObject *Sender)
{
  TStringList *ss=new TStringList;
  SearchInDir_directories_only("H:\\develop\\hist_tester\\DATA\\2011-06-01\\","*.*",  ss);
  for (int i = 0; i <ss->Count ; i++) {
	TSearchRec * r = (TSearchRec * )ss->Objects[i];
	m->Lines->Add(r->Name+" : "+ IntToStr(  r->Size));
  }

}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::Button3Click(TObject *Sender)
{
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile("med_examin4.xml");

  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  m->Lines->Add(cnt);
  for (int i = 0; i < cnt; i++) {

	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);
	 //String s= ndChild->XML;
	 //ndChild->GetText(); //error
	 m->Lines->Add(ndChild->NodeName+" >>>>>>>>>>>>");

	 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
		IXMLNode *ndChild2 =	 ndChild-> ChildNodes->Get(k);
		String s= ndChild2->XML;
		m->Lines->Add(ndChild2->Text);

	 }


  }
  TSurveyFileInfo fi;
  std::vector<TPersonInfo> pv;
  process_survey_xml("med_examin4.xml",fi,pv, m->Lines);
}
//---------------------------------------------------------------------------


void add_slash(String & s)
{
  s=s.Trim();
  if (s.Length()>0) {
   if (s[s.Length()]!='\\') {
	 s+='\\';
   }
  }
}


bool just_started=true;

void __fastcall TfmSurveyMain::Timer1Timer(TObject *Sender)
{
 //
 bool timer1_enabled_=Timer1->Enabled;
 Timer1->Enabled=false;

 try
 {
 if (just_started) {
	just_started=0;
    
 }

 while (lbxFiles->Count>100)
 {
   lbxFiles->Items->Delete(0);
 }
 while (m->Lines->Count>500){
   m->Lines->Delete(0);
 }

 lbRun->Caption=lbRun->Caption+">";
 if (lbRun->Caption.Length()>=4) {
     lbRun->Caption="";
 }

/* if (ss && ssNew) {

	 SearchInDir_files_only(sDirScan,"*.*",  ssNew);
	 int ntx = compare_file_added(ss,ssNew);
	 if (ntx!=-1) {

		 String sNew = ssNew->Strings[ntx];
		 m->Lines->Add("NEW FILE ADDED: "+sNew);
	 }
	 ss->Clear();
	 ss->AddStrings(ssNew);
 }
  */
 obs.directory=iniSet->ReadString("settings","incoming_dir","c:\\develop\\income\\");
 obsHosp.directory=iniSet->ReadString("settings","incoming_dir_hosp","c:\\develop\\income_hosp\\");

 sDirProcessed = iniSet->ReadString("settings","processed_dir","c:\\develop\\income\\processed\\");
 sDirOutcome = iniSet->ReadString("settings","outcoming_dir","c:\\develop\\outcome\\");


// try
// {
  add_slash(obs.directory);
  add_slash(sDirProcessed);
  add_slash(sDirOutcome);

  if (obs.directory!=iniSet->ReadString("settings","incoming_dir","")) {
   iniSet->WriteString("settings","incoming_dir", obs.directory);
  }
  if (obsHosp.directory!=iniSet->ReadString("settings","incoming_dir_hosp","")) {
   iniSet->WriteString("settings","incoming_dir", obsHosp.directory);
  }
  if (sDirProcessed!=iniSet->ReadString("settings","processed_dir","c:\\develop\\income\\processed\\")) {
   iniSet->WriteString("settings","processed_dir", sDirProcessed);
  }
  if (sDirOutcome!=iniSet->ReadString("settings","outcoming_dir","c:\\develop\\outcome\\")) {
   iniSet->WriteString("settings","outcoming_dir", sDirOutcome);
  }

  if (!DirectoryExists(obs.directory)) {
	if (obs.directory.Trim().Length()>0) {
	 ForceDirectories(   obs.directory);
	}
  }
  if (!DirectoryExists(obsHosp.directory)) {
	if (obsHosp.directory.Trim().Length()>0) {
	 ForceDirectories(   obsHosp.directory);
	}
  }
  if (!DirectoryExists(sDirProcessed)) {
	if ( sDirProcessed.Trim().Length()>0 ) {
	 ForceDirectories(  sDirProcessed);
	}
  }
  if (!DirectoryExists(sDirOutcome)) {
	if (sDirOutcome.Trim().Length()>0) {
	 ForceDirectories(  sDirOutcome);
	}
  }
  obs.step();
  obsHosp.step();

 } catch(Exception &e){
     StatusBar1->SimpleText=e.Message;
 };

 //schedule
 TTime dtReport =fmSettings->dpReport->Time;
 //ini->ReadTime("report","time",0);
 int makeRep = fmSettings->chReport->Checked;
 //ini->ReadTime("report","make",0);
 static int reportRunning = 0;
 if  (SecondsBetween(TimeOf(Now()),TimeOf(dtReport))<60 && TimeOf(Now())>TimeOf(dtReport) )
 {
	 //
	 //TDateTime dtLast
	 if (!reportRunning)
	 {
		reportRunning = 1;
		//winExec
		m->Lines->Add("run report "+DateTimeToStr(Now()));
		frmHospRepMain->Show();
		int day_ind= iniSet->ReadInteger("report","days",1);
		day_ind--;
		if (day_ind>=frmHospRepMain->cbDay->Items->Count) {
		   day_ind=frmHospRepMain->cbDay->Items->Count-1;
		}
		if (day_ind<0) {
          day_ind=0;
		}
			frmHospRepMain->cbDay->ItemIndex=day_ind;
		//frmHospRepMain->btRunClick(frmHospRepMain->btRun);
        frmHospRepMain->btRunAllClick(frmHospRepMain->btRunAll);

		delay(2000);
		frmHospRepMain->Close();

	 }


 }
 else
 {
	 reportRunning = 0;
	 //m->Lines->Add("dismiss report activity "+DateTimeToStr(Now()));
     //WinExec("hosp_rep.exe /r",1);
 }


 Timer1->Enabled=timer1_enabled_;
}
//---------------------------------------------------------------------------



String getRespFullName_T1(TSurveyFileInfo fiByName, String name_only)
{
	                  add_slash(sDirOutcome);
					  // @@subdirectories
					  String sDirOutFinal = sDirOutcome;
					  String sDirMask = IntToStr( fiByName.CODE_MO)+"";
					  //sDirMask.Delete(1,2);
					  String sSubdirAuto = sDirMask;
					  sDirMask=sDirMask+"*";
					  TStringList *ssSubdirs=new  TStringList;
					  SearchInDir_Subdirs(sDirOutcome,sDirMask,ssSubdirs);
					  if (ssSubdirs->Count>0) {
						  sDirOutFinal=sDirOutFinal+ssSubdirs->Strings[0];
					  }else
					  {
						  try{
						   add_slash(sDirOutFinal);
						   sDirOutFinal=sDirOutFinal+sSubdirAuto+"_auto";
						   ForceDirectories( sDirOutFinal);
						  }catch (Exception &e)
						  {
						   sDirOutFinal=   sDirOutcome;
						  }
					  }
					  ssSubdirs->Free();
					  add_slash(sDirOutFinal);
					  //^^
					  String fn= /*sDirOutcome @@subdirs*/ sDirOutFinal +name_only;// makeResponseFileName(  fi );
					  return fn;
}


String getRespFullName_wrong_file(TSurveyFileInfo fiByName, String name_only)
{
	 String sDirOutFinal = sDirOutcome;
	 add_slash(sDirOutFinal);

	 if (fiByName.CODE_MO>0) {
		 String fn= getRespFullName_T1 ( fiByName,  name_only);
		 return fn;
	 }

	 String fn= /*sDirOutcome @@subdirs*/ sDirOutFinal +name_only;// makeResponseFileName(  fi );
	 return fn;
}


void unknown_faulty_file_action(String sname)
{                  /*
	ForceDirectories(sDirProcessed);
				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r >0) {
					 if (FileExists(sFileProcessed)) {
						 DeleteFile(sname);
					 }
				  }

				  TStringList *slReport = new TStringList;
				  fiByName.wrong_filename=1; // not PM, T1, T2...
				  fiByName.ORIGINAL_FILENAME=   ExtractFileName(sname);
				  make_error_report_wrong_file(fiByName,slReport);//@@err

				  String resp_fill_name=getRespFullName_wrong_file(fiByName,  "R"+ExtractFileName(sname));
				  slReport->SaveToFile(resp_fill_name);
				  slReport->Free();
				  */
}


bool avoid_receiving_contained_log=0;
bool avoid_receiving_processed_file=0;


void __fastcall TfmSurveyMain::btNewFileAddedClick(TObject *Sender)
{
  Application->ProcessMessages() ;
  try{
  //
  //String sDirProcessed=ExtractFilePath(Application->ExeName)+"processed\\";
  sDirProcessed=iniSet->ReadString("settings","processed_dir","");

  add_slash(sDirProcessed);
	/*
  if (sDirProcessed[sDirProcessed.Length()]!='\\') {
	  sDirProcessed=sDirProcessed+"\\";
  }   */
  TDirectoryObserver* obs=   (TDirectoryObserver*)Sender;

  int ntx =((TDirectoryObserver*)Sender)->ifile_processing;
	//(int) Sender;
  if (ntx!=-1) {
       m->Lines->Clear() ;
	   String sname = obs->ssNew->Strings[ntx];
	   String sFileProcessed =  sDirProcessed+ExtractFileName(sname);

	   lbxFiles->Items->Add(DateTimeToStr(Now())+"-> "+ExtractFileName(sname));

	   m->Lines->Add("event>> "+sname);
	   StatusBar1->SimpleText=ExtractFileName(sname);
	   //return;
	   //if (ExtractFileExt(sname)==".xml")
	   {
			  m->Lines->Add("XML<< ");
			  TSurveyFileInfo fiByName;
			  int err_file_name = fileNameInspect(sname,m->Lines,fiByName);
			  int id_log =  find_file_log(qOmni, sname);
			  if (id_log>-1 && avoid_receiving_contained_log) {
				  m->Lines->Add("file was already received, found in Log: "+sname);
				  m->Lines->Add("file id="+IntToStr(id_log));
				  ForceDirectories(sDirProcessed);
				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r >0) {
					 if (FileExists(sFileProcessed)) {
						 DeleteFile(sname);
					 }
				  }
				  return;
			  }
			  else
			  {

			  }

			  String sProcessedFile_possible =  sDirProcessed+ExtractFileName(sname);
			  if (FileExists(sProcessedFile_possible) && avoid_receiving_processed_file) {
				 m->Lines->Add("file already exists in processed directory: "+sProcessedFile_possible);
				 m->Lines->Add("Assuming prcessed earlier, therefore processing canceled");
				 DeleteFile(sname);
				 return;
			  }


			  if (err_file_name==0 && fiByName.sFileType=="PM")
			  {
				  TSurveyFileInfo fi;
				  std::vector<TPersonInfo> pv;
				  int err = process_survey_xml(sname,fi,pv, m->Lines);
				  /*void add_file(
	   TADOQuery* qOmni,
	   String FILENAME,
	   String DATA,
	   String DATA_OT,
	   int REC_AMOUNT,
	   int FILE_TYPE,
	   TDateTime PROC_DT,
	   int CODE_MO,
	   int ERROR_CODE )*/
	   m->Lines->Add("Line 20 ");
				  int id_file =	   add_file(
	   qOmni,
	   /*fi.FILENAME*/sname,
	   fi.dt_date,
	   fi.DATA_OT,
	   fi.rec_emount ,
	   1,
	   Now(),
	   fi.CODE_MO,
	   err);

	   m->Lines->Add("Line 21 ");

				  if (!err) {
					  //
					  /*int add_survey(   TADOQuery *qOmni,
	  TDate SDATE     ,
	  TDate EDATE     ,
	  int SMOCOD      ,
	  int DISP_TYP    ,
	  int CODE_MO      ,
	  String DS         ,
	  int ID_FILE        ,
	  int ID_PERSON       ,
	  int N_REC
)*/

					  for (int i = 0; i < pv.size(); i++) {
						  //
						  m->Lines->Add("Line 22 ");

						  TPersonInfo pers = pv[i];
						  int id_physical;
						  TPersonInfo  inf;
						  int id_found = find_idcard_person( qOmni, pers.mscard_number, id_physical,inf);
						  int id_person=-1;
						  if (id_found==-1) {
						   // completely new
						   m->Lines->Add("Line 23 ");
						   int id_phys_max= findIDphysicalLast(qOmni);
						   m->Lines->Add("Line 24 ");
						   id_person=add_person(qOmni,pers.lastname,pers.firstname,pers.patronimic,pers.sex,pers.mscard_number,id_phys_max+1,id_file,pers.birth_date);
						   m->Lines->Add("Line 25 ");

						  }
						  else
						  {
						   if (inf.lastname!=pers.lastname || inf.firstname!=pers.firstname || inf.patronimic!=pers.patronimic )
						   {
							// changed name - add new record with same id_physical
							id_person=add_person(qOmni,pers.lastname,pers.firstname,pers.patronimic,pers.sex,pers.mscard_number,id_physical,id_file,pers.birth_date);
						   }else
						   {
						    // use the found person's ID
                            id_person=id_found;
						   }
						  }
						  if (id_person!=-1) {
							  //
							 add_survey(qOmni,
							 (pers.start_date),
							 (pers.end_date),
							 StrToIntDef( pers.SMOCOD,0),
							 StrToIntDef(pers.SURVEY_TYPE,0),
							 StrToIntDef( fi.CODE_MO,0),
							 pers.diagnosis,
							 id_file,
							 id_person,
							 pers.n_rec
							);


						  }
					  }

				  }
				  ForceDirectories(sDirProcessed);
				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r > 0 ) //@@was r & 1
				  {
					 if (FileExists(sFileProcessed)) {
						 m->Lines->Add("File copied to: "+sFileProcessed);
						 DeleteFile(sname);
					 } else m->Lines->Add("Error 1 on File copying to: "+sFileProcessed);
				  }  else m->Lines->Add("Error 2 on File copying to: "+sFileProcessed);

			  } else if (err_file_name==0 && fiByName.sFileType=="RM") {
				  //
				  TReadFileInfo fi;
				  int err = process_read_xml(sname,fi, m->Lines);
				  if (err==0) {
					  std::vector<TPersonInfo> vp ;
					  select_surveys(qOmni, /*fi.CODE_MO*/fiByName.CODE_MO, fi.SDATE, fi.EDATE, vp, m->Lines);
					  // pack
					  TSurveyFileInfo sfi;

					  //String sOutDir =
					  //ini->ReadString("settings","outcoming_dir","c:\\develop\\outcome\\");
					  add_slash(sDirOutcome);
					  // @@subdirectories
					  String sDirOutFinal = sDirOutcome;
					  String sDirMask = IntToStr( fiByName.CODE_MO)+"";
					  sDirMask.Delete(1,2);
					  String sSubdirAuto = sDirMask;
					  sDirMask=sDirMask+"*";
					  TStringList *ssSubdirs=new  TStringList;
					  SearchInDir_Subdirs(sDirOutcome,sDirMask,ssSubdirs);
					  if (ssSubdirs->Count>0) {
						  sDirOutFinal=sDirOutFinal+ssSubdirs->Strings[0];
					  }else
					  {
						  try{
						   add_slash(sDirOutFinal);
						   sDirOutFinal=sDirOutFinal+sSubdirAuto+"_auto";
						   ForceDirectories( sDirOutFinal);
						  }catch (Exception &e)
						  {
						   sDirOutFinal=   sDirOutcome;
						  }
					  }
					  ssSubdirs->Free();
					  add_slash(sDirOutFinal);
					  //^^
					  String fn= /*sDirOutcome @@subdirs*/ sDirOutFinal + makeResponseFileName(  fi );
					  sfi.FILENAME=ExtractFileName(sname); //fn);
					  sfi.CODE_MO=fi.CODE_MO;
					  sfi.DATA_OT=Now();
					  sfi.dt_date=Now();
					  sfi.ORIGINAL_FILENAME=  ExtractFileName(sname);

					  surveys_to_xml(vp, fn, sfi);
				  }
				  else
				  {
					  m->Lines->Add("Error on parsing read XML: "+IntToStr(err));
				  }

				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r >0) //@@
				  {
				  if (FileExists(sFileProcessed))
				   {
						 m->Lines->Add("File copied to: "+sFileProcessed);
						 DeleteFile(sname);
				   }else m->Lines->Add("Error 1 on File copying to: "+sFileProcessed);
				  }else m->Lines->Add("Error 2 on File copying to: "+sFileProcessed);

			  }
			  else if ( err_file_name==0
			  //&& fiByName.sFileType.UpperCase()[1]=='T'
			  && fiByName.sFileType.UpperCase().Pos("T")==1
			  )
			  {
				 //T1-M-1801-2020-05-21.xml
			  //}sname
				  m->Lines->Add("Line 1000 ");
				  THospFileInfo_T1 file_info;
				  file_info.CODE_MO= fiByName.CODE_MO;
				  file_info.FILENAME=  ExtractFileName(fiByName.FILENAME);
				  file_info.dt_date=   fiByName.dt_date;

				  if( fiByName.sFileType.UpperCase()=="T1" )
				  {
				   m->Lines->Add("Line 1001 ");
				   std::vector<TPersonInfoHosp_T1> persons;
				   process_hosp_xml_t1(sname, file_info, persons, m->Lines ,qOmni);
				   m->Lines->Add("Line 1100 ");
				   //@@@@after errors::: hosp_to_base_t1(qOmni, file_info, persons, m->Lines);
				   //void make_error_report( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T1> persons, TStrings* ss )
				   TStringList *slReport = new TStringList;
				   m->Lines->Add("Line 1200 ");
				   make_error_report_T1(file_info, persons,  slReport, qOmni);//@@err

				   hosp_to_base_t1(qOmni, file_info, persons, m->Lines);
				   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
				   //performReferenceCheckings(

				   m->Lines->Add("Line 1300 ");
				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   m->Lines->Add("Line 1400 ");
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();
				  }else
				  if( fiByName.sFileType.UpperCase()=="T2" )
				  {
				   std::vector<TPersonInfoHosp_T2> persons;
				   process_hosp_xml_t2(sname, file_info, persons, m->Lines ,qOmni);
				   ////@@@@ hosp_to_base_t2(qOmni, file_info, persons, m->Lines);

				   TStringList *slReport = new TStringList;
				   make_error_report_T2(file_info, persons,  slReport, qOmni);//@@err

				   //int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message )
				   //performReferenceCheckings(

				   hosp_to_base_t2(qOmni, file_info, persons, m->Lines);

				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();
				  }else
				  if( fiByName.sFileType.UpperCase()=="T3" )
				  {
				   std::vector<TPersonInfoHosp_T3> persons;
				   process_hosp_xml_t3(sname, file_info, persons, m->Lines,qOmni );
				   //hosp_to_base_t3(qOmni, file_info, persons, m->Lines);



				   TStringList *slReport = new TStringList;
				   make_error_report_T3(file_info, persons,  slReport, qOmni);//@@err

				   hosp_to_base_t3(qOmni, file_info, persons, m->Lines);

				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();

				  }else
				  if( fiByName.sFileType.UpperCase()=="T4" )
				  {
				   std::vector<TPersonInfoHosp_T4> persons;
				   process_hosp_xml_t4(sname, file_info, persons, m->Lines , qOmni);
				   //hosp_to_base_t4(qOmni, file_info, persons, m->Lines);


				   TStringList *slReport = new TStringList;
				   make_error_report_T4(file_info, persons,  slReport, qOmni);//@@err

				   hosp_to_base_t4(qOmni, file_info, persons, m->Lines);

				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();

				  } else
				  if( fiByName.sFileType.UpperCase()=="T5" )
				  {
				   std::vector<TPersonInfoHosp_T5> persons;
				   process_hosp_xml_t5(sname, file_info, persons, m->Lines ,qOmni);
				   //hosp_to_base_t5(qOmni, file_info, persons, m->Lines);


				   TStringList *slReport = new TStringList;
				   make_error_report_T5(file_info, persons,  slReport, qOmni);//@@err

				   hosp_to_base_t5(qOmni, file_info, persons, m->Lines);

				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();
				  }else
				  if( fiByName.sFileType.UpperCase()=="T6" )
				  {
				   std::vector<TPersonInfoHosp_T6> persons;
				   process_hosp_xml_t6(sname, file_info, persons, m->Lines ,qOmni);
				   //hosp_to_base_t6(qOmni, file_info, persons, m->Lines);



				   TStringList *slReport = new TStringList;
				   make_error_report_T6(file_info, persons,  slReport, qOmni);//@@err

				   hosp_to_base_t6(qOmni, file_info, persons, m->Lines);

				   String resp_fill_name=getRespFullName_T1(fiByName,  "R"+ExtractFileName(fiByName.FILENAME));
				   slReport->SaveToFile(resp_fill_name);
				   slReport->Free();
				  }


				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r >0) //@@
				  {
				  if (FileExists(sFileProcessed))
				   {
						 m->Lines->Add("File copied to: "+sFileProcessed);
						 DeleteFile(sname);
				   }else m->Lines->Add("Error 1 on File copying to: "+sFileProcessed);
				  }else m->Lines->Add("Error 2 on File copying to: "+sFileProcessed);

			  }
			  else
			  {
				  // incorrect file name

				  int id_file =	   add_file(
					qOmni,
					ExtractFileName(sname),
					Now(),
					Now(),
					0 ,
					10,
					Now(),
					0,
					/*20*/ERROR_INCORRECT_FILENAME );

				  ForceDirectories(sDirProcessed);
				  int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
				  if (r >0) {
					 if (FileExists(sFileProcessed)) {
						 DeleteFile(sname);
					 }
				  }

				  TStringList *slReport = new TStringList;
				  fiByName.wrong_filename=1; // not PM, T1, T2...
				  fiByName.ORIGINAL_FILENAME=   ExtractFileName(sname);
				  make_error_report_wrong_file(fiByName,slReport);//@@err

				  String resp_fill_name=getRespFullName_wrong_file(fiByName,  "R"+ExtractFileName(sname));
				  slReport->SaveToFile(resp_fill_name);
				  slReport->Free();

			  }
	   }

  }
  }catch(Exception & e)
  {
   //unknown error
   try{
   TDirectoryObserver* obs=   (TDirectoryObserver*)Sender;
   int ntx =((TDirectoryObserver*)Sender)->ifile_processing;
	//(int) Sender;
   if (ntx!=-1)
   {

	   m->Lines->Clear() ;
	   String sname = obs->ssNew->Strings[ntx];
	   String sFileProcessed =  sDirProcessed+ExtractFileName(sname);
	   ForceDirectories(sDirProcessed);
	   int r=CopyFileW(sname.c_str(),sFileProcessed.c_str(),0);
	   DeleteFile(sname);


	   TSurveyFileInfo fiByName;
	   int err_file_name = fileNameInspect(sname,m->Lines,fiByName);

	   TStringList *slReport = new TStringList;
	   fiByName.wrong_filename=1; // not PM, T1, T2...
	   fiByName.ORIGINAL_FILENAME=   ExtractFileName(sname);
	   make_error_report_wrong_file(fiByName,slReport);//@@err
	   String resp_fill_name=getRespFullName_wrong_file(fiByName,  "R"+ExtractFileName(sname));
	   slReport->SaveToFile(resp_fill_name);
	   slReport->Free();


   }
   }catch(...){};


  }
}
//---------------------------------------------------------------------------
bool do_run_stopped=0;

void __fastcall TfmSurveyMain::FormCreate(TObject *Sender)
{
 if (ParamCount()>0) {
	String par0 = ParamStr(1).Trim();
	if (par0=="/s") {
	   //btStop->Click();
	   do_run_stopped=1;
	}
 }

 iniSet=new TIniFile(ExtractFilePath(Application->ExeName)+"survey_info.ini");

 obs.OnNewFileAdded=btNewFileAddedClick;
 obs.directory=  iniSet->ReadString("settings","incoming_dir","c:\\develop\\income\\");

 obsHosp.OnNewFileAdded=btNewFileAddedClick;
 obsHosp.directory=  iniSet->ReadString("settings","incoming_dir_hosp","c:\\develop\\income_hosp\\");

 //"h:\\develop\\hist_tester\\DATA\\2011-06-01\\10-50";
 sDirProcessed = iniSet->ReadString("settings","processed_dir","c:\\develop\\income\\processed\\");

 // add the program's icon to the tray
 SetWindowLong(Application->Handle,GWL_EXSTYLE, WS_EX_TOOLWINDOW);
 //Application->ShowMainForm=false;
    NOTIFYICONDATA info;
    memset(&info,0,sizeof(info));
    info.cbSize          =sizeof(info) ;
    info.hWnd            =Handle; // form handle gets notification message
	info.uID             =0;//WM_TRAY_ICON_ID; // id of icon - passed back in wParam of message
	info.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
    info.uCallbackMessage=WM_TRAY_ICON_MESSAGE; // our notification message
	info.hIcon           =Application->Icon->Handle;;//hIcon; // icon to display
	String s="Survey Info Receiver";
	//WCHAR s[100]="hhh";
	wcsncpy(info.szTip, s.w_str() ,sizeof(info.szTip)); // set tool tip text
    info.szTip[sizeof(info.szTip)-1]='0';
    info.uFlags          =NIF_MESSAGE | NIF_ICON | NIF_TIP ; // indicate modifications
	Shell_NotifyIcon(NIM_ADD ,&info) ; // add it

	lbxFiles->Width=m->Width;

	if (Timer1->Enabled==false) {
		btStop->Enabled=false;
		btRun->Enabled=true;

	}

	if (ini->ReadInteger("program","run",0)>0) {
		//
		//btRun->Click();
		//Down=true;
		//btRunClick(0);
	}

}
//---------------------------------------------------------------------------


int add_person(
	   TADOQuery* qOmni,
	   String FAM,
	   String IM,
	   String OT,
	   int W,
	   String NPOLIS,
	   int ID_Physical,
	   int ID_File, TDate DR )
{
	//
	String sql=
	"insert into persons (FAM,   IM,     OT,  W, NPOLIS, ID_Physical,ID_File,DR) values( \
	'"+FAM+"','"+IM+"','"+OT+"',"+IntToStr(W)+", '"+NPOLIS+"', "+IntToStr(ID_Physical)+",     "+IntToStr(ID_File)+", "+sqlDate(DR)+"  ) ";
	qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->ExecSQL();
   int id = scopeIdentity(qOmni);
   return id;
}


int find_idcard_person(TADOQuery* qOmni,String NPOLIS,int & id_physical, TPersonInfo & inf )
{
	String sql="select ID,id_physical,FAM,IM,OT from persons where NPOLIS='"+NPOLIS.Trim()+"' order by ID desc";
	qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->Open();
	int id=-1;
	if (qOmni->RecordCount>0) {
	   id = qOmni->FieldValues["ID"];
	   id_physical= qOmni->FieldValues["id_physical"];
	   inf.lastname =  qOmni->FieldValues["FAM"];
	   inf.firstname =  qOmni->FieldValues["IM"];
	   inf.patronimic =  qOmni->FieldValues["OT"];
	   inf.id_physical= id_physical;
	   inf.id =id;
	}
	return id;

}



//String sql_date(TDateTime dt){ return "'1-1-1990'" ; };

/*
AnsiString sqlDate(TDateTime dt ){
 String t= "'"+FormatFloat("0000",YearOf(dt))+'-'+
  FormatFloat("00",MonthOf(dt))+'-'+
  FormatFloat("00",DayOf(dt))+"'";
 return t;
}

AnsiString sqlDateTime(TDateTime dt ){
 String t= "'"+FormatFloat("0000",YearOf(dt))+'-'+
  FormatFloat("00",MonthOf(dt))+'-'+
  FormatFloat("00",DayOf(dt))+ " " +
  FormatFloat("00",HourOf(dt))+":"+
  FormatFloat("00",MinuteOf(dt))+":"+
  FormatFloat("00",SecondOf(dt))+"'";
 return t;
}
  */

void add_survey(
	  TADOQuery* qOmni
	  ,TDateTime SDATE
	  ,TDateTime EDATE
	  ,String SMOCOD
	  ,int DISP_TYP
	  ,int CODE_MO
	  ,String DS
	  ,int ID_File
	  ,int ID_PERSON
	  ,int N_REC
)
{
	String sql=
"insert into surveys (SDATE,                EDATE,       SMOCOD,              DISP_TYP,      CODE_MO,      DS,                   ID_FILE,    ID_PERSON, N_REC) values( \
		  "+sqlDate(SDATE)+","+sqlDate(EDATE)+",'"+SMOCOD+"',"+IntToStr(DISP_TYP)+", "+CODE_MO+", '"+(DS)+"' ,     "+IntToStr(ID_File)+","+IntToStr(ID_PERSON)+","+IntToStr(N_REC)+"  ) ";
	qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->ExecSQL();
}


void add_file_rec(
	  TADOQuery* qOmni,
	  String filename  ,
	  TDateTime DATA_OT
)
{

}


void __fastcall TfmSurveyMain::Button4Click(TObject *Sender)
{
 String sql=
 "insert into persons (FAM,   IM,     OT,  W, NPOLIS, ID_Physical,ID_File) values( \
				  'Lehtonen','Iiris','Lee',2, 123456, 1,          2  ) ";

  qOmni->SQL->Text=sql;
  qOmni->ExecSQL();

}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::Button5Click(TObject *Sender)
{
 add_person(qOmni,"Wagner","Elizabeth","Sooth",2,"002",1,1,Now());
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::Button6Click(TObject *Sender)
{
 //
 add_survey(qOmni, Now(),Now()+1,"aaa",1,1,"xx12",1,1,2);
}
//---------------------------------------------------------------------------


void TfmSurveyMain::TrayIcon_HandleMessage(TMessage &Message)
{
  //Show();
  switch (Message.LParam)
  {
	case WM_LBUTTONDOWN:
	  // handle l
	  Show();
	  break;
	case WM_LBUTTONDBLCLK:
      // handle left mouse button double click
      break;
	case WM_RBUTTONDOWN:
	  // handle right mouse button click
	  tagPOINT p;
	  GetCursorPos(&p);

      PopupMenu1->Popup(p.x,p.y);
      break;

    // ... and so on ...

    case WM_RBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    break;
  }
}
void __fastcall TfmSurveyMain::FormClose(TObject *Sender, TCloseAction &Action)
{
   Action=caNone;
   Visible=0;
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::Exit1Click(TObject *Sender)
{
	Timer1->Enabled=0;
	//ADOConnection->Connected=0;
	DataModuleBase->ADOConnection->Connected=0;
	OnClose=0;


	NOTIFYICONDATA info;
	memset(&info,0,sizeof(info));
	info.cbSize          =sizeof(info) ;
	info.hWnd            =Handle; // form handle gets notification message
	info.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	info.uCallbackMessage=WM_TRAY_ICON_MESSAGE; // our notification message
	info.hIcon           =Application->Icon->Handle;;//hIcon; // icon to display
	info.uFlags          =NIF_MESSAGE | NIF_ICON | NIF_TIP ; // indicate modifications
	Shell_NotifyIcon(NIM_DELETE ,&info) ; // add it
		 /*	   */

		Close();
}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::IDphysmax1Click(TObject *Sender)
{
	int x=  findIDphysicalLast(qOmni);
	m->Lines->Add(IntToStr(x));
}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::Settings1Click(TObject *Sender)
{
	 bool wasRunning= Timer1->Enabled;
	 //btRun->Down;
	 if (wasRunning) {
		 //btStop->Click() ;
		 Timer1->Enabled=0;
	 }
	 Application->ProcessMessages() ;
	 fmSettings->ShowModal();
	 if (wasRunning) {
		 //
		 //btRun->Down=true;
		 //btRun->Click();
		 Timer1->Enabled=   1;


	 }
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::btRunClick(TObject *Sender)
{
  //


  Timer1->Enabled= btRun->Down;
  btRun->Enabled= !(btRun->Down) ;
  btStop->Enabled= !(btStop->Down) ;

  if (btRun->Down) {

   ini->WriteInteger("program","run",1);
   fmDatabase->btConnectClick(0);

  }  else{
	ini->WriteInteger("program","run",0);

  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::Database1Click(TObject *Sender)
{
   fmDatabase->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::Timer2Timer(TObject *Sender)
{
  if (just_started) {
	  just_started=0;
	  Timer2->Enabled=0;
      //   /s  -to run the program stopped
	  if (ini->ReadInteger("program","run",0)>0 && !do_run_stopped) {
		//
		btRun->Click();
		//Down=true;
	  }

  }
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::findIDphysical1Click(TObject *Sender)
{
   int x=findIDphysicalLast(qOmni);
   m->Lines->Add("findIDphysicalLast "+IntToStr(x) );
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::findOutcomedirectory1Click(TObject *Sender)
{
   //
   String s = "";
   if (m->Lines->Count)  s = m->Lines->Strings[m->Lines->Count-1].Trim();
   TStringList * ssDirs=new TStringList ;
   SearchInDir_Subdirs(sDirOutcome,s+"*",ssDirs);
   m->Lines->Add("found directories: "+IntToStr(ssDirs->Count));
   m->Lines->AddStrings(ssDirs);

   if (ssDirs->Count>0) {


	 String dst =ssDirs->Strings[0];
	 if (sDirOutcome[sDirOutcome.Length()]!='\\') {
		   sDirOutcome=sDirOutcome+ "\\";
	 }
	 dst=sDirOutcome + dst;
	 m->Lines->SaveToFile(dst+"\\test.log") ;
	 m->Lines->Add("copied: "+  dst+"\\test.log");

   }

}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::hospital1Click(TObject *Sender)
{

  //int fileNameInspect(String fname, TStrings * ss, TSurveyFileInfo & fi)
  String fn= "T1-M-1801-2020-05-30.xml";
  //"T1-M-1801-2020-05-51.xml";
  TSurveyFileInfo fi;
  int res= fileNameInspect(fn,m->Lines,fi);

  m->Lines->Add(IntToStr(res));
  m->Lines->Add("");

  THospFileInfo_T1 sfi;
  std::vector<TPersonInfoHosp_T1> persons;

  process_hosp_xml_t1(/*"T1-M-1801-2020-05-21.xml"*/fn,sfi,persons,m->Lines,qOmni);

}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::t21Click(TObject *Sender)
{
  THospFileInfo_T1 sfi;
  std::vector<TPersonInfoHosp_T2> persons;
  process_hosp_xml_t2("T2-M-0018-2020-04-13.xml",sfi,persons,m->Lines,qOmni);
}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::t41Click(TObject *Sender)
{
  THospFileInfo_T1 sfi;

  std::vector<TPersonInfoHosp_T4> persons;
  process_hosp_xml_t4("T4-M-0018-2020-04-02.xml",sfi,persons,m->Lines,qOmni);
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::t51Click(TObject *Sender)
{
  THospFileInfo_T1 sfi;
  std::vector<TPersonInfoHosp_T5> persons;
  process_hosp_xml_t5("T5-M-0018-2020-04-13.xml",sfi,persons,m->Lines,qOmni);

}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::t61Click(TObject *Sender)
{
  THospFileInfo_T1 sfi;
  std::vector<TPersonInfoHosp_T6> persons;
  process_hosp_xml_t6("T6-M-0001-03-04-2020.XML",sfi,persons,m->Lines,qOmni);

}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::strin1Click(TObject *Sender)
{
std::string s="aaaa";
//String s2(s);
s+="bv";

Caption = s[ s.find("b")];
// s.c_str();
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::required1Click(TObject *Sender)
{
	 std::vector<String>  res ;
	 std::vector<int>  errs ;

	 //getRequiredFields(qOmni, 2, res,errs);
     vect_to_ss(res, m->Lines);
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::scope1Click(TObject *Sender)
{
 int x=scopeIdentity(qOmni);
 m->Lines->Add("scope "+IntToStr(x));
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::ident1Click(TObject *Sender)
{
 int x=identCurrent(qOmni,"[DB_263].[dbo].[HOS_T1_SCT]");
  m->Lines->Add("ident "+IntToStr(x));
}
//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::int1Click(TObject *Sender)
{
	String s= FormatFloat("0000",4);
	m->Lines->Add(s);
}
//---------------------------------------------------------------------------

void __fastcall TfmSurveyMain::About2Click(TObject *Sender)
{
  TMsgDlgButtons xx;    xx<<mbOK;
  MessageDlg("Hospitalization/Survey 1.11",mtInformation,xx,0);
}
//---------------------------------------------------------------------------









void __fastcall TfmSurveyMain::exec1Click(TObject *Sender)
{
//AnsiString s="H:\\develop\\hospitalization\\hosp_rep2.exe /r";
//AnsiString path =ExtractFilePath(Application->ExeName);
AnsiString sCmd ="report.bat";
//"hosp_rep2.exe";
AnsiString sPar = "";

//ExtractFilePath(Application->ExeName)+"hosp_rep2.exe /r";
//WinExec(s.c_str() ,3);
//ShellExecuteA(Handle,0,name.c_str(),"/r",path.c_str(),2);
ShellExecuteA(Handle,0,sCmd.c_str(),sPar.c_str(),0,3 );
}

//---------------------------------------------------------------------------


void __fastcall TfmSurveyMain::Reports1Click(TObject *Sender)
{
	frmHospRepMain->Show();
}
//---------------------------------------------------------------------------

