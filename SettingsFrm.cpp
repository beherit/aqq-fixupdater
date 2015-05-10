//---------------------------------------------------------------------------
// Copyright (C) 2009-2015 Krzysztof Grochocki
//
// This file is part of FixUpdater
//
// FixUpdater is free software: you can redistribute it and/or modify
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
// along with GNU Radio. If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#include <inifiles.hpp>
#include <LangAPI.hpp>
#pragma hdrstop
#include "SettingsFrm.h"
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
TSettingsForm *SettingsForm;
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
__declspec(dllimport)void KillTimerEx();
__declspec(dllimport)void SetTimerEx(int Interval);
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMTransparency(TMessage &Message)
{
	Application->ProcessMessages();
	if(sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
	//Lokalizowanie formy
	LangForm(this);
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

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
	//Odczyt ustawien
	aLoadSettings->Execute();
	//Wylaczenie przycisku do zapisu
	SaveButton->Enabled = false;
	//Ustawienie fokusu na przycisku anulowania
	CancelButton->SetFocus();
	//Ustawienie domyslnej zakladki
	PageControl->ActivePage = ReposTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
	//Otwarcie pliku ustawien
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
	//Odczyt repozytoriow
	UrlListPreview->Items->Clear();
	TStringList *Links = new TStringList;
	Ini->ReadSection("Links",Links);
	int LinksCount = Links->Count/2;
	delete Links;
	for(int Count=0; Count<LinksCount; Count++)
	{
		UrlListPreview->Items->Add();
		UrlListPreview->Items->Item[Count]->Checked = Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true);
		UrlListPreview->Items->Item[Count]->SubItems->Add(Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""));
	}
	//Czestotliwosc aktualizacji
	UpdateFrequencyComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateTime", 0);
	//Sposob aktualizacji
	UpdateModeComboBox->ItemIndex = Ini->ReadInteger("Settings", "UpdateMode", 0);
	//Zamkniecie pliku ustawien
	delete Ini;
	//Czas ostatniej aktualizacji
	LastUpdateLabel->Caption = GetLastUpdate();
	LastUpdateLabel->Left = LastUpdateInfoLabel->Left + LastUpdateInfoLabel->Width + 2;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
	//Wylaczenie timera sprawdzania aktualizacji
	KillTimerEx();
	//Otwarcie pliku ustawien
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\FixUpdater\\\\Settings.ini");
	//Odczyt repozytoriow
	TStringList *Links = new TStringList;
	Ini->ReadSection("Links",Links);
	int LinksCount = Links->Count/2;
	delete Links;
	for(int Count=0; Count<LinksCount; Count++)
	{
		//Repozytorium wlaczone
		if(Ini->ReadBool("Links", "Enable" + IntToStr(Count+1), true))
			//Usuniecie adresu repozytoria z aktualizatora
			SetUpdateLink(Ini->ReadString("Links", "Url" + IntToStr(Count+1), ""), false);
	}
	//Zapisywanie aktualnych ustawien repozytorium
	Ini->EraseSection("Links");
	for(int Count=0; Count<UrlListPreview->Items->Count; Count++)
	{
		Ini->WriteString("Links", "Url" +	IntToStr(Count+1), UrlListPreview->Items->Item[Count]->SubItems->Strings[0]);
		Ini->WriteBool("Links", "Enable" + IntToStr(Count+1), UrlListPreview->Items->Item[Count]->Checked);
	}
	//Czestotliwosc aktualizacji
	Ini->WriteInteger("Settings", "UpdateTime", UpdateFrequencyComboBox->ItemIndex);
	//Sposob aktualizacji
	Ini->WriteInteger("Settings", "UpdateMode", UpdateModeComboBox->ItemIndex);
	//Zamkniecie pliku ustawien
	delete Ini;
	//Wlaczenie timera sprawdzania aktualizacji
	if(UpdateFrequencyComboBox->ItemIndex)	SetTimerEx(3600000 * UpdateFrequencyComboBox->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aResetSettingsExecute(TObject *Sender)
{
	//Usuwanie wszystkich dostepnych repozytorium
	UrlListPreview->Items->Clear();
	//Dodawanie domyslnych repozytorium
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[0]->Checked = true;
	UrlListPreview->Items->Item[0]->SubItems->Add("http://beherit.pl/aqq_update/plugins.xml");
	UrlListPreview->Items->Add();
	UrlListPreview->Items->Item[1]->Checked = true;
	UrlListPreview->Items->Item[1]->SubItems->Add("http://beherit.pl/aqq_update/themes.xml");
	//Wlaczenie przycisku do zapisu
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSettingsChangedExecute(TObject *Sender)
{
	//Wlaczenie przycisku do zapisu
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
	//Zamkniecie formy ustawien
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
	//Wylaczenie przyciskow
	SaveButton->Enabled = false;
	CancelButton->Enabled = false;
	OkButton->Enabled = false;
	//Zapisywanie ustawien
	aSaveSettings->Execute();
	//Odczyt ustawien w rdzeniu wtyczki
	LoadSettings();
	//Wlaczenie przyciskow
	CancelButton->Enabled = true;
	OkButton->Enabled = true;
	//Zamkniecie formy ustawien
	Close();
	//Sprawdzanie dostepnosci aktualizacji
	CheckUpdates(UpdateModeComboBox->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
	//Wylaczenie przyciskow
	SaveButton->Enabled = false;
	CancelButton->Enabled = false;
	OkButton->Enabled = false;
	//Zapisywanie ustawien
	aSaveSettings->Execute();
	//Odczyt ustawien w rdzeniu wtyczki
  LoadSettings();
	//Wlaczenie przyciskow
	CancelButton->Enabled = true;
	OkButton->Enabled = true;
	//Sprawdzanie dostepnosci aktualizacji
	CheckUpdates(UpdateModeComboBox->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UrlListPreviewSelectItem(TObject *Sender, TListItem *Item,
			bool Selected)
{
	//Zezwolenie edycji dodatkowych adresow repozytoriow
	if(UrlListPreview->ItemIndex>1)
	{
		DeleteButton->Enabled = true;
		EditButton->Enabled = true;
	}
	//Zablokowanie edycji domyslnych adresow repozytoriow
	else
	{
		DeleteButton->Enabled = false;
		EditButton->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AddButtonClick(TObject *Sender)
{
	//Dodawanie nowego elementu
	UnicodeString URL;
	if(InputQuery(GetLangStr("NewRepository"), GetLangStr("AddNewURL"), URL))
	{
		//Jezeli zostal wpisany jakis tekst
		if(!URL.IsEmpty())
		{
			//Sprawdzanie czy wskazany adres URL jest juz dodany
			bool ItemExists = false;
			for(int Count=0; Count<UrlListPreview->Items->Count; Count++)
			{
				if(UrlListPreview->Items->Item[Count]->SubItems->Strings[0]==URL)
				{
					ItemExists = true;
					Count = UrlListPreview->Items->Count;
				}
			}
			//Wskazany adres URL nie zostal jeszcze dodany
			if(!ItemExists)
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

void __fastcall TSettingsForm::DeleteButtonClick(TObject *Sender)
{
	//Usuwanie wybranego elementu
	UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->Delete();
	//Wylaczanie przyciskow
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
	//Wlaczenie przycisku do zapisu
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::EditButtonClick(TObject *Sender)
{
	//Edycja wybranego elementu
	UnicodeString URL = InputBox(GetLangStr("EditRepository"), GetLangStr("EditURL"), UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0]);
	if(!URL.IsEmpty())
	{
		//Zmiana wybranego elementu
		UrlListPreview->Items->Item[UrlListPreview->ItemIndex]->SubItems->Strings[0] = URL;
		//Wlaczenie przycisku do zapisu
		SaveButton->Enabled = true;
	}
	//Wylaczanie przyciskow
	DeleteButton->Enabled = false;
	EditButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ResetButtonClick(TObject *Sender)
{
	//Przywracanie domyslnych ustawien repozytoriow
	aResetSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UrlListPreviewClick(TObject *Sender)
{
	//Wylaczanie przyciskow
	if(UrlListPreview->ItemIndex==-1)
	{
		DeleteButton->Enabled = false;
		EditButton->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UrlListPreviewItemChecked(TObject *Sender, TListItem *Item)
{
	//Wylaczenie przycisku do zapisu
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning)
{
	AllowSkinning = false;
}
//---------------------------------------------------------------------------

