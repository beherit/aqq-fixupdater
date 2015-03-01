//---------------------------------------------------------------------------
// Copyright (C) 2009-2014 Krzysztof Grochocki
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
#ifndef MainFrmH
#define MainFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include "sLabel.hpp"
#include "sListView.hpp"
#include "sPageControl.hpp"
#include "sSkinManager.hpp"
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include "sSkinProvider.hpp"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TActionList *ActionList;
	TAction *aLoadSettings;
	TAction *aSaveSettings;
	TAction *aResetSettings;
	TsButton *AddButton;
	TsButton *DeleteButton;
	TsListView *UrlListPreview;
	TsButton *ResetButton;
	TsButton *SaveButton;
	TsButton *EditButton;
	TAction *aExit;
	TsPageControl *PageControl;
	TsTabSheet *ReposTabSheet;
	TsTabSheet *UpdaterTabSheet;
	TsComboBox *UpdateTimeComboBox;
	TsLabel *LastUpdateInfoLabel;
	TsBevel *Bevel;
	TsComboBox *UpdateModeComboBox;
	TAction *aSettingsChanged;
	TsButton *CancelButton;
	TsButton *OkButton;
	TsSkinManager *sSkinManager;
	TsLabelFX *LastUpdateLabel;
	TsSkinProvider *sSkinProvider;
	void __fastcall UrlListPreviewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall aResetSettingsExecute(TObject *Sender);
	void __fastcall ResetButtonClick(TObject *Sender);
	void __fastcall UrlListPreviewClick(TObject *Sender);
	void __fastcall EditButtonClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall aSettingsChangedExecute(TObject *Sender);
	void __fastcall UrlListPreviewItemChecked(TObject *Sender, TListItem *Item);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning);
private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
	void __fastcall WMTransparency(TMessage &Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
