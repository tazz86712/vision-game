//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.MPlayer.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *miWebCam;
	TMenuItem *miDriverNameVersion;
	TMenuItem *miDriverOn;
	TMenuItem *miDriverOff;
	TMenuItem *miCreateConnect;
	TMenuItem *miWindowSize;
	TMenuItem *miPreview;
	TMenuItem *miPreviewOn;
	TMenuItem *miPreviewOff;
	TMenuItem *miPreviewScale;
	TMenuItem *miScaleOn;
	TMenuItem *miScaleOff;
	TMenuItem *mi320240;
	TMenuItem *mi640480;
	TMenuItem *mi1280960;
	TMenuItem *miDestroyDisconnect;
	TMenuItem *miCapability;
	TMenuItem *Dialog1;
	TMenuItem *miVideoSource;
	TMenuItem *miVideoFormat;
	TMenuItem *miVideoDisplay;
	TMenuItem *miVideoCompression;
	TMenuItem *miStatus;
	TSavePictureDialog *SavePictureDialog1;
	TMenuItem *miPreviewCallbackFunction;
	TMenuItem *miBall;
	TMenuItem *miSingleColor;
	TMenuItem *miGrayMode;
	TMenuItem *miColorMode;
	TMenuItem *miColorFiltering;
	TMenuItem *miBackground;
	TMenuItem *miDifferenceDetection;
	TMenuItem *miSpecificColor;
	TMenuItem *miSkinColor;
	TMenuItem *miDDColorMode;
	TMenuItem *miBinaryMode;
	TMenuItem *miBlockMode;
	TMenuItem *miInteraction;
	TMenuItem *miFrisbee;
	TPageControl *PageControl1;
	TTabSheet *tsCaptureWindow;
	TLabel *lblY;
	TLabel *lblU;
	TLabel *lblV;
	TLabel *lblFrameCounter;
	TLabel *lbliThreshold;
	TPanel *pnlCaptureWindow;
	TButton *btnGrabFrame;
	TButton *btnGrabFrameNoStop;
	TButton *btnEditCopy;
	TButton *btnFileSaveDIB;
	TEdit *edMinY;
	TEdit *edMaxY;
	TEdit *edMinU;
	TEdit *edMaxU;
	TEdit *edMinV;
	TEdit *edMaxV;
	TEdit *edThreshold;
	TTabSheet *tsDescription;
	TLabel *lblDriverDescription;
	TLabel *lblDriverName;
	TLabel *lblDriverVersion;
	TLabel *lblIndex;
	TLabel *lblOverlay;
	TLabel *lblHasVideoSourceDialog;
	TLabel *lblHasVideoFormatDialog;
	TLabel *lblHasVideoDisplayDialog;
	TLabel *lblDriverReadyToCapture;
	TLabel *lblCanDriverMakepalette;
	TLabel *lblDriverCapacity;
	TLabel *lblCaptureWindowStatus;
	TLabel *lblImageWidth;
	TLabel *lblImageHeight;
	TLabel *lblLiveWindow;
	TLabel *lblOverlayWindow;
	TLabel *lblScale;
	TLabel *lblScroll;
	TLabel *lblUsingDefaultPalette;
	TLabel *lblAudioHardware;
	TLabel *lblCaptureFileExists;
	TLabel *lblCurrentVideoFrame;
	TLabel *lblCurrentVideoFrameDropped;
	TLabel *lblCurrentWaveSamples;
	TLabel *lblCurrentTimeElapsedMS;
	TLabel *lblCapturingNow;
	TLabel *lblReturn;
	TLabel *lblNumberOfVideoAllocated;
	TLabel *lblNumberOfAudioAllocated;
	TImage *imBall;
	TTabSheet *tsImage;
	TImage *imBackground;
	TImage *imDuck1;
	TMenuItem *miPlay1;
	TMenuItem *miReplay;
	TImage *imDuck2;
	TCheckBox *cbBlockDisplay;
	TTabSheet *tsMaterial;
	TImage *imTree;
	TImage *imDuck3;
	TLabel *Label1;
	TLabel *Label2;
	TImage *imTitle;
	TImage *imOver;
	TImage *imWin;
	TMediaPlayer *MediaPlayer1;
	TMenuItem *miMediaPlayer;
	TOpenDialog *OpenDialog1;
	void __fastcall miDriverOnClick(TObject *Sender);
	void __fastcall miDriverOffClick(TObject *Sender);
	void __fastcall miCreateConnectClick(TObject *Sender);
	void __fastcall miPreviewOnClick(TObject *Sender);
	void __fastcall miPreviewOffClick(TObject *Sender);
	void __fastcall miScaleOnClick(TObject *Sender);
	void __fastcall miScaleOffClick(TObject *Sender);
	void __fastcall mi320240Click(TObject *Sender);
	void __fastcall mi640480Click(TObject *Sender);
	void __fastcall mi1280960Click(TObject *Sender);
	void __fastcall miDestroyDisconnectClick(TObject *Sender);
	void __fastcall miCapabilityClick(TObject *Sender);
	void __fastcall miVideoSourceClick(TObject *Sender);
	void __fastcall miVideoFormatClick(TObject *Sender);
	void __fastcall miVideoDisplayClick(TObject *Sender);
	void __fastcall miVideoCompressionClick(TObject *Sender);
	void __fastcall miStatusClick(TObject *Sender);
	void __fastcall btnGrabFrameClick(TObject *Sender);
	void __fastcall btnGrabFrameNoStopClick(TObject *Sender);
	void __fastcall btnEditCopyClick(TObject *Sender);
	void __fastcall btnFileSaveDIBClick(TObject *Sender);
	void __fastcall miBallClick(TObject *Sender);
	void __fastcall miSpecificColorClick(TObject *Sender);
	void __fastcall edMinYChange(TObject *Sender);
	void __fastcall edMaxYChange(TObject *Sender);
	void __fastcall edMinUChange(TObject *Sender);
	void __fastcall edMaxUChange(TObject *Sender);
	void __fastcall edMinVChange(TObject *Sender);
	void __fastcall edMaxVChange(TObject *Sender);
	void __fastcall miSkinColorClick(TObject *Sender);
	void __fastcall miBackgroundClick(TObject *Sender);
	void __fastcall miDDColorModeClick(TObject *Sender);
	void __fastcall miBinaryModeClick(TObject *Sender);
	void __fastcall edThresholdChange(TObject *Sender);
	void __fastcall miBlockModeClick(TObject *Sender);
	void __fastcall miPlay1Click(TObject *Sender);
	void __fastcall miReplayClick(TObject *Sender);
	void __fastcall miMediaPlayerClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
