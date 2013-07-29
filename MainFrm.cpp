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
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
__declspec(dllimport)UnicodeString GetLastUpdate();
__declspec(dllimport)void SetUpdateLink(UnicodeString URL, bool Enabled);
__declspec(dllimport)void CheckUpdates(int Mode);
__declspec(dllimport)void ChangeAddonBrowser(bool Enabled);
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  //Skorkowanie okna
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
  //Odczyt ustawien
  aLoadSettings->Execute();
  //Wylaczenie przycisku do zapisu
  SaveButton->Enabled = false;
  //Ustawienie fokusu na kontrolce
  CancelButton->SetFocus();
  //Ustawienie domyslnej zakladki
  PageControl->ActivePage = LinksTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aLoadSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  //Kana³y aktualizacji
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
  //Czêstotliwoœci aktualizacji
  UpdateTimeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateTime", 0);
  //Czas ostatniej aktualizacji
  LastUpdateLabel->Caption = GetLastUpdate();
  //Pozycja labela
  LastUpdateLabel->Left = LastUpdateInfoLabel->Left + LastUpdateInfoLabel->Width + 2;
  //Sposób aktualizacji
  UpdateModeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //Menedzer dodatkow
  ChangeAddonBrowserCheckBox->Checked = Ini->ReadBool("Settings", "ChangeAddonBrowser", true);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  //Wylaczenie timerow sprawdzania aktualizacji dodatkow
  CheckUpdatesTimer->Enabled = false;
  CheckUpdatesOnStartTimer->Enabled = false;
  //Odczyt pliku ustawien
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
  //Wy³aczanie wszystkich aktualizacji
  TStringList *Links = new TStringList;
  Ini->ReadSection("Links",Links);
  for(int Count=0;Count<Links->Count/2;Count++)
  {
	if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
	{
	  UnicodeString Url = Ini->ReadString("Links", "Url" + IntToStr(Count+1), "");
	  if(!Url.IsEmpty()) SetUpdateLink(Url,false);
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
  //W³aczanie wszystkich aktualizacji
  for(int Count=0;Count<UrlListPreview->Items->Count;Count++)
  {
	if((!UrlListPreview->Items->Item[Count]->SubItems->Strings[0].IsEmpty())&&
	(UrlListPreview->Items->Item[Count]->Checked))
	{
	  SetUpdateLink(UrlListPreview->Items->Item[Count]->SubItems->Strings[0],true);
	}
  }
  //Czêstotliwoœci aktualizacji
  Ini->WriteInteger("Settings", "UpdateTime",UpdateTimeComboBox->ItemIndex);
  if(UpdateTimeComboBox->ItemIndex!=0)
  {
	CheckUpdatesTimer->Interval = 3600000 * UpdateTimeComboBox->ItemIndex;
	CheckUpdatesTimer->Enabled = true;
  }
  //Sposób aktualizacji
  Ini->WriteInteger("Settings", "UpdateMode",UpdateModeComboBox->ItemIndex);
  UpdateMode = UpdateModeComboBox->ItemIndex;
  //Menedzer dodatkow
  Ini->WriteBool("Settings", "ChangeAddonBrowser",ChangeAddonBrowserCheckBox->Checked);
  ChangeAddonBrowser(ChangeAddonBrowserCheckBox->Checked);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
  //Usuwanie wszystkich dostepnych kanalow aktualizacji
  UrlListPreview->Items->Clear();
  //Dodawanie domyslnych kanalow aktualizacji
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
  //Wlaczenie przycisku do zapisu
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSettingsChangedExecute(TObject *Sender)
{
  //Wlaczenie przycisku do zapisu
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  //Zamkniecie formy ustawien
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OkButtonClick(TObject *Sender)
{
  //Wylaczenie przyciskow
  SaveButton->Enabled = false;
  CancelButton->Enabled = false;
  OkButton->Enabled = false;
  //Zapisywanie ustawien
  aSaveSettings->Execute();
  //Wlaczenie przyciskow
  CancelButton->Enabled = true;
  OkButton->Enabled = true;
  //Zamkniecie formy ustawien
  Close();
  //Sprawdzanie dostepnosci aktualizacji
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  //Wylaczenie przyciskow
  SaveButton->Enabled = false;
  CancelButton->Enabled = false;
  OkButton->Enabled = false;
  //Zapisywanie ustawien
  aSaveSettings->Execute();
  //Wlaczenie przyciskow
  CancelButton->Enabled = true;
  OkButton->Enabled = true;
  //Sprawdzanie dostepnosci aktualizacji
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected)
{
  //Zezwolenie edycji elementow
  if(UrlListPreview->ItemIndex>3)
  {
	DeleteButton->Enabled = true;
	EditButton->Enabled = true;
  }
  //Zablokowanie edycji elementow
  else
  {
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AddButtonClick(TObject *Sender)
{
  UnicodeString URL;
  if(InputQuery("Nowy kana³ aktualizacji","Dodaj nowy adres:",URL))
  {
	//Jezeli zostal wpisany jakis tekst
	if(!URL.IsEmpty())
	{
	  //Sprawdzanie czy wskazany adres URL jest juz dodany
	  bool DoNotAdd = false;
	  for(int Count=0;Count<UrlListPreview->Items->Count;Count++)
	  {
		if(UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
		{
		  DoNotAdd = true;
		  Count = UrlListPreview->Items->Count;
		}
	  }
	  //Wskazany adres URL nie zostal jeszcze dodany
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

void __fastcall TMainForm::DeleteButtonClick(TObject *Sender)
{
  //Usuwanie wybranego elementu
  if(UrlListPreview->ItemIndex!=-1)
   UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->Delete();
  //Wylaczanie przyciskow
  DeleteButton->Enabled = false;
  EditButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EditButtonClick(TObject *Sender)
{
  //Edycja wybranego elementu
  if(UrlListPreview->ItemIndex!=-1)
  {
	UnicodeString URL = InputBox("Edycja kana³u aktualizacji","Edytuj adres:",UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]);
	if(!URL.IsEmpty())
	{
	  UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0] = URL;
	}
	UrlListPreview->ItemIndex = -1;
  }
  //Wylaczanie przyciskow
  DeleteButton->Enabled = false;
  EditButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetButtonClick(TObject *Sender)
{
  //Przywracanie domyslnych ustawien kanalow aktualizacji
  aResetSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewClick(TObject *Sender)
{
  //Wylaczanie przyciskow
  if(UrlListPreview->ItemIndex==-1)
  {
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewItemChecked(TObject *Sender, TListItem *Item)
{
  //Wylaczenie przycisku do zapisu
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckUpdatesOnStartTimerTimer(TObject *Sender)
{
  //Wylaczenie timera
  CheckUpdatesOnStartTimer->Enabled = false;
  //Sprawdzanie dostepnosci aktualizacji
  CheckUpdates(UpdateMode);
  //Jezeli aktulizacja nie ma byc sprawdza tylko przy starcie AQQ
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
  //Sprawdzanie dostepnosci aktualizacji
  CheckUpdates(UpdateMode);
}
//---------------------------------------------------------------------------
