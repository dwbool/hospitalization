object fmDatabase: TfmDatabase
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Database'
  ClientHeight = 222
  ClientWidth = 803
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 29
    Width = 84
    Height = 13
    Caption = 'Connection string'
  end
  object lbStatus: TLabel
    Left = 74
    Top = 87
    Width = 47
    Height = 13
    Caption = 'lbStatus'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btBuild: TSpeedButton
    Left = 616
    Top = 75
    Width = 169
    Height = 25
    Caption = 'Build string'
    OnClick = btBuildClick
  end
  object Label2: TLabel
    Left = 10
    Top = 87
    Width = 31
    Height = 13
    Caption = 'Status'
  end
  object btRecreateTables: TSpeedButton
    Left = 536
    Top = 136
    Width = 233
    Height = 25
    Caption = 'Recreate tables'
    Visible = False
    OnClick = btRecreateTablesClick
  end
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 293
    Height = 13
    Caption = 'Stored as:  survey_info.ini / [database] / connection_string='
  end
  object edConn: TComboBox
    Left = 8
    Top = 48
    Width = 777
    Height = 21
    TabOrder = 0
    Text = 'edConn'
  end
  object btConnect: TButton
    Left = 8
    Top = 136
    Width = 113
    Height = 25
    Caption = 'Connect'
    TabOrder = 1
    OnClick = btConnectClick
  end
  object btDisconnect: TButton
    Left = 160
    Top = 136
    Width = 129
    Height = 25
    Caption = 'Disconnect'
    TabOrder = 2
    OnClick = btDisconnectClick
  end
  object btExit: TButton
    Left = 328
    Top = 136
    Width = 97
    Height = 25
    Caption = 'Exit'
    TabOrder = 3
    OnClick = btExitClick
  end
  object qCreate: TADOQuery
    Connection = DataModuleBase.ADOConnection
    CursorType = ctStatic
    Parameters = <>
    SQL.Strings = (
      
        'select d.ID,d.Number,u.FIO as name,d.Summ,d.CreatedDate , u.ID a' +
        's User_ID'
      'from tDocument d'
      'left join tUsers u on u.ID=d.AuthorId'
      'order by d.CreatedDate'
      '')
    Left = 616
    Top = 160
    object qCreateID: TAutoIncField
      DisplayLabel = 'ID '#1076#1086#1082#1091#1084#1077#1085#1090#1072
      FieldName = 'ID'
      ReadOnly = True
    end
    object qCreateNumber: TWideStringField
      DisplayLabel = #1053#1086#1084#1077#1088' '#1076#1086#1082#1091#1084#1077#1085#1090#1072
      DisplayWidth = 20
      FieldName = 'Number'
      Size = 100
    end
    object qCreatename: TWideStringField
      DisplayLabel = #1048#1084#1103' '#1072#1074#1090#1086#1088#1072
      DisplayWidth = 30
      FieldName = 'name'
      Size = 255
    end
    object qCreateSumm: TBCDField
      DisplayLabel = #1057#1091#1084#1084#1072
      DisplayWidth = 15
      FieldName = 'Summ'
      DisplayFormat = '# ### ###.00'
      Precision = 19
    end
    object qCreateCreatedDate: TDateTimeField
      DisplayLabel = #1044#1072#1090#1072' '#1089#1086#1079#1076#1072#1085#1080#1103
      FieldName = 'CreatedDate'
    end
    object qCreateUser_ID: TAutoIncField
      FieldName = 'User_ID'
      ReadOnly = True
      Visible = False
    end
  end
end
