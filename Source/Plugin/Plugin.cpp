
#include "StdAfx.h"
#include "Plugin.h"

// Note: GetString  have been commented out. If you need
// GetString and/or ExecuteBang and you have read what they are used for
// from the SDK docs, uncomment the function(s). Otherwise leave them
// commented out (or get rid of them)!

// MEASURE ENVIROMENT | ORIGINAL MESAURES TYPE -> OMT

BOOL OMT_DarkMode;
BOOL OMT_MicaFocus;
DWM_FROSTEDGLASS_BLUR OMT_Accent;
DWM_SYSTEMBACKDROP_TYPE OMT_Mica;
DWM_FROSTEDGLASS_BACKDROP OMT_Backdrop;
int32_t OMT_Border;
DWM_WINDOW_CORNER_PREFERENCE OMT_Corner;
DWM_FROSTEDGLASS_CBORDER OMT_CBorder;

// OS ENVIROMENT
static bool isValidWinVersion = false;
static bool isWin11 = false;
static bool isWin11Mica = false;

// PLUGIN ENVIROMENT
static bool Error_DarkMode = false;
static bool Error_Acrylic = false;
static bool Error_Mica = false;
static bool Error_Corner = false;
static bool Error_CBorder = false;
static bool Error_Backdrop = false;

// 

MARGINS margins = {};
int references = 0;

// 

bool compare(std::wstring& in, const std::wstring& search)
{
	int end = 0;
	while (in.size() > end && in[end] == L' ') ++end;
	in.erase(0, end);
	if (_wcsnicmp(in.c_str(), search.c_str(), search.size()) == 0)
	{
		in.erase(0, search.size());
		return true;
	}
	return false;
}

inline bool IsAtLeastWin10Build(DWORD buildNumber)
{
	if (!IsWindows10OrGreater())
		return false;

	const auto mask = VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL);

	OSVERSIONINFOEXW osvi{};
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwBuildNumber = buildNumber;
	return VerifyVersionInfo(&osvi, VER_BUILDNUMBER, mask) != FALSE;
}

void loadModule()
{
	if (!hModule || SetWindowCompositionAttribute == NULL) 
	{
		if ((hModule = LoadLibrary(TEXT("user32.dll"))))
		{
			SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
			GetWindowCompositionAttribute = (pGetWindowCompositionAttribute)GetProcAddress(hModule, "GetWindowCompositionAttribute");
		}
		if ((hDwmApi = LoadLibrary(L"DWMAPI.dll")))
			SetWindowAttribute = (pDwmSetWindowAttribute)GetProcAddress(hDwmApi, "DwmSetWindowAttribute");
	}
	references++;
}

void unloadModule()
{
	references--;
	if (references <= 0) 
	{
		FreeLibrary(hModule);
		FreeLibrary(hDwmApi);
		SetWindowCompositionAttribute = NULL;
		SetWindowAttribute = NULL;
	}
}

// 

void initDarkMode(void* rm)
{
	if (RmReadInt(rm, L"DarkMode", 0) == 0)
		OMT_DarkMode = FALSE;
	else
		OMT_DarkMode = TRUE;
}

void initAccent(void* rm)
{
	std::wstring accentType = RmReadString(rm, L"Type", L"BLUR");

	if (_wcsicmp(accentType.c_str(), L"NONE") == 0) 
	{
		OMT_Accent = DWMFB_DISABLED;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"GRADIENT") == 0)
	{
		OMT_Accent = DWMFB_GRADIENT;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"TRANSPARENTGRADIENT") == 0)
	{
		OMT_Accent = DWMFB_TRANSPARENTGRADIENT;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"BLUR") == 0)
	{
		OMT_Accent = DWMFB_BLURBEHIND;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"ACRYLIC") == 0)
	{
		OMT_Accent = DWMFB_ACRYLIC;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"HOSTBACKDROP") == 0)
	{
		OMT_Accent = DWMFB_HOSTBACKDROP;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"TRANSPARENTFULL") == 0)
	{
		OMT_Accent = DWMFB_TRANSPARENTFULL;
		OMT_Mica = DWMSBT_NONE;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICA") == 0)
	{
		OMT_Accent = DWMFB_DISABLED;
		OMT_Mica = DWMSBT_MAINWINDOW;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICAACRYLIC") == 0)
	{
		OMT_Accent = DWMFB_DISABLED;
		OMT_Mica = DWMSBT_TRANSIENTWINDOW;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICAALT") == 0)
	{
		OMT_Accent = DWMFB_DISABLED;
		OMT_Mica = DWMSBT_TABBEDWINDOW;
		return;
	}
}

void initMica(void* rm)
{
	if (RmReadInt(rm, L"MicaOnFocus", 0) == 0)
		OMT_MicaFocus = FALSE;
	else
		OMT_MicaFocus = TRUE;
}

void initBorder(void* rm)
{
	std::wstring borderType = RmReadString(rm, L"Border", L"NONE");

	if (compare(borderType, L"NONE"))
	{
		OMT_Border = DWMFB_NONE;
		return;
	}

	while (!borderType.empty())
	{
		if (compare(borderType, L"TOP")) OMT_Border |= DWMFB_TOP;
		if (compare(borderType, L"LEFT")) OMT_Border |= DWMFB_LEFT;
		if (compare(borderType, L"RIGHT")) OMT_Border |= DWMFB_RIGHT;
		if (compare(borderType, L"BOTTOM")) OMT_Border |= DWMFB_BOTTOM;
		if (compare(borderType, L"ALL"))
		{
			//OMT_Border = DWMFB_ALL;
			break;
		}

		if (!borderType.empty() && !compare(borderType, L"|"))
		{
			RmLogF(rm, LOG_ERROR, L"Invalid border format, expected | between tokens");
			OMT_Border = DWMFB_NONE;
			break;
		}
	}
}

void initCornerAndBorder(void* rm)
{
	if (RmReadInt(rm, L"BorderVisible", 1) == 1)
		OMT_CBorder = DWMFCB_VISIBLE;
	else
		OMT_CBorder = DWMFCB_HIDDEN;

	std::wstring cornerType = RmReadString(rm, L"Corner", L"");

	if (_wcsicmp(cornerType.c_str(), L"ROUND") == 0)
	{
		OMT_Corner = DWMWCP_ROUND;
		return;
	}
	if (_wcsicmp(cornerType.c_str(), L"ROUNDSMALL") == 0)
	{
		OMT_Corner = DWMWCP_ROUNDSMALL;
		return;
	}
	OMT_Corner = DWMWCP_DONOTROUND;
}

void initBackdrop(void* rm)
{
	std::wstring backdropTypes = RmReadString(rm, L"Backdrop", L"");

	if (_wcsicmp(backdropTypes.c_str(), L"DARK") == 0)
	{
		OMT_Backdrop = DWMFB_DARK;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT") == 0)
	{
		OMT_Backdrop = DWMFB_LIGHT;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"DARK2") == 0)
	{
		OMT_Backdrop = DWMFB_DARK2;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT2") == 0)
	{
		OMT_Backdrop = DWMFB_LIGHT2;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"DARK3") == 0)
	{
		OMT_Backdrop = DWMFB_DARK3;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT3") == 0)
	{
		OMT_Backdrop = DWMFB_LIGHT3;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"DARK4") == 0)
	{
		OMT_Backdrop = DWMFB_DARK4;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT4") == 0)
	{
		OMT_Backdrop = DWMFB_LIGHT4;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"DARK5") == 0)
	{
		OMT_Backdrop = DWMFB_DARK5;
		return;
	}
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT5") == 0)
	{
		OMT_Backdrop = DWMFB_LIGHT5;
		return;
	}
}

void initBackwardsCompability(Measure* m, void* rm)
{
	if (RmReadInt(rm, L"BlurEnabled", 1) == 0)
	{	
		m->temp_Accent = OMT_Accent;
		m->temp_Mica = OMT_Mica;
		m->temp_Corner = OMT_Corner;

		OMT_Accent = DWMFB_DISABLED;
		OMT_Mica = DWMSBT_NONE;
		OMT_Corner = DWMWCP_DONOTROUND;
	}
}

// 

void SetSkinAccent(HWND hwnd, BOOL& skinDarkMode, const DWM_FROSTEDGLASS_BLUR& skinAccent, const DWM_FROSTEDGLASS_BORDER& skinBorder, const DWM_FROSTEDGLASS_BACKDROP& skinBackdrop)
{
	if (!hModule && !SetWindowCompositionAttribute) return;

	ACCENTPOLICY policy = { skinAccent, skinBorder, skinBackdrop, NULL };
	WINCOMPATTRDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
	SetWindowCompositionAttribute(hwnd, &data);

	if (isWin11)
		SetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &skinDarkMode, sizeof(skinDarkMode));
}

void SetSkinMica(HWND hwnd, const DWM_SYSTEMBACKDROP_TYPE& skinMica, BOOL& skinMicaFocus)
{
	if (!hModule && !SetWindowCompositionAttribute) return;

	if (isWin11Mica)
	{
		if (!skinMicaFocus)
			SetWindowPos(hwnd, nullptr, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);

		margins = { 0 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);
		margins = { -1 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		SetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &skinMica, sizeof(skinMica));
	}		
}

void SetSkinCornerAndBorder(HWND hwnd, const DWM_WINDOW_CORNER_PREFERENCE& skinCorner, const DWM_FROSTEDGLASS_CBORDER& skinCBorder)
{
	if (!hModule && !SetWindowCompositionAttribute) return;
	
	if (isWin11)
	{
		SetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &skinCorner, sizeof(skinCorner));
		SetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &skinCBorder, sizeof(skinCBorder));
	}
}

//

void checkFeatures()
{
	if (OMT_DarkMode == TRUE && !isWin11)
	{
		Error_DarkMode = true;
		OMT_DarkMode = FALSE;
	}

	if (OMT_Accent == DWMFB_ACRYLIC && !isWin11)
	{
		Error_Acrylic = true;
		OMT_Accent = DWMFB_BLURBEHIND;
	}

	if (OMT_Mica != DWMSBT_NONE && !isWin11Mica)
	{
		Error_Mica = true;
		OMT_MicaFocus = FALSE;
		OMT_Accent = DWMFB_BLURBEHIND;
	}

	if (OMT_Corner != DWMWCP_DONOTROUND && !isWin11)
	{
		Error_Corner = true;
		OMT_Corner = DWMWCP_DONOTROUND;
	}
	
	if (OMT_CBorder != DWMFCB_VISIBLE && !isWin11)
	{
		Error_CBorder = true;
		OMT_CBorder = DWMFCB_VISIBLE;
	}

	if (OMT_Backdrop != DWMFB_NOCOLOR && !isWin11)
	{
		Error_Backdrop = true;
		OMT_Backdrop = DWMFB_NOCOLOR;
	}
}

void checkErrors()
{
	if (SetWindowCompositionAttribute == NULL)
		RmLog(LOG_ERROR, L"Could not load the SetWindowCompositionAttribute function from user32.dll, did microsoft remove it?");

	if (GetWindowCompositionAttribute == NULL)
		RmLog(LOG_ERROR, L"Could not load the GetWindowCompositionAttribute function from user32.dll, did microsoft remove it?");

	if (SetWindowAttribute == NULL)
		RmLog(LOG_ERROR, L"Could not load the DwmSetWindowAttribute function from DWMAPI.dll");

	if (Error_Mica)
		RmLog(LOG_WARNING, L"Mica is not supported until Windows 11 build 22621.");
	
	if (Error_DarkMode)
		RmLog(LOG_WARNING, L"Dark Mode is not supported until Windows 11 build 22000.");
	
	if (Error_Acrylic)
		RmLog(LOG_WARNING, L"Acrylic is not supported until Windows 11 build 22000.");
	
	if (Error_Corner)
		RmLog(LOG_WARNING, L"Round Corner is not supported until Windows 11 build 22000.");
	
	if (Error_CBorder)
		RmLog(LOG_WARNING, L"'BorderVisible' option only works for Round Corners.");
	
	if (Error_Backdrop)
		RmLog(LOG_WARNING, L"Backdrop is not supported until Windows 11 build 22000.");
}

// RAINMETER FUNCTIONS

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	if (!IsWindows10OrGreater()) return;

	isValidWinVersion = true;

	isWin11Mica = IsAtLeastWin10Build(BUILD_22H2);
	isWin11 = isWin11Mica ? true : IsAtLeastWin10Build(BUILD_WIN11);

	OMT_DarkMode = FALSE;
	OMT_MicaFocus = FALSE;
	OMT_Accent = DWMFB_DISABLED;
	OMT_Mica = DWMSBT_NONE;
	OMT_Backdrop = DWMFB_NOCOLOR;
	OMT_Border = DWMFB_NONE;
	OMT_Corner = DWMWCP_DONOTROUND;
	OMT_CBorder = DWMFCB_VISIBLE;

	Measure* m = new Measure;
	m->skin = RmGetSkinWindow(rm);
	initDarkMode(rm);
	initMica(rm);
	loadModule();
	*data = m;

}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	if (!isValidWinVersion) return;

	Measure* m = (Measure*)data;
	m->skin = RmGetSkinWindow(rm);

	initAccent(rm);
	initBorder(rm);
	initBackdrop(rm);
	initCornerAndBorder(rm);
	checkFeatures();
	initBackwardsCompability(m, rm);

	if (OMT_MicaFocus) 
		SetSkinMica(m->skin, OMT_Mica, OMT_MicaFocus);

	if (m->DarkMode == OMT_DarkMode &&
		m->MicaFocus == OMT_MicaFocus &&
		m->Accent == OMT_Accent &&
		m->Mica == OMT_Mica &&
		m->Border == OMT_Border &&
		m->Corner == OMT_Corner &&
		m->CBorder == OMT_CBorder &&
		m->Backdrop == OMT_Backdrop) return;

	SetSkinAccent(m->skin, OMT_DarkMode, OMT_Accent, (DWM_FROSTEDGLASS_BORDER)OMT_Border, OMT_Backdrop);
	if (!OMT_MicaFocus) 
		SetSkinMica(m->skin, OMT_Mica, OMT_MicaFocus);
	SetSkinCornerAndBorder(m->skin, OMT_Corner, OMT_CBorder);

	checkErrors();

	m->DarkMode = OMT_DarkMode;
	m->MicaFocus = OMT_MicaFocus;
	m->Accent = OMT_Accent;
	m->Mica = OMT_Mica;
	m->Border = (DWM_FROSTEDGLASS_BORDER)OMT_Border;
	m->Corner = OMT_Corner;
	m->CBorder = OMT_CBorder;
	m->Backdrop = OMT_Backdrop;
}

PLUGIN_EXPORT double Update(void* data)
{
	return 0.0;
}

//PLUGIN_EXPORT LPCWSTR GetString(void* data)
//{
//	Measure* measure = (Measure*)data;
//	return L"";
//}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
	if (!isValidWinVersion)
	{
		RmLog(LOG_WARNING, L"FrostedGlass commands is supported only on Windows 10 and 11.");
		return;
	}

	if (!hModule && !SetWindowCompositionAttribute) return;

	Measure* m = (Measure*)data;
	std::wstring sargs = args;

	if (isWin11)
	{
		// Mica
		if (isWin11Mica)
		{
			if (compare(sargs, L"SETFOCUS"))
			{
				if (m->Mica != DWMSBT_NONE)
				{
					m->MicaFocus = TRUE;
					SetSkinMica(m->skin, m->Mica, m->MicaFocus);
					m->MicaFocus = OMT_MicaFocus;
				}
				return;
			}
			if(compare(sargs, L"TOGGLEFOCUS"))
			{
				if (m->MicaFocus)
				{
					m->MicaFocus = FALSE;
					OMT_MicaFocus = FALSE;
				}
				else
				{
					m->MicaFocus = TRUE;
					OMT_MicaFocus = TRUE;
				}

				SetSkinMica(m->skin, m->Mica, m->MicaFocus);
				return;
			}
			if (compare(sargs, L"ENABLEFOCUS"))
			{
				OMT_MicaFocus = TRUE;
				m->MicaFocus = TRUE;

				SetSkinMica(m->skin, m->Mica, m->MicaFocus);
				return;
			}
			if (compare(sargs, L"DISABLEFOCUS"))
			{
				OMT_MicaFocus = FALSE;
				m->MicaFocus = FALSE;

				SetSkinMica(m->skin, m->Mica, m->MicaFocus);
				return;
			}
		}
		// DarkMode
		if (compare(sargs, L"TOGGLEMODE"))
		{
			if (m->DarkMode)
			{
				m->DarkMode = FALSE;
				OMT_DarkMode = FALSE;
			} 
			else
			{
				m->DarkMode = TRUE;
				OMT_DarkMode = TRUE;
			}
			
			SetWindowAttribute(m->skin, DWMWA_USE_IMMERSIVE_DARK_MODE, &m->DarkMode, sizeof(m->DarkMode));
			return;
		}
		if (compare(sargs, L"LIGHTMODE"))
		{
			OMT_DarkMode = FALSE;
			m->DarkMode = FALSE;

			SetWindowAttribute(m->skin, DWMWA_USE_IMMERSIVE_DARK_MODE, &m->DarkMode, sizeof(m->DarkMode));
			return;
		}
		if (compare(sargs, L"DARKMODE"))
		{
			OMT_DarkMode = TRUE;
			m->DarkMode = TRUE;

			SetWindowAttribute(m->skin, DWMWA_USE_IMMERSIVE_DARK_MODE, &m->DarkMode, sizeof(m->DarkMode));
			return;
		}
		// Corner
		if (compare(sargs, L"TOGGLECORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;
	
			if (m->Corner == DWMWCP_DONOTROUND)
			{
				m->Corner = m->temp_Corner;
				m->temp_Corner = DWMWCP_DONOTROUND;
			}
			else
			{
				m->temp_Corner = m->Corner;
				m->Corner = DWMWCP_DONOTROUND;
			}

			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->Corner, sizeof(m->Corner));
			return;
		}
		if (compare(sargs, L"ENABLECORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			if (m->Corner != DWMWCP_DONOTROUND) return;

			m->Corner = m->temp_Corner;

			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->Corner, sizeof(m->Corner));
			return;
		}
		if (compare(sargs, L"DISABLECORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			if (m->Corner == DWMWCP_DONOTROUND) return;

			m->temp_Corner = m->Corner;
			m->Corner = DWMWCP_DONOTROUND;

			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->Corner, sizeof(m->Corner));
			return;
		}
		if (compare(sargs, L"SETCORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			m->Corner = DWMWCP_DONOTROUND;
			if (compare(sargs, L"ROUNDSMALL")) m->Corner = DWMWCP_ROUNDSMALL;
			if (compare(sargs, L"ROUND")) m->Corner = DWMWCP_ROUND;
			m->temp_Corner = m->Corner;

			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->Corner, sizeof m->Corner);
			return;
		}
		// BorderVisible
		if (compare(sargs, L"TOGGLEBORDERS"))
		{
			if (m->CBorder == DWMFCB_VISIBLE)
				m->CBorder = DWMFCB_HIDDEN;
			else
				m->CBorder = DWMFCB_VISIBLE;
			
			SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &m->CBorder, sizeof(m->CBorder));
			return;
		}
		if (compare(sargs, L"ENABLEBORDERS"))
		{
			m->CBorder = DWMFCB_VISIBLE;
			SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &m->CBorder, sizeof(m->CBorder));
			return;
		}
		if (compare(sargs, L"DISABLEBORDERS"))
		{
			m->CBorder = DWMFCB_HIDDEN;
			SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &m->CBorder, sizeof(m->CBorder));
			return;
		}
		// Backdrop
		if (compare(sargs, L"TOGGLEBACKDROP"))
		{
			if (m->Backdrop == DWMFB_DARK || m->Backdrop == DWMFB_LIGHT) m->Backdrop = m->Backdrop == DWMFB_DARK ? DWMFB_LIGHT : DWMFB_DARK;
			if (m->Backdrop == DWMFB_DARK2 || m->Backdrop == DWMFB_LIGHT2) m->Backdrop = m->Backdrop == DWMFB_DARK2 ? DWMFB_LIGHT2 : DWMFB_DARK2;
			if (m->Backdrop == DWMFB_DARK3 || m->Backdrop == DWMFB_LIGHT3) m->Backdrop = m->Backdrop == DWMFB_DARK3 ? DWMFB_LIGHT3 : DWMFB_DARK3;
			if (m->Backdrop == DWMFB_DARK4 || m->Backdrop == DWMFB_LIGHT4) m->Backdrop = m->Backdrop == DWMFB_DARK4 ? DWMFB_LIGHT4 : DWMFB_DARK4;
			if (m->Backdrop == DWMFB_DARK5 || m->Backdrop == DWMFB_LIGHT5) m->Backdrop = m->Backdrop == DWMFB_DARK5 ? DWMFB_LIGHT5 : DWMFB_DARK2;

			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"LIGHTBACKDROP"))
		{
			if (m->Backdrop == DWMFB_DARK) m->Backdrop = DWMFB_LIGHT;
			if (m->Backdrop == DWMFB_DARK2) m->Backdrop = DWMFB_LIGHT2;
			if (m->Backdrop == DWMFB_DARK3) m->Backdrop = DWMFB_LIGHT3;
			if (m->Backdrop == DWMFB_DARK4) m->Backdrop = DWMFB_LIGHT4;
			if (m->Backdrop == DWMFB_DARK5) m->Backdrop = DWMFB_LIGHT5;

			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"DARKBACKDROP"))
		{
			if (m->Backdrop == DWMFB_LIGHT) m->Backdrop = DWMFB_DARK;
			if (m->Backdrop == DWMFB_LIGHT2) m->Backdrop = DWMFB_DARK2;
			if (m->Backdrop == DWMFB_LIGHT3) m->Backdrop = DWMFB_DARK3;
			if (m->Backdrop == DWMFB_LIGHT4) m->Backdrop = DWMFB_DARK4;
			if (m->Backdrop == DWMFB_LIGHT5) m->Backdrop = DWMFB_DARK5;

			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"SETBACKDROP"))
		{
			if (compare(sargs, L"LIGHT"))
			{
				if (compare(sargs, L"5")) m->Backdrop = DWMFB_LIGHT5;
				if (compare(sargs, L"4")) m->Backdrop = DWMFB_LIGHT4;
				if (compare(sargs, L"3")) m->Backdrop = DWMFB_LIGHT3;
				if (compare(sargs, L"2")) m->Backdrop = DWMFB_LIGHT2;
				if (compare(sargs, L"1")) m->Backdrop = DWMFB_LIGHT;
			}
			else if (compare(sargs, L"DARK"))
			{
				if (compare(sargs, L"5")) m->Backdrop = DWMFB_DARK5;
				if (compare(sargs, L"4")) m->Backdrop = DWMFB_DARK4;
				if (compare(sargs, L"3")) m->Backdrop = DWMFB_DARK3;
				if (compare(sargs, L"2")) m->Backdrop = DWMFB_DARK2;
				if (compare(sargs, L"1")) m->Backdrop = DWMFB_DARK;
			}

			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			return;
		}
	}

	if (compare(sargs, L"TOGGLEBLUR"))
	{
		if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE)
		{
			m->Accent = m->temp_Accent;
			m->Mica = m->temp_Mica;
			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			SetSkinMica(m->skin, m->Mica, m->MicaFocus);
			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
		}
		else
		{
			m->temp_Accent = m->Accent;
			m->temp_Mica = m->Mica;
			m->Accent = DWMFB_DISABLED;
			m->Mica = DWMSBT_NONE;
			SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
			SetSkinMica(m->skin, m->Mica, m->MicaFocus);
			SetSkinCornerAndBorder(m->skin, DWMWCP_DONOTROUND, m->CBorder);
		}
		return;
	}
	if (compare(sargs, L"ENABLEBLUR"))
	{
		if (m->Accent != DWMFB_DISABLED || m->Mica != DWMSBT_NONE) return;

		m->Accent = m->temp_Accent;
		m->Mica = m->temp_Mica;

		SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Mica, m->MicaFocus);
		SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
		return;
	}
	if (compare(sargs, L"DISABLEBLUR"))
	{
		if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

		m->temp_Accent = m->Accent;
		m->temp_Mica = m->Mica;
		m->Accent = DWMFB_DISABLED;
		m->Mica = DWMSBT_NONE;

		SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Mica, m->MicaFocus);
		SetSkinCornerAndBorder(m->skin, DWMWCP_DONOTROUND, m->CBorder);
		return;
	}
	if (compare(sargs, L"SETBLUR"))
	{		
		if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

		m->Accent = DWMFB_BLURBEHIND;
		m->Mica = DWMSBT_NONE;

		if (compare(sargs, L"ACRYLIC"))
		{
			m->Accent = DWMFB_ACRYLIC;
			m->Mica = DWMSBT_NONE;
		}
		if (compare(sargs, L"MICA"))
		{
			m->Accent = DWMFB_DISABLED;
			m->Mica = DWMSBT_MAINWINDOW;
			if (compare(sargs, L"ACRYLIC"))
			{
				m->Accent = DWMFB_DISABLED;
				m->Mica = DWMSBT_TRANSIENTWINDOW;
			}
			if (compare(sargs, L"ALT"))
			{
				m->Accent = DWMFB_DISABLED;
				m->Mica = DWMSBT_TABBEDWINDOW;
			}
		}

		m->temp_Accent = m->Accent;
		m->temp_Mica = m->Mica;

		SetSkinAccent(m->skin, m->DarkMode, m->Accent, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Mica, m->MicaFocus);
		return;
	}

}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* m = (Measure*)data;

	if (!isValidWinVersion)
	{
		RmLog(LOG_WARNING, L"FrostedGlass plugin is supported only on Windows 10 and 11.");
		return;
	}

	unloadModule();
	delete m;
}