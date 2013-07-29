//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include "sLabel.hpp"
#include "sListView.hpp"
#include "sPageControl.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
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
	TTimer *CheckUpdatesOnStartTimer;
	TTimer *CheckUpdatesTimer;
	TsBevel *Bevel;
	TsComboBox *UpdateModeComboBox;
	TAction *aSettingsChanged;
	TsButton *CancelButton;
	TsButton *OkButton;
	TsSkinProvider *sSkinProvider;
	TsSkinManager *sSkinManager;
	TsLabelFX *LastUpdateLabel;
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
	void __fastcall CheckUpdatesOnStartTimerTimer(TObject *Sender);
	void __fastcall CheckUpdatesTimerTimer(TObject *Sender);
	void __fastcall aSettingsChangedExecute(TObject *Sender);
	void __fastcall UrlListPreviewItemChecked(TObject *Sender, TListItem *Item);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	int UpdateTime;
    int UpdateMode;
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
