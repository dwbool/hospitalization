unit file_dir_funcs;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, fileCtrl, ShellAPI;

function packFilesInDirectory_rar(sDir:String; sRarFileName:String):Integer;

procedure SearchInDir_Subdirs(s,mask:String;  ss:TStrings );

procedure SearchInDir_Subdirs_file(sDir:String;  fname:String; ssFilesWithPath:TStrings );

function FilesContentIsSame(file1,file2:String):Boolean;

function FilesContentIsSame_FieldsCompare(file1,file2:String):Boolean;

//function ValBetweenTags(S, sBeg,sEnd:String):String;

function OrbitaToDateTime(s:String):TDateTime;
procedure SearchInDir_files_only(s,mask:String;  ss:TStrings );
procedure SearchInDir_directories_only(s,mask:String;  ss:TStrings );


implementation

uses ats_utils;



function OrbitaToDateTime(s:String):TDateTime;
var p1:Integer;  sy,sm,sd, sh,smin,ssec:String;
 y,m,d,h,min,sec:Integer;    dt,dtTime:TDateTime;

begin
  //        Orbita Date Time example    2014-05-29 16:00:16
  s:=trim(s) ;
  Result:=0;
  p1:=pos( '-',s);
  if p1>0 then
   begin
     sy:=trim(Copy(s,1,p1-1));
     Delete(s,1,p1);
   end;
  p1:=pos( '-',s);
  if p1>0 then
   begin
     sm:=trim(Copy(s,1,p1-1));
     Delete(s,1,p1);
   end;
  p1:=pos( ' ',s);
  if p1>0 then
   begin
     sd:=trim(Copy(s,1,p1-1));
     Delete(s,1,p1);
   end
  else
   begin
     sd:=trim(s);
     sh:='0';
     smin:='0';
     ssec:='0';
   end;
  ///
  p1:=pos( ':',s);
  if p1>0 then
   begin
     sh:=Copy(s,1,p1-1);
     Delete(s,1,p1);
   end;
  p1:=pos( ':',s);
  if p1>0 then
   begin
     smin:=Copy(s,1,p1-1);
     Delete(s,1,p1);
   end;
  ssec:=trim((s));
  if ssec='' then ssec:='0';
  ////
  y:=strtointdef(sy,-1);
  if (y<100) then y:=y+2000;
  m:=strtointdef(sm,-1);
  d:=strtointdef(sd,-1);
  h:=strtointdef(sh,-1);
  min:=strtointdef(smin,-1);
  sec:=strtointdef(ssec,-1);
  if (y>-1)and(m>-1)and(d>-1)and(h>-1)and(min>-1)and(sec>-1) then
   begin
     //
     try

      dt:=EncodeDate(    y,m,d )  ;
      dtTime:=EncodeTime( h,min,sec,0 );
      dt:= Trunc(dt) + Frac(dtTime);

     except
      dt:=0;
     end;
     Result:=dt;
   end;
end;



function ValBetweenTags(S, sBeg,sEnd:String):String;
var p1,p2,l1,l2:Integer; sv:String;
begin
result:='';
p1:=Pos(sBeg,s);
p2:=Pos(sEnd,s);
if( p1>0) and (p2>0) then
  begin
   //
   l1:=Length(sbeg);
   l2:=length(send);
   sv:=Copy(s,p1+l1, p2-(p1+l1));
   result:=sv;
  end;

end;


function FilesContentIsSame_FieldsCompare(file1,file2:String):Boolean;
var ss1,ss2:TStringList; surname1,surname2, datefrom1,datefrom2, dateto1,dateto2, txt1,txt2:String;
begin
result:=false;
try
 ss1:=TStringList.Create;
 ss2:=TStringList.Create;
 ss1.LoadFromFile(file1);
 ss2.LoadFromFile(file2);
 txt1:=AnsiLowerCase(ss1.Text);
 txt2:=AnsiLowerCase(ss2.Text);

 surname1 :=ValBetweenTags(txt1,'<lastname>','</lastname>');
 datefrom1 :=ValBetweenTags(txt1,'<datefrom>','</datefrom>');
 dateto1 :=ValBetweenTags(txt1,'<dateto>','</dateto>');

 surname2 :=ValBetweenTags(txt2,'<lastname>','</lastname>');
 datefrom2 :=ValBetweenTags(txt2,'<datefrom>','</datefrom>');
 dateto2 :=ValBetweenTags(txt2,'<dateto>','</dateto>');

 Result:= ( surname1=  surname2 )  and  (  datefrom1=   datefrom2  )  and (dateto1=  dateto2)   ;
except

end;

end;



function FilesContentIsSame(file1,file2:String):Boolean;
var ms1,ms2:TMemoryStream; sz,i:Integer;
label l_end;
begin
 Result:=false;
 try
 ms1:=TMemoryStream.Create;
 ms1.LoadFromfile(file1);
 ms2:=TMemoryStream.Create;
 ms2.LoadFromfile(file2);


 if ms1.Size<>ms2.Size then goto l_end;
 ms1.Position:=0;
 ms2.Position:=0;
 Result:=CompareMem(ms1.Memory,ms2.Memory,ms1.Size);


 l_end:;
 ms1.free;
 ms2.free;
 except

 end;
end;



procedure SearchInDir_Subdirs_file(sDir:String;  fname:String; ssFilesWithPath:TStrings );
var  ss:TStringList; i:Integer; fx:String;
begin
ssFilesWithPath.Clear;
  ss:=TStringList.create;
  SearchInDir_Subdirs(sDir,'*',ss);
  if sdir[Length(sdir)]<>'\' then sdir:=sdir+'\';
  for i := 0 to ss.Count-1 do
    begin
      //
      if ss[i][Length(ss[i])]<>'\' then ss[i]:=ss[i]+'\';

      fx:= sDir+ss[i] + fname;
      if fileExists(fx) then
       begin

        ssFilesWithPath.Add(fx);
       end;



    end;



end;


procedure SearchInDir_Subdirs(s,mask:String;  ss:TStrings );
var res:Integer; dir,ext:String; r:TSearchRec;
begin
 ss.Clear;
 try
 FillChar(r,sizeof(r),0);

  try
  res:=FindFirst(s+'\'+mask,-1,r);
  while res=0 do
   begin
   if (r.Name<>'.')and(r.Name<>'..' ) then
   if (r.Attr and faDirectory)>0 then
    begin
     // if wanna process subdirectory
     ss.Add(r.Name);
    end
   else
    begin
    ext:=AnsiLowerCase(ExtractFileExt(r.Name));
    //if (ext='.tsk')or(ext='.tsk') then   // graphic file
      begin
      dir:=trim(s);
      if dir>'' then
       begin
        if dir[Length(dir)]<>'\' then dir:=dir+'\';
        try
         //proc(dir+r.Name,ext,r);   // display file
        except
        end;
       end;
      end;
    end;
   //if Perform then break; // new directory clicked
   res:=FindNext(r);
   end;

  finally
  FindClose(r);
  end;

 except
 end;

end;




procedure SearchInDir_files_only(s,mask:String;  ss:TStrings );
var res:Integer; dir,ext:String; r:TSearchRec;   rnew:^TSearchRec;
begin
 ss.Clear;
 try
 FillChar(r,sizeof(r),0);
 if s[Length(s)]<>'\' then s:=s+'\';
 
  try
  res:=FindFirst(s+''+mask,-1,r);
  while res=0 do
   begin
   if (r.Name<>'.')and(r.Name<>'..' ) then
   if (r.Attr and faDirectory)=0 then
   //if True then
    begin
     // if wanna process subdirectory
     //ss.Add(r.Name);
     rnew:=nil;
     ////////////////////////////// if we make structures in the list
     //New(rnew);
     //CopyMemory(rnew, @r, sizeof(r));
     /////////////////////////////////////////////
     ss.AddObject(s+''+r.Name,TObject(rnew));
    end
   else
    begin
    ext:=AnsiLowerCase(ExtractFileExt(r.Name));
    //if (ext='.tsk')or(ext='.tsk') then   // graphic file
      begin
      dir:=trim(s);
      if dir>'' then
       begin
        if dir[Length(dir)]<>'\' then dir:=dir+'\';
        try
         //proc(dir+r.Name,ext,r);   // display file
        except
        end;
       end;
      end;
    end;
   //if Perform then break; // new directory clicked
   res:=FindNext(r);
   end;

  finally
  FindClose(r);
  end;

 except
 end;

end;




procedure SearchInDir_directories_only(s,mask:String;  ss:TStrings );
var res:Integer; dir,ext:String; r:TSearchRec;  rnew:^TSearchRec;
begin
 ss.Clear;
 try
 FillChar(r,sizeof(r),0);

  try
  res:=FindFirst(s+'\'+mask,-1,r);
  while res=0 do
   begin
   if (r.Name<>'.')and(r.Name<>'..' ) then
   if (r.Attr and faDirectory)>0 then
   //if True then
    begin
     // if wanna process subdirectory
     New(rnew);
     CopyMemory(rnew, @r, sizeof(r));
     ss.AddObject(r.Name,TObject(rnew));
    end
   else
    begin
    ext:=AnsiLowerCase(ExtractFileExt(r.Name));
    //if (ext='.tsk')or(ext='.tsk') then   // graphic file
      begin
      dir:=trim(s);
      if dir>'' then
       begin
        if dir[Length(dir)]<>'\' then dir:=dir+'\';
        try
         //proc(dir+r.Name,ext,r);   // display file
        except
        end;
       end;
      end;
    end;
   //if Perform then break; // new directory clicked
   res:=FindNext(r);
   end;

  finally
  FindClose(r);
  end;

 except
 end;

end;







function packFilesInDirectory_rar(sDir:String; sRarFileName:String):Integer;
var cmd,log:String; t1,dt:Cardinal;  size:Integer;  found:Boolean;
begin

 Result:=0;
 log:=sDir;
 if log[Length(log)]<>'\' then log:=log+'\';
 log:=log+ExtractFilename(ChangeFileExt(sRarFileName,'.txt'));

 DeleteFile(sRarFileName);

 //              -r             >enlisted.log
 cmd:=Format(' a -r -ep1 -c- "%s" "%s" ', [sRarFileName,  sDir]);

 ShellExecute(0,'open','rar',PChar(cmd),'',0);//winrar


 t1:=GetTickCount;
 while (GetTickCount-t1)< 60000 do
  begin
   //  function FindFileDateTimeAndSize(fname:String; var size:Integer):TDateTime;
   dt:=(GetTickCount-t1);
   Application.ProcessMessages;
   if FileExists(sRarFileName ) and (dt>10000) then
    begin
      FindFileDateTimeAndSize( sRarFileName,size);
      if size>0 then
        begin
          Result:=1;
          break;
        end;
    end;
  end; //while
 //



end;


end.
