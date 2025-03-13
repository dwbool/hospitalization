//---------------------------------------------------------------------------

#ifndef hosp_utilsH
#define hosp_utilsH
#include "survey_utils.h"
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
//---------------------------------------------------------------------------

#define ERROR_INCORRECT_FILENAME 50
#define ERROR_FILENAME_NOT_MATCH 51
#define ERROR_GENERAL_NO_HEADER 53
#define ERROR_GENERAL_NO_RECORDS 54
#define ERROR_GENERAL_ROOT_MISSING 52
#define ERROR_GENERAL_PARSER_EXCEPTION 55

int process_hosp_xml_t1(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T1> &persons, TStrings * ssLog , TADOQuery*q);

int process_hosp_xml_t2(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T2> &persons, TStrings * ssLog , TADOQuery*q);

int process_hosp_xml_t3(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T3> &persons, TStrings * ssLog , TADOQuery*q);

int process_hosp_xml_t4(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T4> &persons, TStrings * ssLog , TADOQuery*q);

int process_hosp_xml_t5(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T5> &persons, TStrings * ssLog , TADOQuery*q);

int process_hosp_xml_t6(String fn, THospFileInfo_T1 &file_info, std::vector<TPersonInfoHosp_T6> &persons, TStrings * ssLog , TADOQuery* q);



int hosp_to_base_t1(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T1> persons, TStrings * ssLog );

int hosp_to_base_t2(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T2> persons, TStrings * ssLog );

int hosp_to_base_t3(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T3> persons, TStrings * ssLog );

int hosp_to_base_t4(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T4> persons, TStrings * ssLog );
int hosp_to_base_t5(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T5> persons, TStrings * ssLog );

int hosp_to_base_t6(TADOQuery*q, THospFileInfo_T1 file_info, std::vector<TPersonInfoHosp_T6> persons, TStrings * ssLog );

void getRequiredFields(TADOQuery* q, int n, std::vector<String> & res );


void vect_to_ss(std::vector<String> a, TStrings* ss );

void getRequiredFields(TADOQuery* q, int n, std::vector<String> & res,
//std::vector<int> &err_codes );
 std::map<String,int>& mpErrorsRequired );


void make_error_report_T1( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T1> & persons, TStrings* ss  , TADOQuery* q);
void make_error_report_T2( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T2> & persons, TStrings* ss  , TADOQuery* q);
void make_error_report_T3( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T3> & persons, TStrings* ss , TADOQuery* q);
void make_error_report_T4( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T4> &persons, TStrings* ss , TADOQuery* q);
void make_error_report_T5( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T5> &persons, TStrings* ss , TADOQuery* q);
void make_error_report_T6( THospFileInfo_T1 fi, std::vector<TPersonInfoHosp_T6> &persons, TStrings* ss , TADOQuery* q);



struct TReferenceInfo{
   int err_code;
   String defined_as;
   String field       ;
   String operator1 ;
   String arg1      ;
   String operator2 ;
   String arg2      ;
   String comment      ;


};

void getReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo> & vref_inf /*, String operat_*/);



int performReferenceCheckings(TADOQuery * q, std::vector<TReferenceInfo>  vref_inf, String xml_field, String value, int val_type, int & vref_inf_index, String & error_message , String & comment);



void getDateTimeCheckings(TADOQuery * q, std::vector<TReferenceInfo> & vref_inf/*, String operat_ */);

int performDateTimeCheckings(
   TADOQuery * q,
   std::vector<TReferenceInfo>  vref_inf,
   String xml_field,
   String value,
   int val_type,
   std::vector<TNameValueInfo> nvvReceived,
   int & vref_inf_index,
   String & error_message
	);



void make_error_report_wrong_file( TSurveyFileInfo fi, TStrings* ss );

int identCurrent(TADOQuery * q, String tab_name);

//void make_error_report_wrong_file( TSurveyFileInfo fi, TStrings* ss )



String str_limit_len(String s,int len);


String str_limit_len_quot(String s,int len);


int fileNameInspect_T(String fname, TStrings * ss, TSurveyFileInfo & fi);



String find_or_create_dir(String sDirOutcome, int CODE_MO);
String find_or_create_dir_str(String sDirOutcome, String CODE_MO);

void delay(int ms);

void add_slash(String & s);



#endif
