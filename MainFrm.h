//---------------------------------------------------------------------------
#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include "sLabel.hpp"
#include "sListView.hpp"
#include "sPageControl.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TActionList *ActionList;
	TAction *aReadSettings;
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
	TsTabSheet *LinksTabSheet;
	TsTabSheet *SettingsTabSheet;
	TsComboBox *UpdateTimeComboBox;
	TsLabel *UpdateTimeLabel;
	TsLabel *LastUpdateInfoLabel;
	TTimer *CheckUpdatesOnStartTimer;
	TTimer *CheckUpdatesTimer;
	TsBevel *Bevel;
	TsComboBox *UpdateModeComboBox;
	TsLabel *UpdateModeLabel;
	TsBevel *Bevel2;
	TAction *aSettingsChanged;
	TsButton *CancelButton;
	TsButton *OkButton;
	TsBevel *Bevel1;
	TsCheckBox *ChangeAddonBrowserCheckBox;
	TsSkinProvider *sSkinProvider;
	TsSkinManager *sSkinManager;
	TsLabelFX *LastUpdateLabel;
	TAction *aPageControlSheetChange;
	void __fastcall UrlListPreviewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall aReadSettingsExecute(TObject *Sender);
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
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall aPageControlSheetChangeExecute(TObject *Sender);
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
