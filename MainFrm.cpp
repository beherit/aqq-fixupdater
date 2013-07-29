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
__declspec(dllimport)void SetUpdateLink(bool Enabled, UnicodeString URL);
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
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AddButtonClick(TObject *Sender)
{
  UnicodeString URL;
  if(InputQuery("Adres serwera aktualizacji","Dodaj nowy adres:",URL))
  {
	if(URL!="")
	{
	  int Count=UrlListPreview->Items->Count;
	  UrlListPreview->Items->Add();
	  UrlListPreview->Items->Item[Count]->Checked=true;
	  UrlListPreview->Items->Item[Count]->SubItems->Add(URL);
	  SaveButton->Enabled=true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aReadSettingsExecute(TObject *Sender)
{
  eDir = GetPluginUserDir();

  TIniFile *Ini = new TIniFile(eDir + "\\\\FixUpdater\\\\Url.ini");
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 2);

  UrlListPreview->Items->Clear();

  for(eCount=0;eCount<eUrlCount;eCount++)
  {
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[eCount]->Checked = Ini->ReadBool("Update" + IntToStr(eCount+1), "Enable", true);
	UrlListPreview->Items->Item[eCount]->SubItems->Add( Ini->ReadString("Update" + IntToStr(eCount+1), "Url", "") );
  }

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

  TIniFile *Ini = new TIniFile(eDir + "\\\\FixUpdater\\\\Url.ini");

  //Wyłaczanie wszystkich aktualizacji
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 2);
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
	if(Trim(UrlListPreview->Items->Item[eCount]->SubItems->GetText())!="")
	{
	  Ini->WriteBool("Update" + IntToStr(eCount + 1), "Enable", UrlListPreview->Items->Item[eCount]->Checked);
	  Ini->WriteString("Update" + IntToStr(eCount + 1), "Url", Trim(UrlListPreview->Items->Item[eCount]->SubItems->GetText()));
	}
  }

  //Właczanie wszystkich aktualizacji
  eUrlCount = Ini->ReadInteger("UrlCount", "Count", 2);
  for(eCount=1;eCount<=eUrlCount;eCount++)
  {
	if(Ini->ReadBool("Update" + IntToStr(eCount), "Enable", true)==true)
	{
	  eUrl = Ini->ReadString("Update" + IntToStr(eCount), "Url", "");
	  if(eUrl!="") SetUpdateLink(0, eUrl);
	}
  }

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
	UnicodeString URL = InputBox("Adres serwera aktualizacji","Edycja adresu:",Trim(UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->GetText()));
	if(URL!="")
	{
	  UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Text=URL;
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

