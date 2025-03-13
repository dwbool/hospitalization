object fmSettings: TfmSettings
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Settings'
  ClientHeight = 470
  ClientWidth = 738
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BitBtn1: TBitBtn
    Left = 224
    Top = 412
    Width = 131
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 416
    Top = 412
    Width = 131
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = BitBtn2Click
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 16
    Width = 710
    Height = 105
    Caption = 'Survey ( "dispensarizaci'#243'n" )'
    TabOrder = 2
    object Label1: TLabel
      Left = 32
      Top = 55
      Width = 114
      Height = 13
      Caption = 'Incoming Files Directory'
    end
    object SpeedButton1: TSpeedButton
      Left = 671
      Top = 51
      Width = 23
      Height = 22
      Caption = '...'
      OnClick = SpeedButton1Click
    end
    object edIncomingDir: TEdit
      Left = 192
      Top = 52
      Width = 473
      Height = 21
      TabOrder = 0
    end
    object chSurveyRun: TCheckBox
      Left = 32
      Top = 21
      Width = 281
      Height = 17
      Caption = 'Survey ( "dispensarizaci'#243'n" ) files process'
      TabOrder = 1
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 127
    Width = 710
    Height = 114
    Caption = 'Hospitalization'
    TabOrder = 3
    object Label4: TLabel
      Left = 31
      Top = 61
      Width = 114
      Height = 13
      Caption = 'Incoming Files Directory'
    end
    object SpeedButton4: TSpeedButton
      Left = 671
      Top = 58
      Width = 23
      Height = 22
      Caption = '...'
      OnClick = SpeedButton3Click
    end
    object edIncomingDirHosp: TEdit
      Left = 190
      Top = 58
      Width = 475
      Height = 21
      TabOrder = 0
    end
    object chHospRun: TCheckBox
      Left = 32
      Top = 25
      Width = 233
      Height = 17
      Caption = 'Hospitalization files process'
      TabOrder = 1
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 248
    Width = 705
    Height = 158
    Caption = 'General Settings'
    TabOrder = 4
    object Label2: TLabel
      Left = 32
      Top = 34
      Width = 120
      Height = 13
      Caption = 'Processed Files Directory'
    end
    object SpeedButton2: TSpeedButton
      Left = 671
      Top = 25
      Width = 23
      Height = 22
      Caption = '...'
      OnClick = SpeedButton2Click
    end
    object Label3: TLabel
      Left = 32
      Top = 65
      Width = 122
      Height = 13
      Caption = 'Outcoming Files Directory'
    end
    object SpeedButton3: TSpeedButton
      Left = 671
      Top = 64
      Width = 23
      Height = 22
      Caption = '...'
      OnClick = SpeedButton3Click
    end
    object edProcessedDir: TEdit
      Left = 192
      Top = 26
      Width = 473
      Height = 21
      TabOrder = 0
    end
    object edOutcomingDir: TEdit
      Left = 192
      Top = 65
      Width = 473
      Height = 21
      TabOrder = 1
    end
    object chReport: TCheckBox
      Left = 32
      Top = 112
      Width = 153
      Height = 17
      Caption = 'Make XML Reports at'
      TabOrder = 2
    end
    object dpReport: TDateTimePicker
      Left = 191
      Top = 109
      Width = 186
      Height = 21
      Date = 44111.416666666660000000
      Time = 44111.416666666660000000
      Kind = dtkTime
      TabOrder = 3
    end
  end
end
