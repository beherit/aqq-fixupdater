//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MainFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetLastUpdate();
__declspec(dllimport)void SetUpdateLink(bool Enabled, UnicodeString URL);
__declspec(dllimport)void CheckUpdates(int Mode);
UnicodeString eDir;
UnicodeString eUrl;
int eUrlCount;
int eCount;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected)
{
  DeleteButton->Enabled=true;
  EditButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DeleteButtonClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex!=-1)
   UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->Delete();
  DeleteButton->Enabled=false;
  EditButton->Enabled=false;

  if(UrlListPreview->Items->Count==0)
   SaveButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
  aSaveSettings->Execute();
  Close();
  CheckUpdates(eUpdateMode);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AddButtonClick(TObject *Sender)
{
  UnicodeString URL;
  bool DoNotAdd = false;
  if(InputQuery("Nowy kana³ aktualizacji","Dodaj nowy adres:",URL))
  {
	if(URL!="")
	{
	  int Count=UrlListPreview->Items->Count;
	  for(int XCount=0;XCount<Count;XCount++)
	  {
		if(UrlListPreview->Items->Item[XCount]->SubItems->Strings[0]==URL)
		{
		  DoNotAdd = true;
		  XCount = Count;
		}
	  }
	  if(DoNotAdd==false)
	  {
		UrlListPreview->Items->Add();
		UrlListPreview->Items->Item[Count]->Checked=true;
		UrlListPreview->Items->Item[Count]->SubItems->Add(URL);
		SaveButton->Enabled=true;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  eDir = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(eDir + "\\\\FixUpdater\\\\Settings.ini");
  //Kana³y aktualizacji
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
  UrlListPreview->Items->Clear();
  for(eCount=1;eCount<=eUrlCount;eCount++)
  {
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[eCount-1]->Checked = Ini->ReadBool("Update" + IntToStr(eCount), "Enable", true);
	UrlListPreview->Items->Item[eCount-1]->SubItems->Add( Ini->ReadString("Update" + IntToStr(eCount), "Url", "") );
  }
  //Czêstotliwoœci aktualizacji
  UpdateTimeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateTime", 0);
  //Sposób aktualizacji
  UpdateModeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateMode", 0);
  //Czas ostatniej aktualizacji
  LastUpdateLabel->Caption=GetLastUpdate();
  //Pozycha labela
  LastUpdateLabel->Left = LastUpdateInfoLabel->Left + LastUpdateInfoLabel->Width + 2;
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  aReadSettings->Execute();
  if(UrlListPreview->Items->Count==0)
   SaveButton->Enabled=false;
  else
   SaveButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aSaveSettingsExecute(TObject *Sender)
{
  eDir = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(eDir + "\\\\FixUpdater\\\\Settings.ini");
  //Wy³aczanie wszystkich aktualizacji
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
  for(eCount=1;eCount<=eUrlCount;eCount++)
  {
	if(Ini->ReadBool("Update" + IntToStr(eCount), "Enable", true)==true)
	{
	  eUrl = Ini->ReadString("Update" + IntToStr(eCount), "Url", "");
	  if(eUrl!="") SetUpdateLink(1, eUrl);
	}
  }
  //Zapisywanie aktualnych ustawien
  eUrlCount = UrlListPreview->Items->Count;
  Ini->WriteInteger("UrlCount", "Count",eUrlCount);
  for(eCount=0;eCount<eUrlCount;eCount++)
  {
	if(UrlListPreview->Items->Item[eCount]->SubItems->Strings[0]!="")
	{
	  Ini->WriteBool("Update" + IntToStr(eCount + 1), "Enable", UrlListPreview->Items->Item[eCount]->Checked);
	  Ini->WriteString("Update" + IntToStr(eCount + 1), "Url", UrlListPreview->Items->Item[eCount]->SubItems->Strings[0]);
	}
  }
  //W³aczanie wszystkich aktualizacji
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 3);
  for(eCount=1;eCount<=eUrlCount;eCount++)
  {
	if(Ini->ReadBool("Update" + IntToStr(eCount), "Enable", true)==true)
	{
	  eUrl = Ini->ReadString("Update" + IntToStr(eCount), "Url", "");
	  if(eUrl!="") SetUpdateLink(0, eUrl);
	}
  }
  //Czêstotliwoœci aktualizacji
  if(Ini->ReadInteger("Settings", "UpdateTime", 0)!=UpdateTimeComboBox->ItemIndex)
  {
	Ini->WriteInteger("Settings", "UpdateTime",UpdateTimeComboBox->ItemIndex);
	CheckUpdatesTimer->Enabled = false;
	CheckUpdatesOnStartTimer->Enabled=false;
	if(UpdateTimeComboBox->ItemIndex!=0)
	{
	  CheckUpdatesTimer->Interval = 3600000 * UpdateTimeComboBox->ItemIndex;
	  CheckUpdatesTimer->Enabled = true;
	}
  }
  //Sposób aktualizacji
  Ini->WriteInteger("Settings", "UpdateMode",UpdateModeComboBox->ItemIndex);
  eUpdateMode = UpdateModeComboBox->ItemIndex;
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
  UrlListPreview->Items->Item[2]->SubItems->Add("http://aqqnews.komunikatory.pl/Pliki/aqq_update.xml");

  SaveButton->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetButtonClick(TObject *Sender)
{
  aResetSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewChange(TObject *Sender, TListItem *Item,
          TItemChange Change)
{
  DeleteButton->Enabled=false;
  EditButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UrlListPreviewClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex==-1)
  {
	DeleteButton->Enabled=false;
	EditButton->Enabled=false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EditButtonClick(TObject *Sender)
{
  if(UrlListPreview->ItemIndex!=-1)
  {
	UnicodeString URL = InputBox("Edycja kana³u aktualizacji","Edytuj adres:",UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]);
	if(URL!="")
	{
	  UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]=URL;
	}
	UrlListPreview->ItemIndex=-1;
  }

  DeleteButton->Enabled=false;
  EditButton->Enabled=false;

  if(UrlListPreview->Items->Count==0)
   SaveButton->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckUpdatesOnStartTimerTimer(TObject *Sender)
{
  CheckUpdates(eUpdateMode);
  CheckUpdatesOnStartTimer->Enabled=false;

  if(eUpdateTime!=0)
  {
	CheckUpdatesTimer->Enabled = false;
	CheckUpdatesTimer->Interval = 3600000 * eUpdateTime;
	CheckUpdatesTimer->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CheckUpdatesTimerTimer(TObject *Sender)
{
  CheckUpdates(eUpdateMode);
}
//---------------------------------------------------------------------------

