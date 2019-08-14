//---------------------------------------------------------------------------

#include <vcl.h>
#include <vfw.h>
#include <time.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
// 公用變數宣告。
long dwDriverIndex;	// 電腦的驅動程式代碼。
wchar_t szDeviceName[80];
wchar_t szDeviceVersion[80];
int i, j, k,m,n;
int iFrameCounter;
BITMAPINFO bmiBitmapInfo;
int iVideoWidth, iVideoHeight;
BYTE *ptrCapture;
int iVideoFormatSize;
int iVideoMacroWidth;    // iVideoMacroWidth: 表示一列有多少個宏像素 ( 視訊寬度的一半 )。
int iRowBufferSize;           // iRowBufferSize: 為一列所需的記憶體大小, 宏像素個數乘以 4 。
int iPartner;                       // 用來儲存要交換對象的索引值。
unsigned char ucTempY1, ucTempU, ucTempY2, ucTempV;    // 暫存待交換的色彩值。
unsigned char ucPointRGB[203][203][3];
unsigned char ucPointYUV[203][203][3];
unsigned char ucDuckRGB[3][128][128][3];
unsigned char ucDuckYUV[3][128][128][3];
unsigned char ucTreeRGB[480][640][3];
unsigned char ucTreeYUV[480][640][3];
unsigned char ucTitleRGB[480][640][3];
unsigned char ucTitleYUV[480][640][3];
unsigned char ucResultRGB[2][480][640][3];
unsigned char ucResultYUV[2][480][640][3];
BYTE * ptrImage;
int iB,iG,iR,iY,iU,iV;
int iMinY,iMaxY,iMinU,iMaxU,iMinV,iMaxV;
int iTemp[ 480 ][ 640 ][ 3 ] ;
unsigned char ucBackground[ 480 ][ 640 ][ 3 ] ;
int iTempYUV[ 480 ][ 640 ][ 3 ] ;
unsigned char ucBackgroundYUV[ 480 ][ 640 ][ 3 ];
int t,iC1,iD,iE,iY1,iY2;
int iY1Difference,iUDifference,iY2Difference,iVDifference;
int iSquareDifferenceSum1,iSquareDifferenceSum2,iThreshold;
int iBlockChangeCounter[60][80];
int iVideoBlockWidth,iVideoBlockHeight;
clock_t iTime;
int iAimX=0,iAimY=0,iAimCounter=1;
int iID,iScore,iEnd;
bool bShoot;


// 32-bit 電腦請改成以下形式之宣告:
// char szDeviceName[80];	// 驅動程式名稱 Driver Name。
// char szDeviceVersion[80];	// 驅動程式 Driver Version。

HWND hwndVideo ; 	// 儲存所建立的視訊擷取視窗之代碼。
wchar_t *szWindowName;
wchar_t wstrCaptureWindowName[20] = L"My Capture Window";
int iCapWindowX1; 	// 視訊擷取視窗左上角座標 ( iCapWindowX1, iCapWindowY1 ) 。
int iCapWindowY1;
int iCapWindowWidth;
int iCapWindowHeight;

tagCapDriverCaps cdcCapDriverCapability;
tagCapStatus cdcCapDriverStatus;

wchar_t wstrCaptureFileName[80];  // 錄影儲存之檔名。
CAPTUREPARMS cpCaptureParms;  // CAPTUREPARMS 為錄影參數之結構資料型態。
long dwFileAlloc;              // 錄影要預先配置的硬碟記憶體空間, 以位元組 (byte) 為單

// 預覽回呼函式必須事先宣告於標頭檔。
LRESULT CALLBACK FrameCallbackCounter(HWND hwndVideo , PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackMirror(HWND hwndCapture, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackPoint(HWND hwndVideo , PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackSpecificColorFiltering(HWND hwndVideo, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackSkinColorFiltering ( HWND hwndVideo, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackBackgroundYUY2(HWND hwndVideo, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackDifferenceColorYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackDifferenceBinaryYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackDifferenceBlockYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr);
LRESULT CALLBACK FrameCallbackDuck( HWND hwndCapture, PVIDEOHDR lpvhdr );
LRESULT CALLBACK FrameCallbackDuck6B ( HWND hwndCapture, PVIDEOHDR lpvhdr );
typedef struct stVirtualObject  // 虛擬物件結構資料型態名稱。
	{
	int iID;       // 虛擬物件編號。
	int iX1;       // 顯示位置左上角座標 ( X1, Y1 )。
	int iY1;
	int iX2;       // 顯示位置右下點座標 ( X2, Y2 )。
	int iY2;
	int iHeight;     // 虛擬物件之高。
	int iWidth;     // 虛擬物件之寬。
	int iBlockX1;    // 顯示位置左上角區塊座標 ( BlockX1, BlockY1 )。
	int iBlockY1;
	int iBlockX2;    // 顯示位置右下點區塊座標 ( BlockX2, BlockY2 ).
	int iBlockY2;
	int iDeltaX;
	int iDeltaY;
	bool bVisible;
	unsigned char ucBackgroundRed;   // 虛擬物件背景色 ( 紅 )。
	unsigned char ucBackgroundGreen;  // 虛擬物件背景色 ( 綠 )。
	unsigned char ucBackgroundBlue;  // 虛擬物件背景色 ( 藍 )。
	} stObject;     // 虛擬物件結構資料型態名稱。
stVirtualObject voPoint;  //宣告虛擬物件。
stVirtualObject voDuck;
stVirtualObject voTree;
stVirtualObject voTitle;
stVirtualObject voResult;



//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miDriverOnClick(TObject *Sender)
{
	dwDriverIndex = 0;

	if(capGetDriverDescription(dwDriverIndex,szDeviceName,sizeof(szDeviceName),
		szDeviceVersion,sizeof(szDeviceVersion)))
	{   lblDriverName->Caption = "Name: "+AnsiString(szDeviceName);
        lblDriverVersion->Caption = "Version: "+AnsiString(szDeviceVersion);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miDriverOffClick(TObject *Sender)
{
	lblDriverName->Caption = "DriverName";
	lblDriverVersion->Caption = "DriverVersion";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miCreateConnectClick(TObject *Sender)
{
	iCapWindowX1 = 10;
	iCapWindowY1 = 10;
	iCapWindowWidth = 640;
	iCapWindowHeight = 480;

	pnlCaptureWindow->Width = 660;
	pnlCaptureWindow->Height = 500;
	hwndVideo = capCreateCaptureWindow(szWindowName, WS_CHILD | WS_VISIBLE,
		iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight,
		Form1->pnlCaptureWindow->Handle, 1);


	MediaPlayer1->Open();
	MediaPlayer1->Play();
	capDriverConnect(hwndVideo, dwDriverIndex);

	capPreview(hwndVideo, true);

	capPreviewRate(hwndVideo, 33);

	capPreviewScale(hwndVideo, true);

	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo , &bmiBitmapInfo , iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;
	iRowBufferSize = iVideoMacroWidth * 4;

	iScore = 0;
	iEnd=0;
	iFrameCounter=0;
  // 針對用來存放背景影像及累加影像的陣列設定初始値。
	for ( j = 0; j < iVideoHeight; j++ )
		for ( i = 0; i < iVideoWidth; i++ )
		{
		// RGB 色彩模式背景影像。
		ucBackground[j][i][0] = 0;
		ucBackground[j][i][1] = 0;
		ucBackground[j][i][2] = 0;
		iTemp[j][i][0] = 0;
		iTemp[j][i][1] = 0;
		iTemp[j][i][2] = 0;
		// YUV 色彩模式背景影像。
		ucBackgroundYUV[j][i][0] = 0;
		ucBackgroundYUV[j][i][1] = 0;
		ucBackgroundYUV[j][i][2] = 0;
		iTempYUV[j][i][0] = 0;
		iTempYUV[j][i][1] = 0;
		iTempYUV[j][i][2] = 0;
		}
	// 針對用來顯示背景影像的影像元件設定各項屬性 ( Property ) 値。
	Form1->imBackground->Width = iVideoWidth;
	Form1->imBackground->Height = iVideoHeight;
	Form1->imBackground->Picture->Bitmap->Width = iVideoWidth;
	Form1->imBackground->Picture->Bitmap->Height = iVideoHeight;
	Form1->imBackground->Picture->Bitmap->PixelFormat = pf24bit;
	iThreshold=1600;
	//capSetCallbackOnFrame(hwndVideo, &FrameCallbackBackgroundYUY2);
	// 開啟預覽回呼函式FrameCallbackMirror。
    iVideoBlockWidth = iVideoWidth / 8;
	iVideoBlockHeight = iVideoHeight / 8;

	// 重設亂數產生器
	iTime = clock();
	srand( iTime );

	voDuck.iX1 = -100;
	voDuck.iY1 = 50;
	voDuck.iWidth = 128;
	voDuck.iHeight = 128;
	voDuck.iX2 = voDuck.iX1+voDuck.iWidth-1;
	voDuck.iY2 = voDuck.iY1+voDuck.iHeight-1;
	voDuck.iBlockX1 = voDuck.iX1/8;
	voDuck.iBlockX2 = voDuck.iX2/8;
	voDuck.iBlockY1 = voDuck.iY1/8;
	voDuck.iBlockY2 = voDuck.iY2/8;
	voDuck.bVisible = true;
	voDuck.ucBackgroundRed = 255;
	voDuck.ucBackgroundGreen = 255;
	voDuck.ucBackgroundBlue = 255;
	iID = rand()%3;
	voDuck.iDeltaX=30;
	voDuck.iDeltaY=0;


	for ( j = 0; j < voDuck.iHeight; j++ )
	{
	ptrImage = ( BYTE * )imDuck1->Picture->Bitmap->ScanLine[j];
	for ( i=0; i<voDuck.iWidth; i++ )
	  {
	  iB = ptrImage[i*3];
	  iG = ptrImage[i*3+1];
	  iR = ptrImage[i*3+2];
	  ucDuckRGB[0][j][i][0] = ( unsigned char ) iB;
	  ucDuckRGB[0][j][i][1] = ( unsigned char ) iG;
	  ucDuckRGB[0][j][i][2] = ( unsigned char ) iR;
	  ucDuckYUV[0][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 )  >> 8 ) + 16 );
	  ucDuckYUV[0][j][i][1] = ( unsigned char ) ( ( ( -38 * iR-74 * iG + 112 * iB + 128 )  >> 8 ) + 128 );
	  ucDuckYUV[0][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 )  >> 8 ) + 128 );
	  }
	}

	for ( j = 0; j < voDuck.iHeight; j++ )
	{
	ptrImage = ( BYTE * )imDuck2->Picture->Bitmap->ScanLine[j];
	for ( i=0; i<voDuck.iWidth; i++ )
	  {
	  iB = ptrImage[i*3];
	  iG = ptrImage[i*3+1];
	  iR = ptrImage[i*3+2];
	  ucDuckRGB[1][j][i][0] = ( unsigned char ) iB;
	  ucDuckRGB[1][j][i][1] = ( unsigned char ) iG;
	  ucDuckRGB[1][j][i][2] = ( unsigned char ) iR;
	  ucDuckYUV[1][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 )  >> 8 ) + 16 );
	  ucDuckYUV[1][j][i][1] = ( unsigned char ) ( ( ( -38 * iR-74 * iG + 112 * iB + 128 )  >> 8 ) + 128 );
	  ucDuckYUV[1][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 )  >> 8 ) + 128 );
	  }
	}

    for ( j = 0; j < voDuck.iHeight; j++ )
	{
	ptrImage = ( BYTE * )imDuck3->Picture->Bitmap->ScanLine[j];
	for ( i=0; i<voDuck.iWidth; i++ )
	  {
	  iB = ptrImage[i*3];
	  iG = ptrImage[i*3+1];
	  iR = ptrImage[i*3+2];
	  ucDuckRGB[2][j][i][0] = ( unsigned char ) iB;
	  ucDuckRGB[2][j][i][1] = ( unsigned char ) iG;
	  ucDuckRGB[2][j][i][2] = ( unsigned char ) iR;
	  ucDuckYUV[2][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 )  >> 8 ) + 16 );
	  ucDuckYUV[2][j][i][1] = ( unsigned char ) ( ( ( -38 * iR-74 * iG + 112 * iB + 128 )  >> 8 ) + 128 );
	  ucDuckYUV[2][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 )  >> 8 ) + 128 );
	  }
	}
	voPoint.iX1 = 50;
	voPoint.iY1 = 50;
	voPoint.iWidth = 203;
	voPoint.iHeight = 203;
	voPoint.iX2 = voPoint.iX1 + voPoint.iWidth-1;
	voPoint.iY2 = voPoint.iY1 + voPoint.iWidth-1;
	voPoint.ucBackgroundRed = 255;
	voPoint.ucBackgroundGreen = 255;
	voPoint.ucBackgroundBlue = 255;

	voPoint.iX1 = 0;

	// 將影像元件中的色彩資料放到陣列中
	for (j = 0; j < voPoint.iHeight; j ++)
	{
	ptrImage = (BYTE*) imBall->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voPoint.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucPointRGB[j][i][0] = ( unsigned char ) iB;
		ucPointRGB[j][i][1] = ( unsigned char ) iG;
		ucPointRGB[j][i][2] = ( unsigned char ) iR;
		ucPointYUV[j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucPointYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucPointYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}

	voTree.iX1 = 0;
	voTree.iY1 = 0;
	voTree.iWidth = 640;
	voTree.iHeight =480;
	voTree.iX2 = voTree.iX1 + voTree.iWidth - 1;
	voTree.iY2 = voTree.iY1 + voTree.iHeight -1;

	for (j = 0; j < voTree.iHeight; j ++)
	{
	ptrImage = (BYTE*) imTree->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voTree.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucTreeRGB[j][i][0] = ( unsigned char ) iB;
		ucTreeRGB[j][i][1] = ( unsigned char ) iG;
		ucTreeRGB[j][i][2] = ( unsigned char ) iR;
		ucTreeYUV[j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucTreeYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucTreeYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	voTitle.iX1 = 0;
	voTitle.iY1 = 0;
	voTitle.iWidth = 640;
	voTitle.iHeight = 480;
	voTitle.iX2 = voTitle.iX1 + voTitle.iWidth - 1;
	voTitle.iY2 = voTitle.iY1 + voTitle.iHeight -1;
	for (j = 0; j < voTitle.iHeight; j ++)
	{
	ptrImage = (BYTE*) imTitle->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voTitle.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucTitleRGB[j][i][0] = ( unsigned char ) iB;
		ucTitleRGB[j][i][1] = ( unsigned char ) iG;
		ucTitleRGB[j][i][2] = ( unsigned char ) iR;
		ucTitleYUV[j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucTitleYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucTitleYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	voResult.iX1 = 0;
	voResult.iY1 = 0;
	voResult.iWidth = 640;
	voResult.iHeight = 480;
	voResult.iX2 = voResult.iX1 + voResult.iWidth - 1;
	voResult.iY2 = voResult.iY1 + voResult.iHeight -1;
	for (j = 0; j < voResult.iHeight; j ++)
	{
	ptrImage = (BYTE*) imOver->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voTitle.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucResultRGB[0][j][i][0] = ( unsigned char ) iB;
		ucResultRGB[0][j][i][1] = ( unsigned char ) iG;
		ucResultRGB[0][j][i][2] = ( unsigned char ) iR;
		ucResultYUV[0][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucResultYUV[0][j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucResultYUV[0][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
    for (j = 0; j < voResult.iHeight; j ++)
	{
	ptrImage = (BYTE*) imWin->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voTitle.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucResultRGB[1][j][i][0] = ( unsigned char ) iB;
		ucResultRGB[1][j][i][1] = ( unsigned char ) iG;
		ucResultRGB[1][j][i][2] = ( unsigned char ) iR;
		ucResultYUV[1][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucResultYUV[1][j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucResultYUV[1][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackMirror);

}
LRESULT CALLBACK FrameCallbackMirror ( HWND hwndCapture, PVIDEOHDR lpvhdr)
{

	iAimX=0,iAimY=0,iAimCounter=1;
	ptrCapture = lpvhdr->lpData;
	// iVideoMacroWidth: 表示一列有多少個宏像素 ( 視訊寬度的一半 )。
	// iRowBufferSize: 為一列所需的記憶體大小, 宏像素個數乘以 4 。
	// 處理鏡射問題。

	if(!voDuck.bVisible){
		voDuck.iX1 = -100;
		voDuck.iY1 = rand()%300;
		voDuck.iX2 = voDuck.iX1+voDuck.iWidth-1;
		voDuck.iY2 = voDuck.iY1+voDuck.iHeight-1;
		voDuck.iBlockX1 = voDuck.iX1/8;
		voDuck.iBlockX2 = voDuck.iX2/8;
		voDuck.iBlockY1 = voDuck.iY1/8;
		voDuck.iBlockY2 = voDuck.iY2/8;
		voDuck.bVisible = true;
		iID = rand()%3;
	}
	for ( j=0; j<iVideoHeight; j++  )
	{
		for ( i=0; i<iVideoMacroWidth/2; i++  )
		{
		// 第 i 個 macropixel 的交換對象是第 ( iVideoWidth / 2 ) - i 。
		iPartner = iVideoMacroWidth - i - 1;
		// 先將第 i 個 marcopixel 的四個位元組的色彩值暫存起來。
        ucTempY1 = ptrCapture[j*iRowBufferSize+i*4];
		ucTempU = ptrCapture[j*iRowBufferSize+i*4+1];
        ucTempY2 = ptrCapture[j*iRowBufferSize+i*4+2];
		ucTempV = ptrCapture[j*iRowBufferSize+i*4+3];
		// 把 Partner 的四個位元組的色彩值複製過來, 但 Y1 與 Y2 要互換位置。
		ptrCapture[j*iRowBufferSize+i*4] = ptrCapture[j*iRowBufferSize+iPartner*4+2];
		ptrCapture[j*iRowBufferSize+i*4+1] = ptrCapture[j*iRowBufferSize+iPartner*4+1];
		ptrCapture[j*iRowBufferSize+i*4+2] = ptrCapture[j*iRowBufferSize+iPartner*4];
		ptrCapture[j*iRowBufferSize+i*4+3] = ptrCapture[j*iRowBufferSize+iPartner*4+3];
		// 將暫存的色彩值放到 Partner 的緩衝區記憶體之中, 但 Y1 與 Y2 要互換位置。
		ptrCapture[j*iRowBufferSize+iPartner*4] = ucTempY2;
		ptrCapture[j*iRowBufferSize+iPartner*4+1] = ucTempU;
		ptrCapture[j*iRowBufferSize+iPartner*4+2] = ucTempY1;
		ptrCapture[j*iRowBufferSize+iPartner*4+3] = ucTempV;
		}
	}

    iFrameCounter++;

	if ( iFrameCounter > 30 )
	{
	// 完成累加 30 張影像, 計算平均影像。
		for ( j=0; j < iVideoHeight; j++ )
	  {
	  // 取得 imBackground 影像元件第 j 個 row 的儲存色彩値的記憶體起始位址。
	  ptrImage = ( BYTE * ) Form1->imBackground->Picture->Bitmap->ScanLine[j];
	  t = 0;
	  for ( i=0; i < iVideoWidth; i++ )
        {
        ucBackgroundYUV[j][i][0] = ( unsigned char ) ( iTempYUV[j][i][0] / 30 );
		ucBackgroundYUV[j][i][1] = ( unsigned char ) ( iTempYUV[j][i][1] / 30 );
        ucBackgroundYUV[j][i][2] = ( unsigned char ) ( iTempYUV[j][i][2] / 30 );
        // 將 YUV 色彩轉換成 RGB 色彩後, 存到 imBackgroung 中顯示出來。
        iY = ucBackgroundYUV[j][i][0];
		iU = ucBackgroundYUV[j][i][1];
        iV = ucBackgroundYUV[j][i][2];
		iC1 = iY - 16;
		iD = iU - 128;
		iE = iV - 128;
        // 處理紅色之色彩轉換。
        iR = ( 298 * iC1    + 409 * iE + 128 ) >> 8;
        // 檢查 iR 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
		if ( iR > 255 )
		  iR = 255;
		else if ( iR < 0 )
		  iR = 0;
		// 處理綠色之色彩轉換
        iG = ( 298 * iC1 - 100 * iD - 208 * iE + 128 ) >> 8;
		// 檢查 iG 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
        if ( iG > 255 )
		  iG = 255;
		else if ( iG < 0 )
          iG = 0;
		iB = ( 298 * iC1 + 516 * iD  + 128 ) >> 8;
		// 檢查 iB 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
		if ( iB > 255 )
          iB = 255;
        else if ( iB < 0 )
		  iB = 0;
// 存到 imBackground 中顯示出來。
		ptrImage[t + 2] = ( unsigned char ) iR;
		ptrImage[t + 1] = ( unsigned char ) iG;
        ptrImage[t ] = ( unsigned char ) iB;
		t = t + 3;
		}
	  }

	  Form1->imBackground->Refresh();

	}
  else // ( iFrameCount < 30 )
	{
	k = 0;
	for ( j=0; j < iVideoHeight; j++ )
	  for ( i=0; i < iVideoWidth; i=i+2 )
		{
		iY1 = ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		// 累加宏像素中的第一個像素 ( Pixel 1 )
		iTempYUV[j][i][0] = iTempYUV[j][i][0] + iY1;
		iTempYUV[j][i][1] = iTempYUV[j][i][1] + iU;
		iTempYUV[j][i][2] = iTempYUV[j][i][2] + iV;
		// 累加宏像素中的第二個像素 ( Pixel 2 )
		iTempYUV[j][i+1][0] = iTempYUV[j][i+1][0] + iY2;
		iTempYUV[j][i+1][1] = iTempYUV[j][i+1][1] + iU;
		iTempYUV[j][i+1][2] = iTempYUV[j][i+1][2] + iV;
		k = k + 4;
		if((i>=voTitle.iX1)&&(i<voTitle.iX2)&&(j>=voTitle.iY1)&&(j<voTitle.iY2))
			{
				if((ucTitleRGB[j-voTitle.iY1][i-voTitle.iX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucTitleRGB[j-voTitle.iY1][i-voTitle.iX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucTitleRGB[j-voTitle.iY1][i-voTitle.iX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = 255;
					ptrCapture[k+2] = 255;

				}

			}
	  }
	  Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
	}

	if(iFrameCounter>30){
        for(m=0;m<iVideoBlockHeight;m++)
		for(n=0;n<iVideoBlockWidth;n++)
		  iBlockChangeCounter[m][n]=0;
	k=0;
	for(j=0;j<iVideoHeight;j++){
		for(i=0;i<iVideoWidth;i+=2){
        m = j/8;
		n = i/8;
		if (( ptrCapture[k] >= 0 ) && ( ptrCapture[k] <=150 )
			&& ( ptrCapture[k+1] >= 0  ) && ( ptrCapture[k + 1] <= 255)
			&& ( ptrCapture[k+2] >= 0 ) && ( ptrCapture[k + 2] <= 150)
			&& ( ptrCapture[k+3] >= 170 ) && ( ptrCapture[k + 3] <= 255 ))
		{

		iY1 =  ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		iY1Difference = abs(ptrCapture[k]-ucBackgroundYUV[j][i][0]);
		iUDifference = abs(ptrCapture[k+1]-ucBackgroundYUV[j][i][1]);
		iY2Difference = abs(ptrCapture[k+2]-ucBackgroundYUV[j][i+1][0]);
		iVDifference = abs(ptrCapture[k+3]-ucBackgroundYUV[j][i][2]);
		iSquareDifferenceSum1 = iY1Difference*iY1Difference+iUDifference*iUDifference+iVDifference*iVDifference;
		iSquareDifferenceSum2 = iY2Difference*iY2Difference+iUDifference*iUDifference+iVDifference*iVDifference;

		if ( iSquareDifferenceSum1 > iThreshold )
			iBlockChangeCounter[m][n]++;
		if ( iSquareDifferenceSum2 > iThreshold )
			iBlockChangeCounter[m][n]++;
		iAimX+=i;
		iAimY+=j;
		iAimCounter++;
		bShoot = true;
		}
		if (( ptrCapture[k] >= 0 ) && ( ptrCapture[k] <=255 )
			&& ( ptrCapture[k+1] >= 180  ) && ( ptrCapture[k + 1] <= 255)
			&& ( ptrCapture[k+2] >= 0 ) && ( ptrCapture[k + 2] <= 255)
			&& ( ptrCapture[k+3] >= 90 ) && ( ptrCapture[k + 3] <= 135 ))
		{

		iY1 =  ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		iY1Difference = abs(ptrCapture[k]-ucBackgroundYUV[j][i][0]);
		iUDifference = abs(ptrCapture[k+1]-ucBackgroundYUV[j][i][1]);
		iY2Difference = abs(ptrCapture[k+2]-ucBackgroundYUV[j][i+1][0]);
		iVDifference = abs(ptrCapture[k+3]-ucBackgroundYUV[j][i][2]);
		iSquareDifferenceSum1 = iY1Difference*iY1Difference+iUDifference*iUDifference+iVDifference*iVDifference;
		iSquareDifferenceSum2 = iY2Difference*iY2Difference+iUDifference*iUDifference+iVDifference*iVDifference;

		if ( iSquareDifferenceSum1 > iThreshold )
			iBlockChangeCounter[m][n]++;
		if ( iSquareDifferenceSum2 > iThreshold )
			iBlockChangeCounter[m][n]++;
		iAimX+=i;
		iAimY+=j;
		iAimCounter++;
		bShoot = false;
		}



		k=k+4;
		}
	}
	iAimX/=iAimCounter;iAimY/=iAimCounter;
	k = 0;
	for ( j = 0; j < iVideoHeight; j++ )
	{
		for ( i=0; i < iVideoWidth; i = i + 2 )
		{
		m = j / 8;
		n = i / 8;
		if ( Form1->cbBlockDisplay->Checked )
		{
			ptrCapture[k+1] = 128;
			ptrCapture[k+3] = 128;
			if ( iBlockChangeCounter[m][n] > 31 )
			{
				ptrCapture[k] = 70;
				ptrCapture[k+2] = 110;
			}
			else
			{
				ptrCapture[k] = 16;
				ptrCapture[k+2] = 16;
			}
		}

		if((voDuck.bVisible ==true)&&(i>=voDuck.iX1)&&(i<voDuck.iX2)
			&&(j>=voDuck.iY1)&&(j<voDuck.iY2))
			{   if((ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][0]!=voDuck.ucBackgroundBlue)||
				(ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][1]!=voDuck.ucBackgroundGreen)||
				(ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][2]!=voDuck.ucBackgroundRed))
				{   ptrCapture[k] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][0];
					ptrCapture[k+1] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][1];
					ptrCapture[k+2] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1+1][0];
					ptrCapture[k+3] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][2];

				}

		}
		int iAimX1 = iAimX-(voPoint.iWidth/2),iAimX2 = iAimX+(voPoint.iWidth/2);
		int iAimY1 = iAimY-(voPoint.iHeight/2),iAimY2 = iAimY+(voPoint.iHeight/2);
		if((i>=iAimX1)&&(i<iAimX2)&&(j>=iAimY1)&&(j<iAimY2))
			{
				if((ucPointRGB[j-iAimY1][i-iAimX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucPointRGB[j-iAimY1][i-iAimX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucPointRGB[j-iAimY1][i-iAimX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = ucPointYUV[j-iAimY1][i-iAimX1][0];
					ptrCapture[k+1] = ucPointYUV[j-iAimY1][i-iAimX1][1];
					ptrCapture[k+2] = ucPointYUV[j-iAimY1][i-iAimX1+1][0];
					ptrCapture[k+3] = ucPointYUV[j-iAimY1][i-iAimX1][2];

				}

			}
		if((i>=voTree.iX1)&&(i<voTree.iX2)&&(j>=voTree.iY1)&&(j<voTree.iY2))
			{
				if((ucTreeRGB[j-voTree.iY1][i-voTree.iX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucTreeRGB[j-voTree.iY1][i-voTree.iX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucTreeRGB[j-voTree.iY1][i-voTree.iX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = ucTreeYUV[j-voTree.iY1][i-voTree.iX1][0];
					ptrCapture[k+1] = ucTreeYUV[j-voTree.iY1][i-voTree.iX1][1];
					ptrCapture[k+2] = ucTreeYUV[j-voTree.iY1][i-voTree.iX1+1][0];
					ptrCapture[k+3] = ucTreeYUV[j-voTree.iY1][i-voTree.iX1][2];

				}

			}
		if((i>=voResult.iX1)&&(i<voResult.iX2)&&(j>=voResult.iY1)&&(j<voResult.iY2)&&(iEnd==1))
			{
				if((ucResultRGB[0][j-voResult.iY1][i-voResult.iX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucResultRGB[0][j-voResult.iY1][i-voResult.iX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucResultRGB[0][j-voResult.iY1][i-voResult.iX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = 255;
					ptrCapture[k+2] = 255;

				}

			}
		if((i>=voResult.iX1)&&(i<voResult.iX2)&&(j>=voResult.iY1)&&(j<voResult.iY2)&&(iEnd==2))
			{
				if((ucResultRGB[1][j-voResult.iY1][i-voResult.iX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucResultRGB[1][j-voResult.iY1][i-voResult.iX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucResultRGB[1][j-voResult.iY1][i-voResult.iX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = 255;
					ptrCapture[k+2] = 255;

				}

			}
		k=k+4;
		}
	}
	voDuck.iX1 += voDuck.iDeltaX;
	voDuck.iX2 += voDuck.iDeltaX;
	voDuck.iY1 += voDuck.iDeltaY;
	voDuck.iY2 += voDuck.iDeltaY;

	voDuck.iBlockX1 = voDuck.iX1/8;
	voDuck.iBlockX2 = voDuck.iX2/8;
	voDuck.iBlockY1 = voDuck.iY1/8;
	voDuck.iBlockY2 = voDuck.iY2/8;


	k=0;

	for(m=0;m<iVideoBlockHeight;m++){
		for(n=0;n<iVideoBlockWidth;n++){

			if((voDuck.bVisible)&&(iBlockChangeCounter[m][n]>31)
				&&(m>voDuck.iBlockY1)&&(m<voDuck.iBlockY2)
				&&(n>voDuck.iBlockX1)&&(n<voDuck.iBlockX2)&&bShoot==true&&iEnd==0){
					if(iID==2)iScore+=20;
					if(iID==0)iScore-=10;
					if(iID==1)iEnd=1;
					voDuck.bVisible = false;
					m = iVideoBlockHeight;
					n = iVideoBlockWidth;
				}
			}

		}

	}
	if(voDuck.iX1>=640)voDuck.bVisible = false;
	if(iScore>=100){iEnd=2;}
	Form1->Label2->Caption = iScore;


return (0);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::miPreviewOnClick(TObject *Sender)
{
	capPreview(hwndVideo, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miPreviewOffClick(TObject *Sender)
{
	capPreview(hwndVideo,false);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miScaleOnClick(TObject *Sender)
{
     capPreviewScale(hwndVideo,true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miScaleOffClick(TObject *Sender)
{
	capPreviewScale(hwndVideo,false);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::mi320240Click(TObject *Sender)
{
	pnlCaptureWindow->Width = 340;
	pnlCaptureWindow->Height = 260;
	iCapWindowWidth = 320;
	iCapWindowHeight = 240;
	MoveWindow(hwndVideo, iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight, true);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::mi640480Click(TObject *Sender)
{
	pnlCaptureWindow->Width = 660;
	pnlCaptureWindow->Height = 500;
	iCapWindowWidth = 640;
	iCapWindowHeight = 480;
	MoveWindow(hwndVideo, iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight, true);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::mi1280960Click(TObject *Sender)
{
	pnlCaptureWindow->Width = 1300;
	pnlCaptureWindow->Height = 980;
	iCapWindowWidth = 1280;
	iCapWindowHeight = 960;
	MoveWindow(hwndVideo, iCapWindowX1, iCapWindowY1, iCapWindowWidth, iCapWindowHeight, true);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miDestroyDisconnectClick(TObject *Sender)
{
	capDriverDisconnect(hwndVideo);
	DestroyWindow(hwndVideo);
	pnlCaptureWindow->Width = 9;
	pnlCaptureWindow->Height = 9;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miCapabilityClick(TObject *Sender)
{
	tsDescription->Show();
    // 取得視訊裝置驅動程式的效能。
    if ( capDriverGetCaps ( hwndVideo , &cdcCapDriverCapability ,  sizeof ( cdcCapDriverCapability ) ) )
		{
		lblIndex->Caption = AnsiString ( "Driver Index:") + AnsiString ( cdcCapDriverCapability.wDeviceIndex ) ;
        if ( cdcCapDriverCapability.fHasOverlay )
            lblOverlay->Caption = AnsiString  ( " Can device overlay ?  Yes! " ) ;
        else lblOverlay->Caption = AnsiString ( " Can device overlay ? No! " ) ;
		// lblOverlay 為 Label 物件的 Name。

        if ( cdcCapDriverCapability.fHasDlgVideoSource )
            {
            // miVideoSource 為 MainMenu 物件的 MenuItem 之名稱。
			// 將 MenuItem miVideoSource元件設定成可以使用的狀態。
			miVideoSource->Enabled = true ;
			lblHasVideoSourceDialog->Caption = AnsiString ( " Has video source dialog ? Yes! " );
            }
        else
            {
// 將 MenuItem miVideoSource元件設定成失效, 無法使用的狀態。
            miVideoSource->Enabled = false ;
			lblHasVideoSourceDialog->Caption = AnsiString ( " Has video source dialog? No! " );
            }

        if ( cdcCapDriverCapability.fHasDlgVideoFormat )
            {
// miVideoFormat 為 MainMenu 物件的 MenuItem 之名稱。
// 將 MenuItem miVideoFormat元件設定成可以使用的狀態。
            miVideoFormat->Enabled = true ;
			lblHasVideoFormatDialog->Caption = AnsiString ( " Has video format dialog? Yes! " );
            }
        else
            {
            // 將 MenuItem miVideoFormat元件設定成失效, 無法使用的狀態。
            miVideoFormat->Enabled = false ;
			lblHasVideoFormatDialog->Caption = AnsiString ( " Has video format dialog ? No " ) ;
            }

        if ( cdcCapDriverCapability.fHasDlgVideoDisplay )
            {
            // miVideoDisplay 為 MainMenu 物件下的 MenuItem 之名稱。
            // 將 MenuItem miVideoDisplay 元件設定成可以使用的狀態。
            miVideoDisplay->Enabled = true ;
			lblHasVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? Yes " ) ;
            }
        else
            {
            // 將 MenuItem miVideoDisplay 元件設定成失效, 無法使用的狀態。
            miVideoDisplay->Enabled = false ;
			lblHasVideoDisplayDialog->Caption = AnsiString ( " Has video display dialog ? No " ) ;
            }

        if ( cdcCapDriverCapability.fCaptureInitialized )
			lblDriverReadyToCapture->Caption = AnsiString ( " Driver ready to capture ? Yes " ) ;
		else lblDriverReadyToCapture->Caption = AnsiString ( " Driver ready to capture ? No " ) ;

        if ( cdcCapDriverCapability.fCaptureInitialized )
			lblCanDriverMakepalette->Caption = AnsiString (" Can Driver make palette ? Yes " ) ;
		else lblCanDriverMakepalette->Caption = AnsiString ( " Can Driver make palette ? No " ) ;
    }
    else ShowMessage ( " Capture Window does not connect to any Capture Driver ! " ) ;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miVideoSourceClick(TObject *Sender)
{
	if(!capDlgVideoSource(hwndVideo))
		ShowMessage( " Can't open video source dialog !! " ) ;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miVideoFormatClick(TObject *Sender)
{
	if(!capDlgVideoFormat(hwndVideo))
        ShowMessage( " Can't open video format dialog ! ! " ) ;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miVideoDisplayClick(TObject *Sender)
{
	if (!capDlgVideoDisplay(hwndVideo))
		ShowMessage( " Can't open video display dialog ! ! " ) ;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miVideoCompressionClick(TObject *Sender)
{
    if ( !capDlgVideoCompression( hwndVideo )  )
        ShowMessage( " Can't open video compression dialog ! ! " );

}
//---------------------------------------------------------------------------
void __fastcall TForm1::miStatusClick(TObject *Sender)
{
	if(capGetStatus(hwndVideo,&cdcCapDriverStatus,sizeof(cdcCapDriverStatus))){
		lblImageWidth->Caption = "Image Width: "+AnsiString(cdcCapDriverStatus.uiImageWidth);
		lblImageHeight->Caption = "Image Height: "+AnsiString(cdcCapDriverStatus.uiImageHeight);
		if(cdcCapDriverStatus.fLiveWindow)lblLiveWindow->Caption = "Live Window? Yes!";
		else lblLiveWindow->Caption = "Live Window? No!";
		if(cdcCapDriverStatus.fOverlayWindow)lblOverlayWindow->Caption = "Overlay Window? Yes!";
		else lblOverlayWindow->Caption = "Overlay Window? No!";
		if(cdcCapDriverStatus.fScale)lblScale->Caption = "Scale? Yes!";
		else lblScale->Caption = "Scale? No!";
		lblScroll->Caption = "Scroll: ("+AnsiString(cdcCapDriverStatus.ptScroll.x)+","+AnsiString(cdcCapDriverStatus.ptScroll.y)+")";
		if(cdcCapDriverStatus.fUsingDefaultPalette)lblUsingDefaultPalette->Caption = "Using Default Palette? Yes!";
		else lblUsingDefaultPalette->Caption = "Using Default Palette? No!";
		if(cdcCapDriverStatus.fAudioHardware)lblAudioHardware->Caption = "Audio Hardware? Yes!";
		else lblAudioHardware->Caption = "Audio Hardware? No!";

		if(cdcCapDriverStatus.fCapFileExists)lblCaptureFileExists->Caption = "Capture File Exists? Yes!";
		else lblCaptureFileExists->Caption = "Capture File Exists? No!";

		lblCurrentVideoFrame->Caption = "Current Video Frame: "+AnsiString(cdcCapDriverStatus.dwCurrentVideoFrame);
		lblCurrentVideoFrameDropped->Caption = "Current Video Frame Dropped: "+AnsiString(cdcCapDriverStatus.dwCurrentVideoFramesDropped);
		lblCurrentWaveSamples->Caption = "Current Wave Samples: "+AnsiString(cdcCapDriverStatus.dwCurrentWaveSamples);
		lblCurrentTimeElapsedMS->Caption = "Current Time Elapsed MS: "+AnsiString(cdcCapDriverStatus.dwCurrentTimeElapsedMS);

		if(cdcCapDriverStatus.fCapturingNow)lblCapturingNow->Caption = "Capturing Now? Yes!";
		lblCapturingNow->Caption = "Capturing Now? No!";

		lblReturn->Caption = "Return: "+AnsiString(cdcCapDriverStatus.dwReturn);

		lblNumberOfVideoAllocated->Caption = "Number Of Video Allocated: " + AnsiString(cdcCapDriverStatus.wNumVideoAllocated);
		lblNumberOfAudioAllocated->Caption = "Number Of Audio Allocated: " + AnsiString(cdcCapDriverStatus.wNumAudioAllocated);


	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnGrabFrameClick(TObject *Sender)
{
	if(!capGrabFrame(hwndVideo))
        ShowMessage("Can't Grab Frame To The Video Frame Buffer!");

}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnGrabFrameNoStopClick(TObject *Sender)
{
	if(!capGrabFrameNoStop(hwndVideo))
		ShowMessage("Can't Grab Frame To The Video Frame No Stop!");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnEditCopyClick(TObject *Sender)
{
	if(!capEditCopy(hwndVideo))
      ShowMessage("Can't Copy The Video Frame Buffer To The Clipboard!");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnFileSaveDIBClick(TObject *Sender)
{
	if(SavePictureDialog1->Execute()){
		if(!capFileSaveDIB(hwndVideo,&SavePictureDialog1->FileName[1]))
			ShowMessage("Can't Save the ClipBoard Image!");
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::miBallClick(TObject *Sender)
{
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;

	voPoint.iX1 = 50;
	voPoint.iY1 = 50;
	voPoint.iWidth = 203;
	voPoint.iHeight = 203;
	voPoint.iX2 = voPoint.iX1 + voPoint.iWidth-1;
	voPoint.iY2 = voPoint.iY1 + voPoint.iWidth-1;
	voPoint.ucBackgroundRed = 255;
	voPoint.ucBackgroundGreen = 255;
	voPoint.ucBackgroundBlue = 255;

	// 將影像元件中的色彩資料放到陣列中
	for (j = 0; j < voPoint.iHeight; j ++)
	{
	ptrImage = (BYTE*) imBall->Picture->Bitmap->ScanLine[j];
	for ( i = 0; i<voPoint.iWidth; i++ )
		{
		iB = ptrImage[i*3];
		iG = ptrImage[i*3+1];
		iR = ptrImage[i*3+2];
		ucPointRGB[j][i][0] = ( unsigned char ) iB;
		ucPointRGB[j][i][1] = ( unsigned char ) iG;
		ucPointRGB[j][i][2] = ( unsigned char ) iR;
		ucPointYUV[j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 ) + 16 );
		ucPointYUV[j][i][1] = ( unsigned char ) ( ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 ) + 128 );
		ucPointYUV[j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 ) + 128 );
		}
	}
	// 開啟預覽回呼函式 FrameCallbackGecko。
	capSetCallbackOnFrame ( hwndVideo , &FrameCallbackPoint );


}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackPoint(HWND hwndVideo , PVIDEOHDR lpvhdr){
	ptrCapture = lpvhdr->lpData;
	k=0;
	for(j=0;j<iVideoHeight;j++){
		for(i=0;i<iVideoWidth;i+=2){
			if((i>=voPoint.iX1)&&(i<voPoint.iX2)&&(j>=voPoint.iY1)&&(j<voPoint.iY2)){
				if((ucPointRGB[j-voPoint.iY1][i-voPoint.iX1][0]!=voPoint.ucBackgroundBlue)
				&&(ucPointRGB[j-voPoint.iY1][i-voPoint.iX1][1]!=voPoint.ucBackgroundGreen)
				&&(ucPointRGB[j-voPoint.iY1][i-voPoint.iX1][2]!=voPoint.ucBackgroundRed)){
					ptrCapture[k] = ucPointYUV[j-voPoint.iY1][i-voPoint.iX1][0];
					ptrCapture[k+1] = ucPointYUV[j-voPoint.iY1][i-voPoint.iX1][1];
					ptrCapture[k+2] = ucPointYUV[j-voPoint.iY1][i-voPoint.iX1][0];
					ptrCapture[k+3] = ucPointYUV[j-voPoint.iY1][i-voPoint.iX1][2];

				}

			}
			k=k+4;
		}


	}

	return (0);
}
void __fastcall TForm1::miSpecificColorClick(TObject *Sender)
{
	iFrameCounter=0;
	iVideoFormatSize = capGetVideoFormatSize (hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;

	iR = 0;
	iG = 255;    // 綠色。
	iB = 0;
	iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 )  + 16;
	iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 )  + 128;
	iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 )  + 128;
	iMinY=0;
	iMaxY=255;
	iMinU=0;
	iMaxU=255;
	iMinV=0;
	iMaxV=255;
	capSetCallbackOnFrame (hwndVideo, &FrameCallbackSpecificColorFiltering);

}
LRESULT CALLBACK FrameCallbackSpecificColorFiltering ( HWND hwndVideo, PVIDEOHDR lpvhdr )
{
  ptrCapture = lpvhdr->lpData;
  iFrameCounter++;
  k = 0;
  for ( j = 0; j < iVideoHeight; j++ )
	{
	for ( i = 0; i < iVideoMacroWidth; i++ )
	  {
	  if (( ptrCapture[k] >= iMinY ) && ( ptrCapture[k] <= iMaxY )
	  && ( ptrCapture[k+1] >= iMinU ) && ( ptrCapture[k + 1] <= iMaxU )
	  && ( ptrCapture[k+2] >= iMinY ) && ( ptrCapture[k + 2] <= iMaxY )
	  && ( ptrCapture[k+3] >= iMinV ) && ( ptrCapture[k + 3] <= iMaxV ))
		{
        ptrCapture[k + 1] = iU;
        ptrCapture[k + 3] = iV;
		 }
	  k = k + 4;
	  }
	}

  Form1->lblFrameCounter->Caption = IntToStr(iFrameCounter);
  return (0);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::edMinYChange(TObject *Sender)
{
	iMinY = StrToIntDef(edMinY->Text,0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxYChange(TObject *Sender)
{
	iMaxY = StrToIntDef(edMaxY->Text,255);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMinUChange(TObject *Sender)
{
	iMinU = StrToIntDef(edMinU->Text,90);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxUChange(TObject *Sender)
{
	iMaxU = StrToIntDef(edMaxU->Text,120);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMinVChange(TObject *Sender)
{
	iMinV = StrToIntDef(edMinV->Text,90);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::edMaxVChange(TObject *Sender)
{
	iMaxV = StrToIntDef(edMaxV->Text,120);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miSkinColorClick(TObject *Sender)
{
	iVideoFormatSize = capGetVideoFormatSize (hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo, iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;

	iR = 0;
	iG = 255;    // 綠色。
	iB = 0;
	iY = ( ( 66 * iR + 129 * iG + 25 * iB + 128 ) >> 8 )  + 16;
	iU = ( ( -38 * iR - 74 * iG + 112 * iB + 128 ) >> 8 )  + 128;
	iV = ( ( 112 * iR - 94 * iG - 18 * iB + 128 ) >> 8 )  + 128;

	capSetCallbackOnFrame (hwndVideo, &FrameCallbackSkinColorFiltering);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackSkinColorFiltering (HWND hwndVideo, PVIDEOHDR lpvhdr)
{
	ptrCapture = lpvhdr->lpData;

	k = 0;
	for ( j = 0; j < iVideoHeight; j++ )
	{
		for ( i = 0; i < iVideoMacroWidth; i++ )
		{
			if (( ptrCapture[k] >= 0 ) && ( ptrCapture[k] <=255 )
			&& ( ptrCapture[k+1] >= 0  ) && ( ptrCapture[k + 1] <= 255)
			&& ( ptrCapture[k+2] >= 0 ) && ( ptrCapture[k + 2] <= 255)
			&& ( ptrCapture[k+3] >= 170 ) && ( ptrCapture[k + 3] <= 255 ))
			{
			ptrCapture[k + 1] = iU;
			ptrCapture[k + 3] = iV;
			}
		k = k + 4;
		}
	}


	return (0);
}




void __fastcall TForm1::miBackgroundClick(TObject *Sender)
{

	iFrameCounter=0;
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
  // 針對用來存放背景影像及累加影像的陣列設定初始値。
	for ( j = 0; j < iVideoHeight; j++ )
		for ( i = 0; i < iVideoWidth; i++ )
		{
		// RGB 色彩模式背景影像。
		ucBackground[j][i][0] = 0;
		ucBackground[j][i][1] = 0;
		ucBackground[j][i][2] = 0;
		iTemp[j][i][0] = 0;
		iTemp[j][i][1] = 0;
		iTemp[j][i][2] = 0;
		// YUV 色彩模式背景影像。
		ucBackgroundYUV[j][i][0] = 0;
		ucBackgroundYUV[j][i][1] = 0;
		ucBackgroundYUV[j][i][2] = 0;
		iTempYUV[j][i][0] = 0;
		iTempYUV[j][i][1] = 0;
		iTempYUV[j][i][2] = 0;
		}
	// 針對用來顯示背景影像的影像元件設定各項屬性 ( Property ) 値。
	imBackground->Width = iVideoWidth;
	imBackground->Height = iVideoHeight;
	imBackground->Picture->Bitmap->Width = iVideoWidth;
	imBackground->Picture->Bitmap->Height = iVideoHeight;
	imBackground->Picture->Bitmap->PixelFormat = pf24bit;
	iThreshold=1600;
	// 開啟預覽回呼函式 ( 設定為 FrameCallbackBackgroundYUY2 ) 。
	//capSetCallbackOnFrame ( hwndVideo, &FrameCallbackBackgroundYUY2 );

}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackBackgroundYUY2(HWND hwndVideo, PVIDEOHDR lpvhdr)
{
  iFrameCounter++;

  if ( iFrameCounter > 30 )
	{
	// 完成累加 30 張影像, 計算平均影像。
	for ( j=0; j < iVideoHeight; j++ )
	  {
	  // 取得 imBackground 影像元件第 j 個 row 的儲存色彩値的記憶體起始位址。
	  ptrImage = ( BYTE * ) Form1->imBackground->Picture->Bitmap->ScanLine[j];
	  t = 0;
	  for ( i=0; i < iVideoWidth; i++ )
        {
        ucBackgroundYUV[j][i][0] = ( unsigned char ) ( iTempYUV[j][i][0] / 30 );
		ucBackgroundYUV[j][i][1] = ( unsigned char ) ( iTempYUV[j][i][1] / 30 );
        ucBackgroundYUV[j][i][2] = ( unsigned char ) ( iTempYUV[j][i][2] / 30 );
        // 將 YUV 色彩轉換成 RGB 色彩後, 存到 imBackgroung 中顯示出來。
        iY = ucBackgroundYUV[j][i][0];
		iU = ucBackgroundYUV[j][i][1];
        iV = ucBackgroundYUV[j][i][2];
		iC1 = iY - 16;
		iD = iU - 128;
		iE = iV - 128;
        // 處理紅色之色彩轉換。
        iR = ( 298 * iC1    + 409 * iE + 128 ) >> 8;
        // 檢查 iR 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
		if ( iR > 255 )
		  iR = 255;
		else if ( iR < 0 )
		  iR = 0;
		// 處理綠色之色彩轉換
        iG = ( 298 * iC1 - 100 * iD - 208 * iE + 128 ) >> 8;
		// 檢查 iG 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
        if ( iG > 255 )
		  iG = 255;
		else if ( iG < 0 )
          iG = 0;
        iB = ( 298 * iC1 + 516 * iD     + 128 ) >> 8;
		// 檢查 iB 是否發生溢位 ? 如果超過 255 表示發生溢位, 將其值設定為 255。
		if ( iB > 255 )
          iB = 255;
        else if ( iB < 0 )
		  iB = 0;
// 存到 imBackgroung 中顯示出來。
		ptrImage[t + 2] = ( unsigned char ) iR;
		ptrImage[t + 1] = ( unsigned char ) iG;
        ptrImage[t ] = ( unsigned char ) iB;
        t = t + 3;
		}
	  }
	  Form1->imBackground->Refresh();
	  capSetCallbackOnFrame ( hwndVideo, NULL );
	}
  else // ( iFrameCount < 30 )
	{
	k = 0;
	for ( j=0; j < iVideoHeight; j++ )
	  for ( i=0; i < iVideoWidth; i=i+2 )
		{
		iY1 = ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		// 累加宏像素中的第一個像素 ( Pixel 1 )
		iTempYUV[j][i][0] = iTempYUV[j][i][0] + iY1;
		iTempYUV[j][i][1] = iTempYUV[j][i][1] + iU;
		iTempYUV[j][i][2] = iTempYUV[j][i][2] + iV;
		// 累加宏像素中的第二個像素 ( Pixel 2 )
		iTempYUV[j][i+1][0] = iTempYUV[j][i+1][0] + iY2;
		iTempYUV[j][i+1][1] = iTempYUV[j][i+1][1] + iU;
		iTempYUV[j][i+1][2] = iTempYUV[j][i+1][2] + iV;
		k = k + 4;
	  }
	}
  Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
  return 0;
}

void __fastcall TForm1::miDDColorModeClick(TObject *Sender)
{
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight,
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize ( hwndVideo );
	capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceColorYUY2 ) 。
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackDifferenceColorYUY2);

}
LRESULT CALLBACK FrameCallbackDifferenceColorYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  // 設定 ptrCapture 指向視訊擷取視窗的緩衝區記憶體的起始位址。
  ptrCapture = lpvhdr->lpData;
  // 開始進行色彩差異之計算。
  k = 0;
  for ( j = 0; j < iVideoHeight; j++ )
    {
    for ( i = 0; i < iVideoWidth; i = i + 2 )
      {
	  // 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes 。
      iY1 = ptrCapture[k];
      iU = ptrCapture[k+1];
      iY2 = ptrCapture[k+2];
      iV = ptrCapture[k+3];
// 計算差異値。
      iY1Difference = abs ( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
      iUDifference = ucBackgroundYUV[j][i][1] - ptrCapture[k + 1];
      iY2Difference = abs ( ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0] );
      iVDifference = ucBackgroundYUV[j][i+1][2] - ptrCapture[k+3];
 // 將差異値以彩色模式顯示在視訊擷取視窗。
      ptrCapture[k ] = iY1Difference + 16;
	  ptrCapture[k+1] = iUDifference + 128;
      ptrCapture[k+2] = iY2Difference + 16;
      ptrCapture[k+3] = iVDifference + 128;
      // 更新 k 値, 準備處理下一個宏像素( macropixel )
      k = k + 4;
      }
    }
  Form1->lblFrameCounter->Caption = AnsiString(iFrameCounter);
  return 0;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::miBinaryModeClick(TObject *Sender)
{
    iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight，
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo );
	capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceBinaryYUY2 )
	capSetCallbackOnFrame(hwndVideo, &FrameCallbackDifferenceBinaryYUY2);

}
LRESULT CALLBACK FrameCallbackDifferenceBinaryYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
  iFrameCounter++;
  // 設定 ptrCapture 指向是訊擷取視窗的緩衝區記憶體的起始位址。
  ptrCapture = lpvhdr->lpData;

  k = 0;
  for (j=0; j < iVideoHeight; j++ )
    {
    for (i=0; i < iVideoWidth; i=i+2)
      {
      // 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes。
      iY1 = ptrCapture[k];
      iU = ptrCapture[k+1];
      iY2 = ptrCapture[k+2];
      iV = ptrCapture[k+3];
      // 計算差異値 。
      iY1Difference = ptrCapture[k] - ucBackgroundYUV[j][i][0];
      iUDifference = ptrCapture[k+1] - ucBackgroundYUV[j][i][1];
      iY2Difference =ptrCapture[k+2] - ucBackgroundYUV[j][i+1][0];
      iVDifference = ptrCapture[k+3] - ucBackgroundYUV[j][i+1][2];
      // 分別計算 2 個pixel 差異値的平方和。
      iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
      // 不管是黑或白, U, V 都是 128。
      ptrCapture[k+1] = 128;
      ptrCapture[k+3] = 128;
      // 設定 pixel 1 的 Y, 即 Y1
	  if ( iSquareDifferenceSum1 > iThreshold )
        ptrCapture[k] = 255;
      else
        ptrCapture[k] = 16;
      // 設定 pixel 2 的 Y, 即 Y2
      if  ( iSquareDifferenceSum2 > iThreshold )
          ptrCapture[k+2] = 255;
      else
          ptrCapture[k+2] = 16;
       // 更新 k 値, 準備處理下一個 macropixel。
k = k + 4;
}
    }
  Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
  return 0;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::edThresholdChange(TObject *Sender)
{
	iThreshold = StrToIntDef (edThreshold->Text, 1600 );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::miBlockModeClick(TObject *Sender)
{
	iFrameCounter = 0;
	// 因為 FrameCallBackGrayMode 需要用到 iVideoWidth, iVideoHeight，
	// 所以必須事先使用 capGetVideoFormat 來取得視訊內容的高與寬。
	iVideoFormatSize = capGetVideoFormatSize ( hwndVideo );
	capGetVideoFormat ( hwndVideo, &bmiBitmapInfo, iVideoFormatSize );
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	// 開啟預覽回呼函式 ( 設定為 FrameCallbackDifferenceBlockBinaryYUY2 )
	capSetCallbackOnFrame ( hwndVideo, &FrameCallbackDifferenceBlockYUY2 );

}
LRESULT CALLBACK FrameCallbackDifferenceBlockYUY2 ( HWND hwndCapture, PVIDEOHDR lpvhdr )
{
iFrameCounter++;
ptrCapture = lpvhdr->lpData;
// 重設區塊累加陣列。
for (m=0; m<60; m++ )
	for (n=0; n<80; n++ )
        iBlockChangeCounter[m][n] = 0;

k = 0;
for ( j = 0; j < iVideoHeight; j++ )
    {
    for ( i = 0; i < iVideoWidth; i = i + 2 )
        {
        // 計算目前的 j, i 這個像素是屬於哪一個 block 的累加範圍。
        m = j / 8;
        n = i / 8;
        // 每次處理 1 個 macropixel, 共 2 個 pixels, 4 bytes。
        iY1 = ptrCapture[k];
        iU = ptrCapture[k + 1];
        iY2 = ptrCapture[k + 2];
        iV = ptrCapture[k + 3];
        // 計算差異値
        iY1Difference = abs ( ptrCapture[k] - ucBackgroundYUV[j][i][0] );
        iUDifference = abs ( ptrCapture[k + 1] - ucBackgroundYUV[j][i][1] );
        iY2Difference = abs ( ptrCapture[k + 2] - ucBackgroundYUV[j][i + 1][0] );
        iUDifference = abs ( ptrCapture[k + 3] - ucBackgroundYUV[j][i + 1][2] );
        // 分別計算 2 個pixel 差異値的平方和
        iSquareDifferenceSum1 = iY1Difference * iY1Difference + iUDifference * iUDifference + iVDifference * iVDifference;
        iSquareDifferenceSum2 = iY2Difference * iY2Difference + iUDifference * iUDifference + iVDifference * iVDifference;
        // 累加每個區塊中的差異像素
        if ( iSquareDifferenceSum1 > iThreshold )
            iBlockChangeCounter[m][n]++;
        if ( iSquareDifferenceSum2 > iThreshold )
            iBlockChangeCounter[m][n]++;
        // 更新 k 値, 準備處理下一個 macropixel
        k = k + 4;
      }
    }
  // 顯示黑白區塊。
  k = 0;
  for ( j=0; j<iVideoHeight; j++ )
    {
    for ( i=0; i<iVideoWidth; i=i+2 )
      {
      // 計算目前的 j, i 這個像素是屬於哪一個 block 的範圍。
      m = j / 8;
      n = i / 8;
      // 不管是黑或白, U, V 都是 128
      ptrCapture[k+1] = 128;
      ptrCapture[k+3] = 128;
      // Block 中白點的個數超過一半就整個區塊塗成白色。
      if ( iBlockChangeCounter[m][n] > 31 )
        {
        ptrCapture[k] = 255;
        ptrCapture[k+2] = 255;
        }
      else
        {
        ptrCapture[k] = 16;
        ptrCapture[k+2] = 16;
        }
    k = k + 4;
    }
     }

Form1->lblFrameCounter->Caption = AnsiString ( iFrameCounter );
return 0;
}

//---------------------------------------------------------------------------


void __fastcall TForm1::miPlay1Click(TObject *Sender)
{
	iVideoFormatSize = capGetVideoFormatSize(hwndVideo);
	capGetVideoFormat(hwndVideo, &bmiBitmapInfo,iVideoFormatSize);
	iVideoWidth = bmiBitmapInfo.bmiHeader.biWidth;
	iVideoHeight = bmiBitmapInfo.bmiHeader.biHeight;
	iVideoMacroWidth = iVideoWidth / 2;
	iVideoBlockWidth = iVideoWidth / 8;
	iVideoBlockHeight = iVideoHeight / 8;

	// 重設亂數產生器
	iTime = clock();
	srand( iTime );


    voDuck.iX1 = -100;
	voDuck.iY1 = 50;
	voDuck.iWidth = 112;
	voDuck.iHeight = 112;
	voDuck.iX2 = voDuck.iX1+voDuck.iWidth-1;
	voDuck.iY2 = voDuck.iY1+voDuck.iHeight-1;
	voDuck.iBlockX1 = voDuck.iX1/8;
	voDuck.iBlockX2 = voDuck.iX2/8;
	voDuck.iBlockY1 = voDuck.iY1/8;
	voDuck.iBlockY2 = voDuck.iY2/8;
	voDuck.bVisible = true;
	voDuck.ucBackgroundRed = 255;
	voDuck.ucBackgroundGreen = 255;
	voDuck.ucBackgroundBlue = 255;
	for ( j = 0; j < voDuck.iHeight; j++ )
	{
	ptrImage = ( BYTE * )imDuck1->Picture->Bitmap->ScanLine[j];
	for ( i=0; i<voDuck.iWidth; i++ )
	  {
	  iB = ptrImage[i*3];
	  iG = ptrImage[i*3+1];
	  iR = ptrImage[i*3+2];
	  ucDuckRGB[0][j][i][0] = ( unsigned char ) iB;
	  ucDuckRGB[0][j][i][1] = ( unsigned char ) iG;
	  ucDuckRGB[0][j][i][2] = ( unsigned char ) iR;
	  ucDuckYUV[0][j][i][0] = ( unsigned char ) ( ( ( 66 * iR + 129 * iG + 25 * iB + 128 )  >> 8 ) + 16 );
	  ucDuckYUV[0][j][i][1] = ( unsigned char ) ( ( ( -38 * iR-74 * iG + 112 * iB + 128 )  >> 8 ) + 128 );
	  ucDuckYUV[0][j][i][2] = ( unsigned char ) ( ( ( 112 * iR - 94 * iG - 18 * iB + 128 )  >> 8 ) + 128 );
	  }
	}

	capSetCallbackOnFrame(hwndVideo,&FrameCallbackDuck6B);
}
LRESULT CALLBACK FrameCallbackDuck ( HWND hwndCapture, PVIDEOHDR lpvhdr ){
	iFrameCounter++;
	ptrCapture = lpvhdr->lpData;

	for(m=0;m<iVideoBlockHeight;m++)
		for(n=0;n<iVideoBlockWidth;n++)
		  iBlockChangeCounter[m][n]=0;

	k=0;
	for(j=0;j<iVideoHeight;j++){
		for(i=0;i<iVideoWidth;i+=2){
		m = j/8;
		n = i/8;
		iY1 =  ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		iY1Difference = abs(ptrCapture[k]-ucBackgroundYUV[j][i][0]);
		iUDifference = abs(ptrCapture[k+1]-ucBackgroundYUV[j][i][1]);
		iY2Difference = abs(ptrCapture[k+2]-ucBackgroundYUV[j][i+1][0]);
		iVDifference = abs(ptrCapture[k+3]-ucBackgroundYUV[j][i][2]);

		iSquareDifferenceSum1 = iY1Difference*iY1Difference+iUDifference*iUDifference+iVDifference*iVDifference;
		iSquareDifferenceSum2 = iY2Difference*iY2Difference+iUDifference*iUDifference+iVDifference*iVDifference;

		if(iSquareDifferenceSum1>iThreshold)iBlockChangeCounter[m][n]++;
		if(iSquareDifferenceSum2>iThreshold)iBlockChangeCounter[m][n]++;



			if((voDuck.bVisible ==true)&&(i>=voDuck.iX1)&&(i<voDuck.iX2)
			&&(j>=voDuck.iY1)&&(j<voDuck.iY2))
			{   if((ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][0]!=voDuck.ucBackgroundBlue)||
				(ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][1]!=voDuck.ucBackgroundGreen)||
				(ucDuckRGB[iID][j-voDuck.iY1][i-voDuck.iX1][2]!=voDuck.ucBackgroundRed))
				{   ptrCapture[k] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][0];
					ptrCapture[k+1] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][1];
					ptrCapture[k+2] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1+1][0];
					ptrCapture[k+3] = ucDuckYUV[iID][j-voDuck.iY1][i-voDuck.iX1][2];

				}

		}

		k=k+4;
		}
	}
      k = 0;


	k=0;
	for(m=0;m<iVideoBlockHeight;m++){
		for(n=0;n<iVideoBlockWidth;n++){

			if((voDuck.bVisible)&&(iBlockChangeCounter[m][n]>31)
				&&(m>voDuck.iBlockY1)&&(m<voDuck.iBlockY2)
				&&(n>voDuck.iBlockX1)&&(n<voDuck.iBlockX2)){
					voDuck.bVisible = false;
					m = iVideoBlockHeight;
					n = iVideoBlockWidth;
				}
			}

	}
	return 0;
}

void __fastcall TForm1::miReplayClick(TObject *Sender)
{
	voDuck.iX1 = -100;
		voDuck.iY1 = rand()%355;
		voDuck.iX2 = voDuck.iX1+voDuck.iWidth-1;
		voDuck.iY2 = voDuck.iY1+voDuck.iHeight-1;
		voDuck.iBlockX1 = voDuck.iX1/8;
		voDuck.iBlockX2 = voDuck.iX2/8;
		voDuck.iBlockY1 = voDuck.iY1/8;
		voDuck.iBlockY2 = voDuck.iY2/8;
		voDuck.bVisible = true;
		iID = rand()%2;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK FrameCallbackDuck6B ( HWND hwndCapture, PVIDEOHDR lpvhdr ){
	iFrameCounter++;
	//ptrCapture = lpvhdr->lpData;

	for(m=0;m<iVideoBlockHeight;m++)
		for(n=0;n<iVideoBlockWidth;n++)
		  iBlockChangeCounter[m][n]=0;
	k=0;
	for(j=0;j<iVideoHeight;j++){
		for(i=0;i<iVideoWidth;i+=2){
		m = j/8;
		n = i/8;
		iY1 =  ptrCapture[k];
		iU = ptrCapture[k+1];
		iY2 = ptrCapture[k+2];
		iV = ptrCapture[k+3];
		iY1Difference = abs(ptrCapture[k]-ucBackgroundYUV[j][i][0]);
		iUDifference = abs(ptrCapture[k+1]-ucBackgroundYUV[j][i][1]);
		iY2Difference = abs(ptrCapture[k+2]-ucBackgroundYUV[j][i+1][0]);
		iVDifference = abs(ptrCapture[k+3]-ucBackgroundYUV[j][i][2]);

		iSquareDifferenceSum1 = iY1Difference*iY1Difference+iUDifference*iUDifference+iVDifference*iVDifference;
		iSquareDifferenceSum2 = iY2Difference*iY2Difference+iUDifference*iUDifference+iVDifference*iVDifference;

		if ( iSquareDifferenceSum1 > iThreshold )
			iBlockChangeCounter[m][n]++;
		if ( iSquareDifferenceSum2 > iThreshold )
			iBlockChangeCounter[m][n]++;

		k=k+4;
		}
	}

	k = 0;
	for ( j = 0; j < iVideoHeight; j++ )
    {
		for ( i=0; i < iVideoWidth; i = i + 2 )
		{
		m = j / 8;
		n = i / 8;
		if ( Form1->cbBlockDisplay->Checked )
		{
			ptrCapture[k+1] = 128;
			ptrCapture[k+3] = 128;
			if ( iBlockChangeCounter[m][n] > 31 )
			{
				ptrCapture[k] = 255;
				ptrCapture[k+2] = 255;
			}
			else
			{
				ptrCapture[k] = 16;
				ptrCapture[k+2] = 16;
			}
		}
		for(m=0;m<iVideoBlockHeight;m++){
		for(n=0;n<iVideoBlockWidth;n++){

			if((voDuck.bVisible)&&(iBlockChangeCounter[m][n]>31)
				&&(m>voDuck.iBlockY1)&&(m<voDuck.iBlockY2)
				&&(n>voDuck.iBlockX1)&&(n<voDuck.iBlockX2)&&bShoot==true){
					if(iID==0)
					voDuck.bVisible = false;
					m = iVideoBlockHeight;
					n = iVideoBlockWidth;
				}
			}

		}
		k=k+4;
		}
		}
	k=0;
	for(m=0;m<iVideoBlockHeight;m++){
		for(n=0;n<iVideoBlockWidth;n++){

			if((voDuck.bVisible)&&(iBlockChangeCounter[m][n]>31)
				&&(m>voDuck.iBlockY1)&&(m<voDuck.iBlockY2)
				&&(n>voDuck.iBlockX1)&&(n<voDuck.iBlockX2)){
					voDuck.bVisible = false;
					m = iVideoBlockHeight;
					n = iVideoBlockWidth;
				}
			}

	}
	return 0;

}





void __fastcall TForm1::miMediaPlayerClick(TObject *Sender)
{
	if (OpenDialog1->Execute())
	MediaPlayer1->FileName = OpenDialog1->FileName;
}
//---------------------------------------------------------------------------




