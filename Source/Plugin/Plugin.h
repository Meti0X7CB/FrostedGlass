
#pragma once

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

constexpr DWORD BUILD_1803 = 17134; // Windows 10 1803 (April 2018 Update)

constexpr DWORD BUILD_WIN11 = 22000; // Windows 11 first "stable" build
constexpr DWORD BUILD_22H2 = 22621; // Windows 11 22H2 first to support mica properly

constexpr uint32_t WCA_ACCENT_POLICY = 19;

// custom enumerations

enum DWM_FROSTEDGLASS_BLUR : int32_t 
{
	DWMFB_DISABLED = 0,
	DWMFB_GRADIENT = 1, // UNUSED
	DWMFB_TRANSPARENTGRADIENT = 2, // UNUSED
	DWMFB_BLURBEHIND = 3,
	DWMFB_ACRYLIC = 4,
	DWMFB_HOSTBACKDROP = 5, // UNUSED
	DWMFB_TRANSPARENTFULL = 6 // UNUSED
};

enum DWM_FROSTEDGLASS_BORDER : int32_t
{
	DWMFB_NONE = 0x0,
	DWMFB_LEFT = 0x20,
	DWMFB_TOP = 0x40,
	DWMFB_RIGHT = 0x80,
	DWMFB_BOTTOM = 0x100,
	DWMFB_ALL = (DWMFB_LEFT | DWMFB_TOP | DWMFB_RIGHT | DWMFB_BOTTOM)
};

enum DWM_FROSTEDGLASS_BACKDROP : uint32_t
{
	DWMFB_NOCOLOR = 0x00000000, 
	DWMFB_DARK = 0x01000000,
	DWMFB_DARK2 = 0x3F000000,
	DWMFB_DARK3 = 0x7E000000,
	DWMFB_DARK4 = 0xBD000000,
	DWMFB_DARK5 = 0xFF000000,
	DWMFB_LIGHT = 0x01FFFFFF,
	DWMFB_LIGHT2 = 0x3FFFFFFF,
	DWMFB_LIGHT3 = 0x7EFFFFFF,
	DWMFB_LIGHT4 = 0xBDFFFFFF,
	DWMFB_LIGHT5 = 0xFFFFFFFF
};

enum DWM_FROSTEDGLASS_CBORDER : COLORREF
{
	DWMFCB_VISIBLE = 0xFFFFFFFF,
	DWMFCB_HIDDEN = 0xFFFFFFFE
};

//

struct ACCENTPOLICY
{
	DWM_FROSTEDGLASS_BLUR nAccentState = DWM_FROSTEDGLASS_BLUR::DWMFB_DISABLED;
	DWM_FROSTEDGLASS_BORDER nFlags = DWM_FROSTEDGLASS_BORDER::DWMFB_NONE;
	DWM_FROSTEDGLASS_BACKDROP nColor = DWM_FROSTEDGLASS_BACKDROP::DWMFB_DARK;
	int32_t nAnimationId = NULL;
};

struct WINCOMPATTRDATA 
{
	uint32_t nAttribute = NULL;
	void* pData = nullptr;
	uint32_t ulDataSize = NULL;
};

HINSTANCE hModule = NULL;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
pSetWindowCompositionAttribute SetWindowCompositionAttribute;
typedef BOOL(WINAPI* pGetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
pGetWindowCompositionAttribute GetWindowCompositionAttribute;

HMODULE hDwmApi = NULL;
typedef HRESULT(WINAPI* pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
pDwmSetWindowAttribute SetWindowAttribute;

//

struct Measure
{
	HWND skin = NULL;
	// Init Values
	BOOL DarkMode = FALSE;
	BOOL MicaFocus = FALSE;
	DWM_FROSTEDGLASS_BLUR Accent = DWMFB_DISABLED;
	DWM_SYSTEMBACKDROP_TYPE Mica = DWMSBT_NONE;
	DWM_FROSTEDGLASS_BACKDROP Backdrop = DWMFB_NOCOLOR;
	DWM_FROSTEDGLASS_BORDER Border = DWMFB_NONE;
	DWM_WINDOW_CORNER_PREFERENCE Corner = DWMWCP_DONOTROUND;
	DWM_FROSTEDGLASS_CBORDER CBorder = DWMFCB_VISIBLE;
	// Temp Values for ExecuteBang
	DWM_FROSTEDGLASS_BLUR temp_Accent = DWMFB_DISABLED;
	DWM_SYSTEMBACKDROP_TYPE temp_Mica = DWMSBT_NONE;
	//DWM_FROSTEDGLASS_BACKDROP temp_Backdrop = DWMFB_NOCOLOR; // UNUSED
	//DWM_FROSTEDGLASS_BORDER temp_Border = DWMFB_NONE; // UNUSED
	DWM_WINDOW_CORNER_PREFERENCE temp_Corner = DWMWCP_DONOTROUND;
	//DWM_FROSTEDGLASS_CBORDER temp_CBorder = DWMFCB_VISIBLE; // UNUSED
};

bool compare(std::wstring& in, const std::wstring& search);
bool IsAtLeastWin10Build(DWORD buildNumber);
void loadModule();
void unloadModule();

void initPluginDefaultValues();

void initRainmeterOptions(void* rm);
void initDarkMode(void* rm);
void initAccent(void* rm);
void initMica(void* rm);
void initBorder(void* rm);
void initCornerAndBorder(void* rm);
void initBackdrop(void* rm);
void initBackwardsCompability(struct Measure* m);

void SetSkinAccent(HWND hwnd, const DWM_FROSTEDGLASS_BLUR& skinAccent, const DWM_FROSTEDGLASS_BORDER& skinBorder, const DWM_FROSTEDGLASS_BACKDROP& skinBackdrop);
void SetSkinCornerAndBorder(HWND hwnd, const DWM_WINDOW_CORNER_PREFERENCE& skinCorner, const DWM_FROSTEDGLASS_CBORDER& skinCBorder);
void SetSkinMica(HWND hwnd, const DWM_SYSTEMBACKDROP_TYPE& skinMica, BOOL& skinMicaFocus);
void SetSkinDarkMode(HWND hwnd, BOOL& skinDarkMode);

void checkFeatures();
void checkErrors();