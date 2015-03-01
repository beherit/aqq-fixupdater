//---------------------------------------------------------------------------
// Copyright (C) 2009-2015 Krzysztof Grochocki
//
// This file is part of FixUpdater
//
// FixUpdater is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// FixUpdater is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

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
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)int GetHUE();
__declspec(dllimport)int GetSaturation();
__declspec(dllimport)int GetBrightness();
__declspec(dllimport)UnicodeString GetLastUpdate();
__declspec(dllimport)void SetUpdateLink(UnicodeString URL, bool Enabled);
__declspec(dllimport)void CheckUpdates(int Mode);
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::WMTransparency(TMessage &Message)
{
	Application->ProcessMessages();
	if(sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
		UnicodeString ThemeSkinDir = GetThemeSkinDir();
		//Plik zaawansowanej stylizacji okien istnieje
		if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
		{
			//Dane pliku zaawansowanej stylizacji okien
			ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
			sSkinManager->SkinDirectory = ThemeSkinDir;
			sSkinManager->SkinName = "Skin.asz";
			//Ustawianie animacji AlphaControls
			if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
			else sSkinManager->AnimEffects->FormShow->Time = 0;
			sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
			//Zmiana kolorystyki AlphaControls
			sSkinManager->HueOffset = GetHUE();
			sSkinManager->Saturation = GetSaturation();
			sSkinManager->Brightness = GetBrightness();
			//Aktywacja skorkowania AlphaControls
			sSkinManager->Active = true;
		}
		//Brak pliku zaawansowanej stylizacji okien
		else sSkinManager->Active = false;
	}
	//Zaawansowana stylizacja okien wylaczona
	else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
	//Odczyt ustawien
	aLoadSettings->Execute();
	//Wylaczenie przycisku do zapisu
	SaveButton->Enabled = false;
	//Ustawienie fokusu na kontrolce
	CancelButton->SetFocus();
	//Ustawienie domyslnej zakladki
	PageControl->ActivePage = ReposTabSheet;
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
			Ini->WriteString("Links", "Url" +	IntToStr(Count+1), UrlListPreview->Items->Item[Count]->SubItems->Strings[0]);
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
	delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::aResetSettingsExecute(TObject *Sender)
{
	//Usuwanie wszystkich dostepnych repozytorium
	UrlListPreview->Items->Clear();
	//Dodawanie domyslnych repozytorium
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[0]->Checked = true;
	UrlListPreview->Items->Item[0]->SubItems->Add("http://beherit.pl/aqq_update/stable.xml");
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[1]->Checked = false;
	UrlListPreview->Items->Item[1]->SubItems->Add("http://beherit.pl/aqq_update/beta.xml");
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
	if(UrlListPreview->ItemIndex>1)
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
	if(InputQuery("Nowe repozytorium","Dodaj nowy adres:",URL))
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
	//Wlaczenie przycisku do zapisu
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EditButtonClick(TObject *Sender)
{
	//Edycja wybranego elementu
	if(UrlListPreview->ItemIndex!=-1)
	{
		UnicodeString URL = InputBox("Edycja repozytorium","Edytuj adres:",UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]);
		if(!URL.IsEmpty())
		{
			//Zmiana wybranego elementu
			UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0] = URL;
			//Wlaczenie przycisku do zapisu
			SaveButton->Enabled = true;
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
	//Przywracanie domyslnych ustawien repozytorium
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

void __fastcall TMainForm::sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning)
{
	AllowSkinning = false;
}
//---------------------------------------------------------------------------

