
#include "StdAfx.h"
#include "Plugin.h"

/*
// CHANGELOG FOR 1.1.9
// Added Corner Type <RoundWs> [Experimental]
// - Added bangs.
// Added Effect Type <Luminance> [Experimental]
// Added Backdrop Type <LightBase|DarkBase>
// - Added bangs
// Fixed issue related to Luminance effect when Blur was changed to Normal blur with measures bangs.
// Fixed issue when sometimes Droptop loses their focus closing any menu window.
// Improved Plugin performance.
*/

// Note: GetString  have been commented out. If you need
// GetString and/or ExecuteBang and you have read what they are used for
// from the SDK docs, uncomment the function(s). Otherwise leave them
// commented out (or get rid of them)!

// MEASURE ENVIROMENT | ORIGINAL MESAURES TYPE -> OMT

static bool isValidWinVersion;
static bool isWin10;
static bool isWin11Mica;
static bool isWin11;

static bool Error_HModule;
static bool Error_HDwmapi;

int references = 0;

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
	if (!IsWindows10OrGreater()) return false;

	const auto mask = VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL);

	OSVERSIONINFOEXW osvi{};
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwBuildNumber = buildNumber;
	return VerifyVersionInfo(&osvi, VER_BUILDNUMBER, mask) != FALSE;
}

void loadModule()
{
	if (Error_HModule || SetWindowCompositionAttribute == NULL)
	{

		if ((hModule = LoadLibrary(TEXT("user32.dll"))))
		{
			Error_HModule = false;
			SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
			GetWindowCompositionAttribute = (pGetWindowCompositionAttribute)GetProcAddress(hModule, "GetWindowCompositionAttribute");
		}
		if ((hDwmApi = LoadLibrary(L"DWMAPI.dll")))
		{
			Error_HDwmapi = false;
			SetWindowAttribute = (pDwmSetWindowAttribute)GetProcAddress(hDwmApi, "DwmSetWindowAttribute");
		}
		if (SetWindowCompositionAttribute == NULL) RmLog(LOG_ERROR, L"Could not load the SetWindowCompositionAttribute function from user32.dll, did Microsoft remove it?");

		if (GetWindowCompositionAttribute == NULL) RmLog(LOG_ERROR, L"Could not load the GetWindowCompositionAttribute function from user32.dll, did Microsoft remove it?");

		if (SetWindowAttribute == NULL) RmLog(LOG_ERROR, L"Could not load the DwmSetWindowAttribute function from DWMAPI.dll");
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
void initPluginDefaultValues()
{
	isValidWinVersion = true;

	isWin10 = IsAtLeastWin10Build(BUILD_1803);

	isWin11Mica = IsAtLeastWin10Build(BUILD_22H2);
	isWin11 = isWin11Mica ? true : IsAtLeastWin10Build(BUILD_WIN11);

	Error_HModule = true;
	Error_HDwmapi = true;
}

void initRainmeterOptions(void* rm, Measure* m)
{
	m->BlurEnabled = (RmReadInt(rm, L"BlurEnabled", 1) == 1) ? TRUE : FALSE;
	m->Disabled = (RmReadInt(rm, L"Disabled", 0) == 1) ? TRUE : FALSE;
	m->DynamicVariables = (RmReadInt(rm, L"DynamicVariables", 0) == 1) ? TRUE : FALSE;
}
void initDarkMode(void* rm, Measure* m)
{
	m->OMT_DarkMode = (RmReadInt(rm, L"DarkMode", 0) == 1) ? TRUE : FALSE;
}
void initMica(void* rm, Measure* m)
{
	m->OMT_MicaFocus = (RmReadInt(rm, L"MicaOnFocus", 0) == 1) ? TRUE : FALSE;
}
void initAccent(void* rm, Measure* m)
{
	std::wstring accentType = RmReadString(rm, L"Type", L"");

	m->OMT_Accent = DWMFB_DISABLED;
	m->OMT_Mica = DWMSBT_NONE;

	if (accentType.empty()) return;

	if (_wcsicmp(accentType.c_str(), L"ACCENT") == 0)
	{
		m->OMT_Accent = DWMFB_COLORACCENT;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"BLUR") == 0)
	{
		m->OMT_Accent = DWMFB_BLURBEHIND;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"ACRYLIC") == 0)
	{
		m->OMT_Accent = DWMFB_ACRYLIC;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICA") == 0)
	{
		m->OMT_Mica = DWMSBT_MAINWINDOW;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICAACRYLIC") == 0)
	{
		m->OMT_Mica = DWMSBT_TRANSIENTWINDOW;
		return;
	}
	if (_wcsicmp(accentType.c_str(), L"MICAALT") == 0)
	{
		m->OMT_Mica = DWMSBT_TABBEDWINDOW;
		return;
	}
}
void initBorder(void* rm, Measure* m)
{
	std::wstring borderType = RmReadString(rm, L"Border", L"");

	m->OMT_Border = DWMFB_NONE;

	while (!borderType.empty())
	{
		if (compare(borderType, L"TOP")) m->OMT_Border |= DWMFB_TOP;
		if (compare(borderType, L"LEFT")) m->OMT_Border |= DWMFB_LEFT;
		if (compare(borderType, L"RIGHT")) m->OMT_Border |= DWMFB_RIGHT;
		if (compare(borderType, L"BOTTOM")) m->OMT_Border |= DWMFB_BOTTOM;
		if (compare(borderType, L"NONE")) break;
		if (compare(borderType, L"ALL"))
		{
			m->OMT_Border = DWMFB_ALL;
			break;
		}

		if (!borderType.empty() && !compare(borderType, L"|"))
		{
			RmLogF(rm, LOG_ERROR, L"Invalid border format, expected | between tokens");
			m->OMT_Border = DWMFB_NONE;
			break;
		}
	}
}
void initCornerAndBorder(void* rm, Measure* m)
{
	std::wstring cornerType = RmReadString(rm, L"Corner", L"");

	m->OMT_CBorder = DWMFCB_VISIBLE;
	m->OMT_Corner = DWMWCP_DONOTROUND;

	if (RmReadInt(rm, L"BorderVisible", 1) == 0) m->OMT_CBorder = DWMFCB_HIDDEN;

	if (cornerType.empty()) return;

	if (_wcsicmp(cornerType.c_str(), L"ROUND") == 0)
	{
		m->OMT_Corner = DWMWCP_ROUND;
		return;
	}
	if (_wcsicmp(cornerType.c_str(), L"ROUNDWS") == 0)
	{
		m->OMT_Corner = (DWM_WINDOW_CORNER_PREFERENCE)4;
		return;
	}
	if (_wcsicmp(cornerType.c_str(), L"ROUNDSMALL") == 0)
	{
		m->OMT_Corner = DWMWCP_ROUNDSMALL;
		return;
	}
}
void initBackdrop(void* rm, Measure* m)
{
	std::wstring backdropType = RmReadString(rm, L"Backdrop", L"");

	m->OMT_Backdrop = DWMFB_NOCOLOR;

	if (backdropType.empty()) return;

	if (_wcsicmp(backdropType.c_str(), L"DARK") == 0)
	{
		m->OMT_Backdrop = DWMFB_DARK;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHT") == 0)
	{
		m->OMT_Backdrop = DWMFB_LIGHT;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"DARK2") == 0)
	{
		m->OMT_Backdrop = DWMFB_DARK2;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHT2") == 0)
	{
		m->OMT_Backdrop = DWMFB_LIGHT2;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"DARK3") == 0)
	{
		m->OMT_Backdrop = DWMFB_DARK3;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHT3") == 0)
	{
		m->OMT_Backdrop = DWMFB_LIGHT3;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"DARK4") == 0)
	{
		m->OMT_Backdrop = DWMFB_DARK4;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHT4") == 0)
	{
		m->OMT_Backdrop = DWMFB_LIGHT4;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"DARK5") == 0)
	{
		m->OMT_Backdrop = DWMFB_DARK5;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHT5") == 0)
	{
		m->OMT_Backdrop = DWMFB_LIGHT5;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"DARKBASE") == 0)
	{
		m->OMT_Backdrop = DWMFB_WINDARK_BASE;
		return;
	}
	if (_wcsicmp(backdropType.c_str(), L"LIGHTBASE") == 0)
	{
		m->OMT_Backdrop = DWMFB_WINLIGHT_BASE;
		return;
	}
}
void initEffect(void* rm, Measure* m)
{
	m->OMT_Effect = DWMFF_LUMINANCE_LOW;

	if (m->OMT_Accent != DWMFB_ACRYLIC) return;

	std::wstring effectType = RmReadString(rm, L"Effect", L"");


	if (compare(effectType, L"LUMINANCE")) m->OMT_Effect |= DWMFF_LUMINANCE_HIGH;
}
void initBackwardsCompability(Measure* m)
{
	if (!m->BlurEnabled || m->Disabled)
	{
		m->temp_Accent = m->OMT_Accent;
		m->temp_Mica = m->OMT_Mica;
		m->temp_Corner = m->OMT_Corner;

		m->OMT_Accent = DWMFB_DISABLED;
		m->OMT_Mica = DWMSBT_NONE;
		m->OMT_Corner = DWMWCP_DONOTROUND;
	}
}

void SetSkinAccent(HWND hwnd, const DWM_FROSTEDGLASS_BLUR& skinAccent, const DWM_FROSTEDGLASS_UNDOCUMENTED& skinEffect, const DWM_FROSTEDGLASS_BORDER& skinBorder, const DWM_FROSTEDGLASS_BACKDROP& skinBackdrop)
{
	if (Error_HModule) return;

	ACCENTPOLICY policy = { skinAccent, (DWM_FROSTEDGLASS_BORDER)(skinBorder | skinEffect) , skinBackdrop, NULL };
	WINCOMPATTRDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
	SetWindowCompositionAttribute(hwnd, &data);
}
void SetSkinMica(HWND hwnd, const DWM_FROSTEDGLASS_BLUR& skinAccent, const DWM_SYSTEMBACKDROP_TYPE& skinMica, BOOL& skinMicaFocus)
{
	if (Error_HModule) return;
	if (!isWin11Mica) return;

	if (skinMica == DWMSBT_NONE)
		SetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &skinMica, sizeof(skinMica));
	else
	{
		if (!skinMicaFocus) SetWindowPos(hwnd, hwnd, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		SetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &skinMica, sizeof(skinMica));

		margins = { 0 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);
	}
}
void SetSkinCornerAndBorder(HWND hwnd, const DWM_WINDOW_CORNER_PREFERENCE& skinCorner, const DWM_FROSTEDGLASS_CBORDER& skinCBorder)
{
	if (Error_HModule) return;
	if (!isWin11) return;

	SetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &skinCorner, sizeof(skinCorner));
	SetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &skinCBorder, sizeof(skinCBorder));
}
void SetSkinDarkMode(HWND hwnd, BOOL& skinDarkMode)
{
	if (!isWin11) return;

	SetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &skinDarkMode, sizeof(skinDarkMode));
}

void checkFeatures(Measure* m)
{
	if ((m->OMT_DarkMode == TRUE) && !isWin11)
	{
		m->Warn_DarkMode = true;
		m->OMT_DarkMode = FALSE;
	}

	if ((m->OMT_Accent == DWMFB_ACRYLIC) && !isWin10)
	{
		m->Warn_Acrylic = true;
		m->OMT_Accent = DWMFB_BLURBEHIND;
	}

	if ((m->OMT_Mica != DWMSBT_NONE) && !isWin11Mica)
	{
		m->Warn_Mica = true;
		m->OMT_MicaFocus = FALSE;
		m->OMT_Accent = DWMFB_BLURBEHIND;
		m->OMT_Mica = DWMSBT_NONE;
	}

	if ((m->OMT_Corner != DWMWCP_DONOTROUND) && !isWin11)
	{
		m->Warn_Corner = true;
		m->OMT_Corner = DWMWCP_DONOTROUND;
	}

	if ((m->OMT_CBorder != DWMFCB_VISIBLE) && !isWin11)
	{
		m->Warn_CBorder = true;
		m->OMT_CBorder = DWMFCB_VISIBLE;
	}

	if ((m->OMT_Backdrop != DWMFB_NOCOLOR) && !isWin11)
	{
		m->Warn_Backdrop = true;
		m->OMT_Backdrop = DWMFB_NOCOLOR;
	}

	if ((m->OMT_Effect != DWMFF_LUMINANCE_LOW) && !isWin11)
	{
		m->Warn_Effect = true;
		m->OMT_Effect = DWMFF_LUMINANCE_LOW;
	}
	
	if ((m->OMT_MicaFocus == TRUE) && (m->Accent != DWMFB_DISABLED) && (m->Mica != DWMSBT_NONE))
		m->OMT_MicaFocus = FALSE;
}
void checkErrors(Measure* m)
{
	if (m->Warn_Mica) RmLog(LOG_WARNING, L"Mica is not supported until Windows 11 build 22621.");

	if (m->Warn_DarkMode) RmLog(LOG_WARNING, L"Dark Mode is not supported until Windows 11 build 22000.");

	if (m->Warn_Acrylic) RmLog(LOG_WARNING, L"Acrylic is not supported until Windows 10 build 17134 and Windows 11 build 22000.");

	if (m->Warn_Corner) RmLog(LOG_WARNING, L"Round Corner is not supported until Windows 11 build 22000.");

	if (m->Warn_CBorder) RmLog(LOG_WARNING, L"'BorderVisible' option only works for Round Corners.");

	if (m->Warn_Backdrop) RmLog(LOG_WARNING, L"Backdrop is not supported until Windows 11 build 22000.");

	if (m->Warn_Effect) RmLog(LOG_WARNING, L"Effect is not supported until Windows 11 build 22000.");
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	if (!IsWindows10OrGreater()) return;

	initPluginDefaultValues();

	Measure* m = new Measure;
	m->skin = RmGetSkinWindow(rm);
	*data = m;

	loadModule();
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	if (!isValidWinVersion) return;

	Measure* m = (Measure*)data;

	initRainmeterOptions(rm, m);
	initDarkMode(rm, m);
	initMica(rm, m);
	initAccent(rm, m);
	initBorder(rm, m);
	initCornerAndBorder(rm, m);
	initBackdrop(rm, m);
	initEffect(rm, m);
	checkFeatures(m);
	initBackwardsCompability(m);

	if (m->OMT_MicaFocus) SetSkinMica(m->skin, m->OMT_Accent, m->OMT_Mica, m->OMT_MicaFocus);

	if (m->DarkMode == m->OMT_DarkMode &&
		m->MicaFocus == m->OMT_MicaFocus &&
		m->Accent == m->OMT_Accent &&
		m->Mica == m->OMT_Mica &&
		m->Border == m->OMT_Border &&
		m->Effect == m->OMT_Effect &&
		m->Corner == m->OMT_Corner &&
		m->CBorder == m->OMT_CBorder &&
		m->Backdrop == m->OMT_Backdrop) return;

	SetSkinAccent(m->skin, m->OMT_Accent, (DWM_FROSTEDGLASS_UNDOCUMENTED)m->OMT_Effect, (DWM_FROSTEDGLASS_BORDER)m->OMT_Border, m->OMT_Backdrop);
	if (!m->OMT_MicaFocus) SetSkinMica(m->skin, m->OMT_Accent, m->OMT_Mica, m->OMT_MicaFocus);
	SetSkinCornerAndBorder(m->skin, m->OMT_Corner, m->OMT_CBorder);
	SetSkinDarkMode(m->skin, m->OMT_DarkMode);

	checkErrors(m);

	m->DarkMode = m->OMT_DarkMode;
	m->MicaFocus = m->OMT_MicaFocus;
	m->Accent = m->OMT_Accent;
	m->Mica = m->OMT_Mica;
	m->Border = (DWM_FROSTEDGLASS_BORDER)m->OMT_Border;
	m->Effect = (DWM_FROSTEDGLASS_UNDOCUMENTED)m->OMT_Effect;
	m->Corner = m->OMT_Corner;
	m->CBorder = m->OMT_CBorder;
	m->Backdrop = m->OMT_Backdrop;
}

PLUGIN_EXPORT double Update(void* data)
{
	if (!isValidWinVersion) return 0.0;

	Measure* m = (Measure*)data;

	if (!m->DynamicVariables && (m->Mica != DWMSBT_NONE))
	{
		if (m->MicaFocus) SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
	}

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

	if (Error_HModule) return;

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
					SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
					m->MicaFocus = m->OMT_MicaFocus;
					SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
				}
				return;
			}
			if (compare(sargs, L"TOGGLEFOCUS"))
			{
				m->MicaFocus = m->MicaFocus == TRUE ? FALSE : TRUE;

				SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
				return;
			}
			if (compare(sargs, L"ENABLEFOCUS"))
			{
				m->MicaFocus = TRUE;

				SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
				return;
			}
			if (compare(sargs, L"DISABLEFOCUS"))
			{
				m->MicaFocus = FALSE;

				SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
				return;
			}
		}
		// DarkMode
		if (compare(sargs, L"TOGGLEMODE"))
		{
			m->DarkMode = m->DarkMode == TRUE ? FALSE : TRUE;

			SetSkinDarkMode(m->skin, m->DarkMode);
			return;
		}
		if (compare(sargs, L"LIGHTMODE"))
		{
			m->DarkMode = FALSE;

			SetSkinDarkMode(m->skin, m->DarkMode);
			return;
		}
		if (compare(sargs, L"DARKMODE"))
		{
			m->DarkMode = TRUE;

			SetSkinDarkMode(m->skin, m->DarkMode);
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

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		if (compare(sargs, L"ENABLECORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			if (m->Corner != DWMWCP_DONOTROUND) return;

			m->Corner = m->temp_Corner;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		if (compare(sargs, L"DISABLECORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			if (m->Corner == DWMWCP_DONOTROUND) return;

			m->temp_Corner = m->Corner;
			m->Corner = DWMWCP_DONOTROUND;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		if (compare(sargs, L"SETCORNER"))
		{
			if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

			m->Corner = DWMWCP_DONOTROUND;
			if (compare(sargs, L"ROUNDSMALL")) m->Corner = DWMWCP_ROUNDSMALL;
			if (compare(sargs, L"ROUNDWS")) m->Corner = (DWM_WINDOW_CORNER_PREFERENCE)4;
			if (compare(sargs, L"ROUND")) m->Corner = DWMWCP_ROUND;
			m->temp_Corner = m->Corner;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		// BorderVisible
		if (compare(sargs, L"TOGGLEBORDERS"))
		{
			m->CBorder = m->CBorder == DWMFCB_VISIBLE ? DWMFCB_HIDDEN : DWMFCB_VISIBLE;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		if (compare(sargs, L"ENABLEBORDERS"))
		{
			m->CBorder = DWMFCB_VISIBLE;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
			return;
		}
		if (compare(sargs, L"DISABLEBORDERS"))
		{
			m->CBorder = DWMFCB_HIDDEN;

			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
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
			if (m->Backdrop == DWMFB_WINDARK_BASE || m->Backdrop == DWMFB_WINLIGHT_BASE) m->Backdrop = m->Backdrop == DWMFB_WINDARK_BASE ? DWMFB_WINLIGHT_BASE : DWMFB_WINDARK_BASE;

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"LIGHTBACKDROP"))
		{
			if (m->Backdrop == DWMFB_DARK) m->Backdrop = DWMFB_LIGHT;
			if (m->Backdrop == DWMFB_DARK2) m->Backdrop = DWMFB_LIGHT2;
			if (m->Backdrop == DWMFB_DARK3) m->Backdrop = DWMFB_LIGHT3;
			if (m->Backdrop == DWMFB_DARK4) m->Backdrop = DWMFB_LIGHT4;
			if (m->Backdrop == DWMFB_DARK5) m->Backdrop = DWMFB_LIGHT5;
			if (m->Backdrop == DWMFB_WINDARK_BASE) m->Backdrop = DWMFB_WINLIGHT_BASE;

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"DARKBACKDROP"))
		{
			if (m->Backdrop == DWMFB_LIGHT) m->Backdrop = DWMFB_DARK;
			if (m->Backdrop == DWMFB_LIGHT2) m->Backdrop = DWMFB_DARK2;
			if (m->Backdrop == DWMFB_LIGHT3) m->Backdrop = DWMFB_DARK3;
			if (m->Backdrop == DWMFB_LIGHT4) m->Backdrop = DWMFB_DARK4;
			if (m->Backdrop == DWMFB_LIGHT5) m->Backdrop = DWMFB_DARK5;
			if (m->Backdrop == DWMFB_WINLIGHT_BASE) m->Backdrop = DWMFB_WINDARK_BASE;

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			return;
		}
		if (compare(sargs, L"SETBACKDROP"))
		{
			if (compare(sargs, L"LIGHT"))
			{
				if (compare(sargs, L"BASE")) m->Backdrop = DWMFB_WINLIGHT_BASE;
				if (compare(sargs, L"5")) m->Backdrop = DWMFB_LIGHT5;
				if (compare(sargs, L"4")) m->Backdrop = DWMFB_LIGHT4;
				if (compare(sargs, L"3")) m->Backdrop = DWMFB_LIGHT3;
				if (compare(sargs, L"2")) m->Backdrop = DWMFB_LIGHT2;
				if (compare(sargs, L"1")) m->Backdrop = DWMFB_LIGHT;
			}
			if (compare(sargs, L"DARK"))
			{
				if (compare(sargs, L"BASE")) m->Backdrop = DWMFB_WINDARK_BASE;
				if (compare(sargs, L"5")) m->Backdrop = DWMFB_DARK5;
				if (compare(sargs, L"4")) m->Backdrop = DWMFB_DARK4;
				if (compare(sargs, L"3")) m->Backdrop = DWMFB_DARK3;
				if (compare(sargs, L"2")) m->Backdrop = DWMFB_DARK2;
				if (compare(sargs, L"1")) m->Backdrop = DWMFB_DARK;
			}

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			return;
		}
	}

	if (compare(sargs, L"TOGGLEBLUR"))
	{
		if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE)
		{
			m->Accent = m->temp_Accent;
			m->Mica = m->temp_Mica;

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
			SetSkinCornerAndBorder(m->skin, m->Corner, m->CBorder);
		}
		else
		{
			m->temp_Accent = m->Accent;
			m->temp_Mica = m->Mica;
			m->Accent = DWMFB_DISABLED;
			m->Mica = DWMSBT_NONE;

			SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
			SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
			SetSkinCornerAndBorder(m->skin, DWMWCP_DONOTROUND, m->CBorder);
		}
		return;
	}
	if (compare(sargs, L"ENABLEBLUR"))
	{
		if (m->Accent != DWMFB_DISABLED || m->Mica != DWMSBT_NONE) return;

		m->Accent = m->temp_Accent;
		m->Mica = m->temp_Mica;

		SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
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

		SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
		SetSkinCornerAndBorder(m->skin, DWMWCP_DONOTROUND, m->CBorder);
		return;
	}
	if (compare(sargs, L"SETBLUR"))
	{
		if (m->Accent == DWMFB_DISABLED && m->Mica == DWMSBT_NONE) return;

		m->Accent = DWMFB_BLURBEHIND;
		m->Mica = DWMSBT_NONE;
		m->Effect = DWMFF_LUMINANCE_LOW;

		if (compare(sargs, L"ACRYLIC"))
		{
			m->Accent = DWMFB_ACRYLIC;
			m->Effect = (DWM_FROSTEDGLASS_UNDOCUMENTED)m->OMT_Effect;
		}
		if (compare(sargs, L"MICA"))
		{
			m->Accent = DWMFB_DISABLED;
			m->Mica = DWMSBT_MAINWINDOW;
			if (compare(sargs, L"ACRYLIC")) m->Mica = DWMSBT_TRANSIENTWINDOW;
			if (compare(sargs, L"ALT")) m->Mica = DWMSBT_TABBEDWINDOW;
		}

		m->temp_Accent = m->Accent;
		m->temp_Mica = m->Mica;

		SetSkinAccent(m->skin, m->Accent, m->Effect, m->Border, m->Backdrop);
		SetSkinMica(m->skin, m->Accent, m->Mica, m->MicaFocus);
		return;
	}
}

PLUGIN_EXPORT void Finalize(void* data)
{
	if (!isValidWinVersion)
	{
		RmLog(LOG_WARNING, L"FrostedGlass plugin is supported only on Windows 10 and 11.");
		return;
	}

	Measure* m = (Measure*)data;

	SetSkinAccent(m->skin, DWMFB_DISABLED, DWMFF_LUMINANCE_LOW, DWMFB_NONE, DWMFB_DARK);
	SetSkinMica(m->skin, DWMFB_DISABLED, DWMSBT_NONE, m->MicaFocus);
	SetSkinCornerAndBorder(m->skin, DWMWCP_DONOTROUND, DWMFCB_VISIBLE);

	unloadModule();

	delete m;
}