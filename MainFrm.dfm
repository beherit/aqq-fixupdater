object MainForm: TMainForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'FixUpdater - ustawienia'
  ClientHeight = 268
  ClientWidth = 417
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001002000680400001600000028000000100000002000
    000001002000000000004004000000000000000000000000000000000000FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF002195D7121D8F
    D4931989D209FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00279EDB17259BDA84309FDCFF70C5
    E9FF1E90D5DD1A8AD23BFFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF002BA3DE2A39A8E0FF82CEEEFFCCF8FCFFD4FF
    FFFF91D9F1FF45AADEFF1A8BD24EFFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF002EA8E11439ACE2FF90D3F0FFDEFCFEFFAFFEFFFF6FFE
    FFFFD1FCFEFF99DBF3FF51AFE1FF1B8CD354FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0030ABE2FF81CEEEFFD7F5FCFF84E9F9FF8BFAFEFF7AFC
    FEFF59E9F9FFBFEFFBFF99D7F2FF39A1DCFF1B8DD328FFFFFF00FFFFFF00FFFF
    FF00FFFFFF0033B0E58F77CBEDFFD8F4FCFF84DEF6FF79E0F7FF90F4FCFF85F8
    FDFF55DDF6FF3AC9F0FFC4EDFAFF7DC7EBFF2093D6D3FFFFFF00FFFFFF00FFFF
    FF00FFFFFF0035B2E6FFD6F4FBFFA0E5F8FF72D9F5FF78DCF6FF95F0FBFF8BF4
    FCFF56D7F4FF3FC8F0FF38C2EFFFC3ECFAFF34A1DDFF2194D71CFFFFFF00FFFF
    FF00FFFFFF0037B5E7FFDDF6FDFF7DDCF6FF78DBF5FF7BDDF6FF9BEEFBFF94F2
    FCFF5BD6F4FF45C9F1FF38C3EFFFC5EDFAFF41A8E0FF2398D92DFFFFFF00FFFF
    FF00FFFFFF0038B7E8FFDFF7FDFF83DFF7FF7EDCF6FF81DEF7FFA3EEFBFF9EF2
    FCFF62D6F5FF4ACCF2FF3FC6F0FFC7EEFBFF40AAE0FF259BDA28FFFFFF00FFFF
    FF00FFFFFF003ABAE9FFE0F8FDFF89E1F7FF84DFF7FF88E0F7FFACEEFBFFA7F1
    FCFF69D7F4FF52CDF2FF46C8F0FFC8EEFBFF43ADE2FF279EDC2AFFFFFF00FFFF
    FF00FFFFFF003BBCEAFFE2F8FDFF8FE4F8FF8AE1F7FF90E2F7FFE6F7FDFFEBF9
    FEFF71DAF5FF5AD0F3FF4DCCF1FFCBEFFBFF35A8E0FF29A1DD11FFFFFF00FFFF
    FF003EBFEC0D46C1ECFFE4F9FDFFE2F8FDFFD5F5FCFFAFEAF9FFE8F8FDFFECF9
    FEFF89E1F7FFB5EAFAFFD0F1FBFFCDF0FBFF57BAE7FF2BA3DE45FFFFFF00FFFF
    FF003FC1EDFF8CDBF4FF61CAF0FF76D1F2FFA5E1F6FFE7F9FEFFF3FCFEFFF1FC
    FEFFE0F7FDFFA6E0F5FF74CAEDFF4DB8E7FF8DD4F0FF2DA6E0FFFFFFFF00FFFF
    FF0040C3EEFF3FC1EDFF3EC0EC363DBEEC583CBCEB9F3BBBEAFF88D5F2FF87D4
    F1FF36B4E7FF35B2E6B233B0E56932AEE42F30ABE2F52FA9E1FFFFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF003BBBEAD539B9
    E9FFFFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 0
    Top = 231
    Width = 417
    Height = 38
    Shape = bsTopLine
  end
  object SaveButton: TButton
    Left = 334
    Top = 237
    Width = 75
    Height = 25
    Caption = 'Zapisz'
    TabOrder = 1
    OnClick = SaveButtonClick
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 401
    Height = 217
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Kana'#322'y aktualizacji'
      object AddButton: TButton
        Left = 6
        Top = 160
        Width = 75
        Height = 25
        Caption = 'Dodaj'
        TabOrder = 1
        OnClick = AddButtonClick
      end
      object DeleteButton: TButton
        Left = 87
        Top = 160
        Width = 75
        Height = 25
        Caption = 'Usu'#324
        Enabled = False
        TabOrder = 2
        OnClick = DeleteButtonClick
      end
      object EditButton: TButton
        Left = 168
        Top = 160
        Width = 75
        Height = 25
        Caption = 'Edytuj'
        Enabled = False
        TabOrder = 0
        OnClick = EditButtonClick
      end
      object ResetButton: TButton
        Left = 310
        Top = 160
        Width = 75
        Height = 25
        Caption = 'Domy'#347'lne'
        TabOrder = 3
        OnClick = ResetButtonClick
      end
      object UrlListPreview: TListView
        Left = 6
        Top = 4
        Width = 379
        Height = 150
        Checkboxes = True
        Columns = <
          item
            Caption = 'Aktywne'
            Width = 68
          end
          item
            AutoSize = True
            Caption = 'Adres'
          end>
        Items.ItemData = {
          03C2010000040000000000000001000000FFFFFFFF01000000FFFFFFFF000000
          00002768007400740070003A002F002F0062006500680065007200690074002E
          0070006C002F006100710071005F007500700064006100740065002F00730074
          00610062006C0065002E0078006D006C000000000000000000FFFFFFFF010000
          00FFFFFFFF00000000002568007400740070003A002F002F0062006500680065
          007200690074002E0070006C002F006100710071005F00750070006400610074
          0065002F0062006500740061002E0078006D006C000000000001000000FFFFFF
          FF01000000FFFFFFFF00000000003368007400740070003A002F002F00610071
          0071006E006500770073002E006B006F006D0075006E0069006B00610074006F
          00720079002E0070006C002F0050006C0069006B0069002F006100710071005F
          007500700064006100740065002E0078006D006C000000000000000000FFFFFF
          FF01000000FFFFFFFF00000000002868007400740070003A002F002F00770077
          0077002E007A0079006C006200650072002E0069006E0066006F002D0073002E
          0070006C002F006100710071002F0062006500740061002E0078006D006C00FF
          FFFFFFFFFFFFFF}
        ReadOnly = True
        RowSelect = True
        TabOrder = 4
        ViewStyle = vsReport
        OnChange = UrlListPreviewChange
        OnClick = UrlListPreviewClick
        OnSelectItem = UrlListPreviewSelectItem
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Cz'#281'stotliwo'#347#263' aktualizacji'
      ImageIndex = 1
      object UpdateInfoLabel: TLabel
        Left = 17
        Top = 134
        Width = 351
        Height = 39
        Alignment = taCenter
        Caption = 
          'Ustawienie cz'#281'stotliwo'#347'ci sprawdzania aktualizacji przez wtyczk'#281 +
          ' jest niezale'#380'ne od ustawie'#324' w AQQ. Spradzenie czy s'#261' dost'#281'pne a' +
          'ktualizacje nie zostanie przeprowadzone gdy AQQ b'#281'dziie zablokow' +
          'ane!'
        Enabled = False
        WordWrap = True
      end
      object UpdateModeLabel: TLabel
        Left = 8
        Top = 8
        Width = 248
        Height = 13
        Caption = 'Sprawdzaj dost'#281'pno'#347#263' aktualizacji AQQ i dodatk'#243'w:'
      end
      object LastUpdateInfoLabel: TLabel
        Left = 8
        Top = 55
        Width = 147
        Height = 13
        Caption = 'Data ostatniego sprawdzania -'
      end
      object LastUpdateLabel: TLabel
        Left = 158
        Top = 55
        Width = 119
        Height = 13
        Caption = 'RRRR-MM-DD HH:MM:SS'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGreen
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentColor = False
        ParentFont = False
      end
      object Bevel: TBevel
        Left = 4
        Top = 118
        Width = 386
        Height = 3
        Shape = bsTopLine
      end
      object UpdateModeComboBox: TComboBox
        Left = 12
        Top = 28
        Width = 145
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        Items.Strings = (
          'Przy w'#322#261'czeniu AQQ'
          'Co godzin'#281
          'Co dwie godziny'
          'Co trzy godziny'
          'Co cztery godziny')
      end
      object CheckUpdatesAfterSaveBox: TCheckBox
        Left = 10
        Top = 82
        Width = 291
        Height = 17
        Caption = 'Sprawdzaj dost'#281'pno'#347#263' aktualizacji po zapisaniu ustawie'#324
        TabOrder = 1
      end
    end
  end
  object ActionList: TActionList
    object aReadSettings: TAction
      Category = 'Settings'
      Caption = 'aReadSettings'
      OnExecute = aReadSettingsExecute
    end
    object aSaveSettings: TAction
      Category = 'Settings'
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aResetSettings: TAction
      Category = 'Settings'
      Caption = 'aResetSettings'
      OnExecute = aResetSettingsExecute
    end
    object aExit: TAction
      Caption = 'aExit'
      ShortCut = 27
      OnExecute = aExitExecute
    end
  end
  object CheckUpdatesOnStartTimer: TTimer
    Enabled = False
    Interval = 300000
    OnTimer = CheckUpdatesOnStartTimerTimer
    Left = 384
    Top = 8
  end
  object CheckUpdatesTimer: TTimer
    Enabled = False
    Interval = 3600000
    OnTimer = CheckUpdatesTimerTimer
    Left = 352
    Top = 8
  end
end
