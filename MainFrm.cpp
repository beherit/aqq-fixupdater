//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MainFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sComboBox"
#pragma link "sLabel"
#pragma link "sListView"
#pragma link "sPageControl"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetLastUpdate();
__declspec(dllimport)void SetUpdateLink(bool Enabled, UnicodeString URL);
__declspec(dllimport)void CheckUpdates(int Mode);
__declspec(dllimport)void ChangeAddonBrowser(bool Enabled);
UnicodeString eUrl;
//-----------------------------------S----------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected)
{
  if(UrlListPreview->ItemIndex>3)
  {
	DeleteButton->Enabled = true;
	EditButton->Enabled = true;
  }
  else
  {
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DeleteButtonClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex!=-1)
   UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->Delete();
  DeleteButton->Enabled = false;
  EditButton->Enabled = false;

  if(UrlListPreview->Items->Count==0)
   SaveButton->Enabled = false;
  else
   SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  SaveButton->Enabled = false;
  aSaveSettings->Execute();
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AddButtonClick(TObject *Sender)
{
  UnicodeString URL;
  bool DoNotAdd = false;
  if(InputQuery("Nowy kana� aktualizacji","Dodaj nowy adres:",URL))
  {
	if(!URL.IsEmpty())
	{
	  for(int Count=0;Count<UrlListPreview->Items->Count;Count++)
	  {
		if(UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
		{
		  DoNotAdd = true;
		  Count = UrlListPreview->Items->Count;
		}
	  }
	  if(!DoNotAdd)
	  {
		UrlListPreview->Items->Add();
		UrlListPreview->Items->Item[UrlListPreview->Items->Count-1]->Checked = true;
		UrlListPreview->Items->Item[UrlListPreview->Items->Count-1]->SubItems->Add(URL);
		SaveButton->Enabled = true;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  //Kana�y aktualizacji
  UrlListPreview->Items->Clear();
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<(Links->Count/2);Count++)
  {
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[Count]->Checked = Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), false);
	UrlListPreview->Items->Item[Count]->SubItems->Add(Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""));
  }
  delete Links;
  //Cz�stotliwo�ci aktualizacji
  UpdateTimeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateTime", 0);
  //Spos�b aktualizacji
  UpdateModeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //Czas ostatniej aktualizacji
  LastUpdateLabel->Caption = GetLastUpdate();
  //Pozycja labela
  LastUpdateLabel->Left = LastUpdateInfoLabel->Left + LastUpdateInfoLabel->Width + 2;
  //Menedzer dodatkow
  ChangeAddonBrowserCheckBox->Checked = Ini->ReadBool("Settings", "ChangeAddonBrowser", true);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  aReadSettings->Execute();
  SaveButton->Enabled = false;
  PageControl->ActivePage = LinksTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  CheckUpdatesTimer->Enabled = false;
  CheckUpdatesOnStartTimer->Enabled = false;

  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  //Wy�aczanie wszystkich aktualizacji
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
	{
	  eUrl = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(!eUrl.IsEmpty()) SetUpdateLink(false,eUrl);
	}
  }
  delete Links;
  //Zapisywanie aktualnych ustawien
  Ini->EraseSection("Links");
  for(int Count=0;Count<UrlListPreview->Items->Count;Count++)
  {
	if(!UrlListPreview->Items->Item[Count]->SubItems->Strings[0].IsEmpty())
	{
	  Ini->WriteString("Links", "Url" +  IntToStr(Count+1), UrlListPreview->Items->Item[Count]->SubItems->Strings[0]);
	  Ini->WriteBool("Links", "Enable" + IntToStr(Count+1), UrlListPreview->Items->Item[Count]->Checked);
	}
  }
  //W�aczanie wszystkich aktualizacji
  for(int Count=0;Count<UrlListPreview->Items->Count;Count++)
  {
	if((!UrlListPreview->Items->Item[Count]->SubItems->Strings[0].IsEmpty())&&
	(UrlListPreview->Items->Item[Count]->Checked))
	{
	  SetUpdateLink(true,UrlListPreview->Items->Item[Count]->SubItems->Strings[0]);
	}
  }
  //Spos�b aktualizacji
  Ini->WriteInteger("Settings", "UpdateMode",UpdateModeComboBox->ItemIndex);
  UpdateMode = UpdateModeComboBox->ItemIndex;
  //Cz�stotliwo�ci aktualizacji
  Ini->WriteInteger("Settings", "UpdateTime",UpdateTimeComboBox->ItemIndex);
  if(UpdateTimeComboBox->ItemIndex!=0)
  {
	CheckUpdatesTimer->Interval = 3600000 * UpdateTimeComboBox->ItemIndex;
	CheckUpdatesTimer->Enabled = true;
  }
  //Menedzer dodatkow
  Ini->WriteBool("Settings", "ChangeAddonBrowser",ChangeAddonBrowserCheckBox->Checked);
  ChangeAddonBrowser(ChangeAddonBrowserCheckBox->Checked);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
  UrlListPreview->Items->Clear();

  UrlListPreview->Items->Add();
  UrlListPreview->Items->Item[0]->Checked = true;
  UrlListPreview->Items->Item[0]->SubItems->Add("http://beherit.pl/aqq_update/stable.xml");
  UrlListPreview->Items->Add();
  UrlListPreview->Items->Item[1]->Checked = false;
  UrlListPreview->Items->Item[1]->SubItems->Add("http://beherit.pl/aqq_update/beta.xml");
  UrlListPreview->Items->Add();
  UrlListPreview->Items->Item[2]->Checked = true;
  UrlListPreview->Items->Item[2]->SubItems->Add("http://files.aqqnews.pl/fixupdater.php");
  UrlListPreview->Items->Add();
  UrlListPreview->Items->Item[3]->Checked = false;
  UrlListPreview->Items->Item[3]->SubItems->Add("http://files.aqqnews.pl/fixupdater-beta.php");

  SaveButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetButtonClick(TObject *Sender)
{
  aResetSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex==-1)
  {
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EditButtonClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex!=-1)
  {
	UnicodeString URL = InputBox("Edycja kana�u aktualizacji","Edytuj adres:",UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]);
	if(!URL.IsEmpty())
	{
	  UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0] = URL;
	}
	UrlListPreview->ItemIndex = -1;
  }

  DeleteButton->Enabled = false;
  EditButton->Enabled = false;

  if(UrlListPreview->Items->Count==0)
   SaveButton->Enabled = false;
  else
   SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckUpdatesOnStartTimerTimer(TObject *Sender)
{
  CheckUpdatesOnStartTimer->Enabled = false;
  CheckUpdates(UpdateMode);

  if(UpdateTime!=0)
  {
	CheckUpdatesTimer->Enabled = false;
	CheckUpdatesTimer->Interval = 3600000 * UpdateTime;
	CheckUpdatesTimer->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckUpdatesTimerTimer(TObject *Sender)
{
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSettingsChangedExecute(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewItemChecked(TObject *Sender, TListItem *Item)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OkButtonClick(TObject *Sender)
{
  aSaveSettings->Execute();
  Close();
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CancelButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  UnicodeString ThemeSkinDir = GetThemeSkinDir();
  if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
  {
	ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	sSkinManager->SkinDirectory = ThemeSkinDir;
	sSkinManager->SkinName = "Skin.asz";
	sSkinProvider->DrawNonClientArea = ChkSkinEnabled();
	sSkinManager->Active = true;
  }
  else
   sSkinManager->Active = false;
}
//---------------------------------------------------------------------------
