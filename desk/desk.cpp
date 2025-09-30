#include "pch.h"
#include "theme.h"
#include "ThemesPage.h"
#include "BackgroundPage.h"
#include "ScreensaverPage.h"
#include "AppearancePage.h"
#include "SettingsPage.h"
#include "desk.h"
#include "helper.h"
#include "uxtheme.h"

HINSTANCE g_hinst;
IThemeManager2* pThemeManager = NULL;
IDesktopWallpaper* pDesktopWallpaper = NULL;
ULONG_PTR gdiplusToken;

IUnknown* currentITheme;
THEMEINFO* selectedTheme = new THEMEINFO();
BOOL selectionPicker;
PROCESS_INFORMATION pi;

const IID IID_IThemeManager2 = { 0xc1e8c83e, 0x845d, 0x4d95, {0x81, 0xdb, 0xe2, 0x83, 0xfd, 0xff, 0xc0, 0x00} };

void PropertySheetMoment(LPWSTR lpCmdLine)
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT hr = CoCreateInstance(CLSID_ThemeManager2, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pThemeManager));
	pThemeManager->Init(ThemeInitNoFlags);

	hr = CoCreateInstance(CLSID_DesktopWallpaper, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));

#ifndef NDEBUG
	AllocConsole();
	FILE* pFile;
	freopen_s(&pFile, "CONOUT$", "w", stdout);
#endif

	printf("Hello world!\n");

	WTL::CPropertySheet sheet(L"Display Properties");

	sheet.m_psh.dwFlags |= PSH_USEICONID;
	sheet.m_psh.pszIcon = MAKEINTRESOURCE(IDI_ICON1);

	// initialize theme manager
	InitUxtheme();

	CThemeDlgProc themedlg;
	sheet.AddPage(themedlg);

	CBackgroundDlgProc backgrounddlg;
	sheet.AddPage(backgrounddlg);

	CScrSaverDlgProc screensaverdlg;
	sheet.AddPage(screensaverdlg);

	CAppearanceDlgProc appearancedlg;
	sheet.AddPage(appearancedlg);

	CSettingsDlgProc settingsdlg;
	sheet.AddPage(settingsdlg);

	//show
	sheet.DoModal();

	// cleanup
	Gdiplus::GdiplusShutdown(gdiplusToken);
	pThemeManager->Release();
	pDesktopWallpaper->Release();
	if (currentITheme) currentITheme->Release();
	free(selectedTheme);
	_TerminateProcess(pi);
}

extern "C" LONG APIENTRY CPlApplet(
	HWND hwndCPL,			// handle of Control Panel window
	UINT uMsg,				// message
	LONG_PTR lParam1,       // first message parameter
	LONG_PTR lParam2        // second message parameter
)
{
	LPCPLINFO lpCPlInfo;
	LONG retCode = 0;

	switch (uMsg)
	{
	case CPL_INIT:
		return TRUE;

	case CPL_GETCOUNT:
		return 1L;

	case CPL_INQUIRE:
		lpCPlInfo = (LPCPLINFO)lParam2;
		lpCPlInfo->idIcon = IDI_ICON1;
		lpCPlInfo->idName = IDS_THEMESCPL;
		lpCPlInfo->idInfo = IDS_THEMESDESC;
		return (LONG)TRUE;

	case CPL_DBLCLK:
		lParam2 = 0L;
		// fall through
	case CPL_STARTWPARMS:
		PropertySheetMoment((LPWSTR)lParam2);
		return (LONG)TRUE;
	}
	return retCode;
}