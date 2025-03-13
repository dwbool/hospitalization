object DataModuleBase: TDataModuleBase
  OldCreateOrder = False
  Height = 381
  Width = 642
  object ADOConnection: TADOConnection
    ConnectionString = 
      'Provider=SQLOLEDB.1;Password=1234;Persist Security Info=True;Use' +
      'r ID=sa;Initial Catalog=DB_SURVEYS;Data Source=(local);Use Proce' +
      'dure for Prepare=1;Auto Translate=True;Packet Size=4096;Workstat' +
      'ion ID=ASU08;Use Encryption for Data=False;Tag with column colla' +
      'tion when possible=False'
    LoginPrompt = False
    Provider = 'SQLOLEDB.1'
    Left = 40
    Top = 24
  end
end
