//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#include <XMLDoc.hpp>
#include <XMLIntf.hpp>
#include <ComObj.hpp>
#include <stdio.h>
#include <vector>


#include <dateutils.hpp>

#include "survey_utils.h"

#include "file_dir_funcs.hpp"
#include "fm_katsoja.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma hdrstop


void readSqlDate(String s, int &y, int &m,int &d)
{
 try{
  if (s.Length()==0) {
				   return;
  }
  char x=  s.w_str()[0];
  if ( x==39
  //'\''
  )
  {
	 //s[0]=' ';
	 s=s.SubString1(2,s.Length()-1);
  }
  char x2 =  s.w_str()[s.Length()-1];
  if ( x2 =='\'') {
	 //s[s.Length()-1]=' ';
	 s=s.SubString1(1,s.Length()-1);

  }
  s=s.Trim();
  swscanf(s.w_str(),L"%d-%d-%d", &y,  &m, &d);

  }catch(...){};
}


void readSqlTime(String s, int &h, int &m)
{
 try{
  if (s.Length()==0) {
				   return;
  }
  char x=  s.w_str()[0];
  if ( x==39
  //'\''
  )
  {
	 //s[0]=' ';
	 s=s.SubString1(2,s.Length()-1);
  }
  char x2 =  s.w_str()[s.Length()-1];
  if ( x2 =='\'') {
	 //s[s.Length()-1]=' ';
	 s=s.SubString1(1,s.Length()-1);

  }
  s=s.Trim();
  swscanf(s.w_str(),L"%d-%d", &h,  &m);

  }catch(...){};
}



void readSqlTime_colon(String s, int &h, int &m)
{
 try{
  if (s.Length()==0) {
				   return;
  }
  char x=  s.w_str()[0];
  if ( x==39
  //'\''
  )
  {
	 //s[0]=' ';
	 s=s.SubString1(2,s.Length()-1);
  }
  char x2 =  s.w_str()[s.Length()-1];
  if ( x2 =='\'') {
	 //s[s.Length()-1]=' ';
	 s=s.SubString1(1,s.Length()-1);

  }
  s=s.Trim();
  swscanf(s.w_str(),L"%d:%d", &h,  &m);

  }catch(...){};
}


TDateTime readSqlDateAsDT(String s)
{
   int y,m,d;
   readSqlDate(s,y,m,d);
   TDateTime dt;//=
   TryEncodeDate(y,m,d,dt);
   return dt;
}


int process_read_xml(String fn, TReadFileInfo& file_info, TStrings* ssLog)
{
  int error_code=0;
  int flags_summary=0;
  int struct_flags=0;
  int hdr_flags=0;
  TXMLDocument *x= new TXMLDocument(Application);
  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;
  for (int i = 0; i < cnt; i++) {

	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 )
		 {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);
			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			}
			if (ndStub->NodeName.UpperCase()=="DATA_B")//"SDATE") //@@changed 2020-05-18
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.SDATE=dtStub;
			  }catch(...){};
			}
			if (ndStub->NodeName.UpperCase()=="DATA_E")//"EDATE") //@@changed 2020-05-18
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.EDATE=dtStub;
			  }catch(...){};
			}
			if (ndStub->NodeName.UpperCase()=="CODE_MO")
			{
			  hdr_flags|= 1<<2;
			  file_info.CODE_MO=StrToIntDef(  ndStub->Text.Trim(),0);
			}
			}



		 }
  }


   return error_code;

}


int process_survey_xml(String fn, TSurveyFileInfo &file_info, std::vector<TPersonInfo> &persons, TStrings * ssLog )
{
  int error_code=0;
  int flags_summary=0;
  TXMLDocument *x= new TXMLDocument(Application);

  x->LoadFromFile(fn);
  IXMLNode *ndRoot = x->DocumentElement;
  int cnt=  ndRoot->ChildNodes->Count;

  unsigned char struct_flags=0;
  unsigned char hdr_flags=0;
  unsigned short pers_flags=0;

  for (int i = 0; i < cnt; i++) {

	 IXMLNode *ndChild=	 ndRoot-> ChildNodes->Get(i);

	 if (ndChild->NodeName.LowerCase()=="zglv" ) {
		 if ( struct_flags & 1<<0 ) {

			 ssLog->Add("ERROR: More than one header");
			 if (error_code==0) error_code=5;

		 }
		 struct_flags|=1<<0;

		 file_info.dt_date=1;
		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//
			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);
			if (ndStub->NodeName.UpperCase()=="FILENAME")
			{
			  hdr_flags|= 1<<0;
			  file_info.FILENAME=ndStub->Text.Trim();
			}
			if (ndStub->NodeName.UpperCase()=="DATA")
			{
			  hdr_flags|= 1<<1;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   TDateTime dtStub= EncodeDate(y,m,d);
			   file_info.dt_date=dtStub;
			  }catch(...){};
			}
			if (ndStub->NodeName.UpperCase()=="CODE_MO")
			{
			  hdr_flags|= 1<<2;
			  file_info.CODE_MO=StrToIntDef(  ndStub->Text.Trim(),0);
			}
			if (ndStub->NodeName.UpperCase()=="DATA_OT")
			{
			  hdr_flags|= 1<<3;
			  int y;
			  int m;
			  int d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   file_info.DATA_OT= EncodeDate(y,m,d);
			  }catch(...){};
			}
		 }

		 if (hdr_flags != 0x0F) {
			 // header does not contain all the required fields
			 if ( !( hdr_flags & 1<<0 ) ) {
				ssLog->Add("FILENAME is missing");
				if (error_code==0) error_code=1;
			 }
			 if ( !( hdr_flags & 1<<1 ) ) {
				ssLog->Add("DATA (creation date!) is missing");
				if (error_code==0) error_code=2;
			 }
			 if ( !( hdr_flags & 1<<2 ) ) {
				ssLog->Add("CODE_MO (health facility code) is missing");
				if (error_code==0) error_code=3;
			 }
			 if ( !( hdr_flags & 1<<3 ) ) {
				ssLog->Add("DATA_OT (report date!) is missing");
				if (error_code==0) error_code=4;
			 }
		 }

	 }

	 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
		IXMLNode *ndChild2 =	 ndChild-> ChildNodes->Get(k);
		String s= ndChild2->XML;


	 }
	 /////////////////////
	 if (ndChild->NodeName.LowerCase()=="zap" ) {
		 struct_flags|=1<<1;
		 TPersonInfo pinf;
		 memset(&pinf,0,sizeof(pinf));

		 for (int k = 0; k < ndChild->ChildNodes->Count; k++) {
			//


			IXMLNode * ndStub =  ndChild-> ChildNodes->Get(k);
			if (ndStub->NodeName.UpperCase()=="N_ZAP")
			{
			  pers_flags|=1<<0;
			  pinf.n_rec= StrToIntDef(  ndStub->Text.Trim(),0);
			  ssLog->Add(("N_ZAP "+IntToStr(pinf.n_rec)));
			}
			if (ndStub->NodeName.UpperCase()=="FAM")
			{
			  pers_flags|=1<<1;
			  pinf.lastname=   ndStub->Text.Trim() ;
			  ssLog->Add("lastname "+pinf.lastname);
			}
			if (ndStub->NodeName.UpperCase()=="IM")
			{
			  pers_flags|=1<<2;
			  pinf.firstname=   ndStub->Text.Trim() ;
			  ssLog->Add("firstname "+pinf.firstname);
			}
			if (ndStub->NodeName.UpperCase()=="OT")
			{
			  pers_flags|=1<<3;
			  pinf.patronimic=   ndStub->Text.Trim() ;
			  ssLog->Add("patronimic "+pinf.patronimic);
			}

			if (ndStub->NodeName.UpperCase()=="W")
			{
			  pers_flags|=1<<4;
			  pinf.sex = StrToIntDef(  ndStub->Text.Trim(),0) ;
			  ssLog->Add("sex "+IntToStr(pinf.sex));
			}
			if (ndStub->NodeName.UpperCase()=="DR")
			{
			  pers_flags|=1<<5;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.birth_date= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("birth_date "+DateTimeToStr(pinf.birth_date));
			}
			if (ndStub->NodeName.UpperCase()=="NPOLIS")
			{
			  pers_flags|=1<<6;
			  pinf.mscard_number = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("mscard_number "+(pinf.mscard_number));
			}
		   if (ndStub->NodeName.UpperCase()=="DS")
			{
			  pers_flags|=1<<7;
			  pinf.diagnosis = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("diagnosis "+(pinf.diagnosis));
			}
			if (ndStub->NodeName.UpperCase()=="SDATE")
			{
			  pers_flags|=1<<8;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.start_date= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("start_date "+DateTimeToStr(pinf.start_date));
			}
			if (ndStub->NodeName.UpperCase()=="EDATE")
			{
			  pers_flags|=1<<9;
			  int y,m,d;
			  readSqlDate(ndStub->Text,y,m,d);
			  try{
			   pinf.end_date= EncodeDate(y,m,d);
			  }catch(...){};
			  ssLog->Add("end_date "+DateTimeToStr(pinf.end_date));
			}
			if (ndStub->NodeName.UpperCase()=="SMOCOD")
			{
			  pers_flags|=1<<10;
			  pinf.SMOCOD = (  ndStub->Text.Trim() ) ;
			  ssLog->Add("SMOCOD "+(pinf.SMOCOD));
			}
			if (ndStub->NodeName.UpperCase()=="DISP_TYP")
			{
			  ssLog->Add("node found:: DISP_TYP ");
			  pers_flags|=1<<11;
			  ssLog->Add("Line 1 ");
			  pinf.SURVEY_TYPE = StrToIntDef(  ndStub->Text.Trim() ,0) ;
			  ssLog->Add("Line 2 ");
			  ssLog->Add("SURVEY_TYPE "+(pinf.SURVEY_TYPE));
			  ssLog->Add("Line 3 ");
			}
			ssLog->Add("Line 4 ");
		 }
		 ssLog->Add("Line 5 ");
		 pinf.pers_flags=pers_flags;
		 ssLog->Add("Line 6 ");
		 if ( pers_flags != 0xFFF ) {
			  ssLog->Add("Line 7 ");
			  ssLog->Add("ERROR: wrong person record");
			  if (error_code==0) error_code=7;
		 }
		 ssLog->Add("Line 8 ");
		 persons.push_back(pinf);
		 ssLog->Add("Line 9 ");


	 }
	 ssLog->Add("Line 10 ");


  }

  ssLog->Add("Line 11 ");
  if ( !(struct_flags & 1<<0 )) {
	  ssLog->Add("ERROR: no header");
	  if (error_code==0) error_code=6;
  }
  if ( !(struct_flags & 1<<1 )) {
	  ssLog->Add("ERROR: not a single record found");
	  if (error_code==0) error_code=8;
  }
  file_info.rec_emount= persons.size();
  flags_summary = pers_flags |  (hdr_flags<<16) | (struct_flags<<24);

  ssLog->Add("Line 12 ");
  x->Free();
  ssLog->Add("Line 13 ");
  return error_code;
}


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


void clear_file_info_strings(TStrings * ss)
{
  for (int i = 0; i < ss->Count; i++)
  {
	  TSearchRec * rc = (TSearchRec*)ss->Objects[i];
	  if (rc!=0 )
	  try
	  {
		 delete rc;
	  }catch(...){};

  }
  ss->Clear();
}


void clear_file_info_data(TStrings * ss)
{
  for (int i = 0; i < ss->Count; i++)
  {
	  TSearchRec * rc = (TSearchRec*)ss->Objects[i];
	  if (rc!=0 )
	  try
	  {
		 delete rc;
	  }catch(...){};
      ss->Objects[i]=0;
  }
//  ss->Clear();
}


void TDirectoryObserver::step()
{
	//
	if (ssOld==0) {

		 ssOld=new TStringList;
		 ssNew=new TStringList;
		 //SearchInDir_files_only(directory,"*.*",  ssOld);

	}
	if (ssOld && ssNew) {


	 SearchInDir_files_only(directory,"*.*",  ssNew);

	 for (int i = 0; i < ssNew->Count; i++) {
		String s= ssNew->Strings[i];
		int ntx = ssOld->IndexOf(s);
		if (ntx==-1) {
			//new file added
			String sNew = ssNew->Strings[i];
			//m->Lines->Add("NEW FILE ADDED: "+sNew);
			if (OnNewFileAdded !=0 ) {
				//
				ifile_processing=i;
                OnNewFileAdded((TObject*) this );
            }

		}
	 }

	 /*
	 int ntx = compare_file_added(ssOld,ssNew);
	 if (ntx!=-1) {

		 String sNew = ssNew->Strings[ntx];
		 //m->Lines->Add("NEW FILE ADDED: "+sNew);
		 if (OnNewFileAdded !=0 ) {
		   ifile_processing=ntx;//@@now we pass the entire object itself
		   //     and saved this 'ntx' index in ifile_processing field
		   OnNewFileAdded((TObject*) this );

		 }
	 }
	 */
	 clear_file_info_strings(ssOld);//->Clear();
	 clear_file_info_data(ssNew);
	 ssOld->AddStrings(ssNew);
 }


}



AnsiString sqlDate(TDateTime dt )
{
 String t= "'"+FormatFloat("0000",YearOf(dt))+'-'+
  FormatFloat("00",MonthOf(dt))+'-'+
  FormatFloat("00",DayOf(dt))+"'";
 return t;
}

AnsiString sqlTime(TDateTime dt )
{
 String t= "'"+FormatFloat("00",HourOf(dt))+':'+
  FormatFloat("00",MinuteOf(dt))+':'+
  FormatFloat("00",SecondOf(dt))+"'";
 return t;
}


AnsiString surveyTime_NoQuotes(TDateTime dt )
{
 String t= ""+FormatFloat("00",HourOf(dt))+'-'+
  FormatFloat("00",MinuteOf(dt)) ;
  //FormatFloat("00",SecondOf(dt))+"'";
 return t;
}


AnsiString sqlDateTime(TDateTime dt )
{
 String t= "'"+FormatFloat("0000",YearOf(dt))+'-'+
  FormatFloat("00",MonthOf(dt))+'-'+
  FormatFloat("00",DayOf(dt))+" "
 +FormatFloat("00",HourOf(dt))+':'+
  FormatFloat("00",MinuteOf(dt))+':'+
  FormatFloat("00",SecondOf(dt))+"'";
 return t;
}

AnsiString sqlDateNoQuotes(TDateTime dt )
{
 String t= ""+FormatFloat("0000",YearOf(dt))+'-'+
  FormatFloat("00",MonthOf(dt))+'-'+
  FormatFloat("00",DayOf(dt))+"";
 return t;
}


int find_file_log(TADOQuery  * qOmni, String fn)
{
int id=-1;
  fn=ExtractFileName(fn);
  String sql=
  "SELECT id from files_log where FILENAME='"+fn+"'";
  qOmni->Close();
  qOmni->SQL->Text=sql;
  qOmni->Open();
  if (qOmni->RecordCount>0) {
	  id=qOmni->Fields->Fields[0]->AsInteger;
  }
  return id;
}

int select_surveys(TADOQuery  * qOmni, int CODE_MO, TDate dateBegin,TDate dateEnd, std::vector<TPersonInfo>& vp , TStrings* ssLog)
{
String sql=
"SELECT \
ROW_NUMBER() over (order by ID_PERSON asc,SDATE), \
	  s.[ID_PERSON],                              \
	  p.FAM,                                      \
	  p.IM,                                       \
	  isNull(p.OT,'') as OT,                                       \
	  p.W,                                        \
	  isNull(p.DR,'1900-1-1') as DR,                    \
	  p.NPOLIS,                                   \
	  s.DS,                                       \
	  s.SDATE,                                    \
	  s.EDATE,                                    \
	  s.SMOCOD,                                   \
	  s.DISP_TYP          \
  FROM [DB_SURVEYS].[dbo].[surveys] s left join [DB_SURVEYS].[dbo].persons p \
  on p.ID=s.ID_PERSON \
  where s.CODE_MO="+IntToStr(CODE_MO)+"      \
  and( s.SDATE between "+sqlDate(dateBegin)+" and "+sqlDate(dateEnd)+" \
  or s.EDATE between "+sqlDate(dateBegin)+" and  "+sqlDate(dateEnd)+" )";

  ssLog->Add(sql);

  qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->Open();

	vp.clear();
	while (!qOmni->Eof)
	{
	  TPersonInfo p;
	  p.lastname = qOmni->FieldValues["FAM"];
	  p.firstname = qOmni->FieldValues["IM"];
	  p.patronimic = qOmni->FieldValues["OT"];
	  p.sex=  qOmni->FieldValues["W"];

	  String sDT  =  (qOmni->FieldValues["DR"]);
	  TDateTime dt1 = qOmni->FieldByName("DR")->AsDateTime;
	  p.birth_date= dt1; //readSqlDateAsDT(sDT);

	  p.mscard_number= qOmni->FieldValues["NPOLIS"];
	  p.diagnosis=  qOmni->FieldValues["DS"];

	  sDT=   qOmni->FieldValues["SDATE"];
	  dt1 =   qOmni->FieldByName("SDATE")->AsDateTime;
	  p.start_date = dt1; //readSqlDateAsDT(sDT);

	  sDT =  qOmni->FieldValues["EDATE"];
	  dt1 =   qOmni->FieldByName("EDATE")->AsDateTime;
	  p.end_date = dt1;  //readSqlDateAsDT(sDT);

	  p.SMOCOD =qOmni->FieldValues["SMOCOD"];
	  p.SURVEY_TYPE  =qOmni->FieldValues["DISP_TYP"];


	  vp.push_back(p);
	  qOmni->Next();
	}




}


int generateResponsePacketNumber()
{
   TDateTime dt = iniSet->ReadDate("packet_number","date",Now());
   int num=1;
   if ( DateOf(dt)!=DateOf(Now()) )
   {
	  ini->WriteDate("packet_number","date",Now());
	  ini->WriteInteger("packet_number","val",1);
	  //return 1;
   }
   else
   {
	  num=ini->ReadInteger("packet_number","val",1);
	  if (num>=100) {
	   num=num % 100;
	  }
	  ini->WriteInteger("packet_number","val",num+1);
   }
   return num;
}


String makeResponseFileName( TReadFileInfo fi )
{
 String MO = "M"+IntToStr(fi.CODE_MO);
 String s="PM"; //"RR"; //@@changed 2020-05-=18
 String sdate;
 int y,m,d;
 y=YearOf(Now());
 m=MonthOf(Now());
 d=DayOf(Now());
 sdate.sprintf(L"%04d%02d%02d",y,m,d);
 int num= generateResponsePacketNumber();
 String res= s+"-"+MO+"-"+sdate+"-"+FormatFloat("00",num)+".xml";
 return res;


}


void surveys_to_xml(  std::vector<TPersonInfo> vp, String fn, TSurveyFileInfo sfi)
{
	TStringList *ss=new TStringList();
	ss->Add("<ZL_LIST>") ;
	ss->Add("");
	ss->Add(" <ZGLV>")   ;
	ss->Add("  <FILENAME>"+ExtractFileName(fn)+"</FILENAME>") ;   //sfi.FILENAME+
	ss->Add("  <DATA>"+sqlDateNoQuotes(sfi.dt_date)+"</DATA>") ;
	ss->Add("  <CODE_MO>"+IntToStr(sfi.CODE_MO)+"</CODE_MO>") ;
	ss->Add("  <DATA_OT>"+sqlDateNoQuotes(sfi.DATA_OT)+"</DATA_OT>") ;
	ss->Add("  <Z_NAME>"+ExtractFileName(sfi.ORIGINAL_FILENAME)+"</Z_NAME>") ;
	ss->Add(" </ZGLV>")  ;
	ss->Add("");
	for (int i = 0; i < vp.size(); i++)
	{
		TPersonInfo & p = vp[i];
		ss->Add(" <ZAP>")   ;
		ss->Add("  <N_ZAP>"+IntToStr(i+1)+"</N_ZAP>") ;
		ss->Add("  <FAM>"+p.lastname+"</FAM>") ;
		ss->Add("  <IM>"+p.firstname+"</IM>")  ;
		ss->Add("  <OT>"+p.patronimic+"</OT>") ;
		ss->Add("  <W>"+IntToStr(p.sex)+"</W>") ;
		ss->Add("  <DR>"+sqlDateNoQuotes(p.birth_date)+"</DR>") ;
		ss->Add("  <NPOLIS>"+(p.mscard_number)+"</NPOLIS>") ;
		ss->Add("  <DS>"+(p.diagnosis)+"</DS>") ;
		ss->Add("  <SDATE>"+sqlDateNoQuotes(p.start_date)+"</SDATE>") ;
		ss->Add("  <EDATE>"+sqlDateNoQuotes(p.end_date)+"</EDATE>") ;
		ss->Add("  <DISP_TYP>"+IntToStr(p.SURVEY_TYPE)+"</DISP_TYP>") ;
		ss->Add(" </ZAP>")   ;
		ss->Add("");
	}
	ss->Add("</ZL_LIST>") ;
	ss->SaveToFile(fn);
}



TTime hyphenStrToTime(AnsiString s)
{
	//
	int h,min;
	sscanf(s.c_str(),"%02d-%02d",&h,&min);
	TTime t=	EncodeTime(h,min,0,0);
	return t;

}


TTime hyphenStrToDate(AnsiString s)
{
	//
	int y,m,d;
	sscanf(s.c_str(),"%04d-%02d-%02d",&y,&m,&d);
	TDate t=	EncodeDate(y,m,d);
	return t;

}



void TPersonInfoHosp::add_error(int code, String desc)
{
  TErrorLogInfo e;
  e.code=code;

  elvErrors.push_back(e);

}



String elv_errors_to_str(std::vector<TErrorLogInfo> elvErrors)
{
   String s="'";
   for (int i = 0; i < elvErrors.size(); i++)
   {
	 if ( s.Length()<240 ) { //250 length of field

	 }
	 s+= ";"+IntToStr(elvErrors[i].code);
   }
   if (elvErrors.size()>0) {
	 s=s+";";
   }
   return s+"'"; //@@01.09.2020 - added ; in the end of string ;101;201;
}






//////////////////////
////////////////////


int select_surveys(TADOQuery *qOmni, int CODE_MO, TDate dt1, TDate dt2  )
{
     String sql=
	 "select * from surveys where SDATE between "+sqlDate(dt1)+" and "+sqlDate(dt2)
	 +" or EDATE between " +sqlDate(dt1) +" and "+sqlDate(dt2)
	 +" and CODE_MO="+IntToStr(CODE_MO)
	 ;
	  qOmni->Close();
	 qOmni->SQL->Text=sql;
	 qOmni->Open();
	 if (qOmni->RecordCount>0) {
		 //
		 for (int i = 0; i <qOmni->RecordCount ; i++) {

            qOmni->Next();
		 }
	 }
	 return  qOmni->RecordCount;
}


int find_survey(   TADOQuery *qOmni,
	  TDate SDATE     ,
	  TDate EDATE     ,
	  int SMOCOD      ,
	  int DISP_TYP    ,
	  int CODE_MO      ,
	  String DS         ,
	  int ID_PERSON
	  )
{
	 String sql=
	 "select * from surveys where SDATE="+sqlDate(SDATE)
	 +" and EDATE=" +sqlDate(EDATE)
	 +" and SMOCOD="+IntToStr(SMOCOD)
	 +" and DISP_TYP="+IntToStr(  DISP_TYP)
	 +" and CODE_MO="+IntToStr(CODE_MO)
	 +" and DS='"+DS+"' and ID_PERSON="+IntToStr(ID_PERSON);
	 qOmni->Close();
	 qOmni->SQL->Text=sql;
	 qOmni->Open();
	 if (qOmni->RecordCount>0) {
	  int id =qOmni->FieldValues["ID"];
	  return id;
	 }else{
      return -1;
     }
}



int add_survey(   TADOQuery *qOmni,
	  TDate SDATE     ,
	  TDate EDATE     ,
	  int SMOCOD      ,
	  int DISP_TYP    ,
	  int CODE_MO      ,
	  String DS         ,
	  int ID_FILE        ,
	  int ID_PERSON       ,
	  int N_REC
)
{
	//
	String sql=
	"insert into surveys ([SDATE] \
	  ,[EDATE]                     \
	  ,[SMOCOD]                     \
	  ,[DISP_TYP]                    \
	  ,[CODE_MO]                      \
	  ,[DS]                            \
	  ,[ID_FILE]                        \
	  ,[ID_PERSON]                       \
	  ,[N_REC] ) values("
  +	  sqlDate(SDATE)          +" , "
  +	  sqlDate(EDATE)          +" , "
  +	  IntToStr(SMOCOD)        +" , "
  +	  IntToStr(DISP_TYP)      +" , "
  +	  IntToStr(CODE_MO)       +" , "
  +   "'"+DS    +"' , "
  +	  IntToStr(ID_FILE)       +" , "
  +	  IntToStr(ID_PERSON)     +" , "
  +	  IntToStr(N_REC)      +" ) ";
	qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->ExecSQL();
	int id= scopeIdentity( qOmni);
	return id;
}

int add_file(               // for Surveys system, similar func exists in Hospitalization called hosp_file_header_to_log
	   TADOQuery* qOmni,
	   String FILENAME,
	   String DATA,
	   String DATA_OT,
	   int REC_AMOUNT,
	   int FILE_TYPE,
	   TDateTime PROC_DT,
	   int CODE_MO,
	   int ERROR_CODE )
{
	//
	String code_mo = FormatFloat("0000", CODE_MO);
	int nloops=0;
	while (code_mo.Length()>4)
	{
	  code_mo.Delete(1,1);
	  if (++nloops > 20) break;
	}

	FILENAME=ExtractFileName(FILENAME);
	String sql=
	"insert into files_log (FILENAME,   DATA,     DATA_OT,  REC_AMOUNT, FILE_TYPE, PROC_DT,CODE_MO,ERROR_CODE) values( \
	'"+FILENAME+"',"+sqlDateTime(/*DATA*/Now())+","+sqlDate(DATA_OT)+","+IntToStr(REC_AMOUNT)+", "+IntToStr(FILE_TYPE)+", "+sqlDateTime(PROC_DT)
	 +",     "+
	 //IntToStr(CODE_MO) // for Hospitalization we made this field nvarchar(20), therefore:
	 "'"+code_mo+"'"

	 +","+IntToStr(ERROR_CODE)+"  ) ";

	qOmni->Close();
	qOmni->SQL->Text=sql;
	qOmni->ExecSQL();
	int id= scopeIdentity( qOmni);
	return id;

}





int fileNameInspect(String fname, TStrings * ss, TSurveyFileInfo & fi)
{
   //
   int err=0;

   fi.ORIGINAL_FILENAME=ExtractFileName(fname);


   String ext=  (ExtractFileExt(fname));
   if (ext.LowerCase()!=".xml")
   {
	  err|=1<<3;
   }




   String sshort= ExtractFileName(fname);

   String sFileType = sshort.SubString(1,2);
   fi.sFileType=sFileType;

   if (sFileType[1]=='T') {
	   int res=fileNameInspect_T( fname,  ss,  fi);
	   return res;
   }

   sshort.Delete(1,3);
   fi.sOrganisationType=sshort.SubString(1,1);
   if (sshort[1] !='M') {
	   ss->Add("not M");
       err|=1<<0;
   }
   else
   {
	   ss->Add("start M");
   }

   sshort.Delete(1,1);
   String mcode = sshort.SubString(1,6);
   ss->Add("code "+mcode);
   if (mcode.Length()!=6) {
	   err|=1<<1;
   }
   fi.CODE_MO=StrToIntDef(mcode,0);

   sshort.Delete(1,7);
   String sy = sshort.SubString(1,4);
   ss->Add("year "+sy);
   if (sy.Length()!=4) {
	  err|=1<<2;
   }

   sshort.Delete(1,4)    ;
   String smon = sshort.SubString(1,2);
   ss->Add("month "+smon);

   sshort.Delete(1,2)    ;
   String sday = sshort.SubString(1,2);
   ss->Add("day "+sday);

   sshort.Delete(1,3)    ; // -
   String spack = sshort.SubString(1,2);
   ss->Add("pack "+spack);

   int y,m,d,pack;
   y=StrToIntDef(sy,0);
   m=StrToIntDef(smon,0);
   d=StrToIntDef(sday,0);
   pack=StrToIntDef(spack,0);
   try{
   TryEncodeDate(y,m,d, fi.dt_date);
   }catch(...){};


   return err;
}





