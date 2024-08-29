#include <Windows.h>
#include "../API/RainmeterAPI.h"
#include <string>
#include <sstream>
#include <VersionHelpers.h>
#include <dwmapi.h>

// Overview: This is a blank canvas on which to build your plugin.

// Note: GetString and ExecuteBang have been commented out. If you need
// GetString and/or ExecuteBang and you have read what they are used for
// from the SDK docs, uncomment the function(s). Otherwise leave them
// commented out (or get rid of them)!


const COLORREF rgbNull = 0xFFFFFFFE;
const COLORREF rgbDefault = 0xFFFFFFFF;

const int WCA_ACCENT_POLICY = 19;

enum class AccentState
{
	DISABLED = 0,
	GRADIENT = 1, // not used (only gives a solid color based on Gradient Color)
	TRANSPARENTGRADIENT = 2, // not used (Always gives light blue for me >.>)
	BLURBEHIND = 3,
	ACRYLIC = 4,
	INVALID = 5
};

enum AccentFlags
{
	NONE = 0,
	LEFTBORDER = 0x20,
	TOPBORDER = 0x40,
	RIGHTBORDER = 0x80,
	BOTTOMBORDER = 0x100,
	ALLBORDERS = (LEFTBORDER | TOPBORDER | RIGHTBORDER | BOTTOMBORDER)
};

enum AccentDrop
{
	DARK = 0x00000000,
	DARK2 = 0x3F000000,
	DARK3 = 0x7E000000,
	DARK4 = 0xBD000000,
	DARK5 = 0xFF000000,
	LIGHT = 0x00FFFFFF,
	LIGHT2 = 0x3FFFFFFF,
	LIGHT3 = 0x7EFFFFFF,
	LIGHT4 = 0xBDFFFFFF,
	LIGHT5 = 0xFFFFFFFF,
};

struct ACCENTPOLICY
{
	int nAccentState;
	int nFlags;
	unsigned int nColor;
	int nAnimationId;
};

struct WINCOMPATTRDATA
{
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
};

HINSTANCE hModule;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
pSetWindowCompositionAttribute SetWindowCompositionAttribute;
typedef BOOL(WINAPI* pGetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
pGetWindowCompositionAttribute GetWindowCompositionAttribute;

HMODULE hDwmApi;
typedef HRESULT(WINAPI* pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
pDwmSetWindowAttribute SetWindowAttribute;

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

bool IsWindowsBuildOrGreater(WORD wMajorVersion, WORD wMinorVersion, DWORD dwBuildNumber) {
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0 };
	DWORDLONG mask = 0;

	mask = VerSetConditionMask(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	mask = VerSetConditionMask(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
	mask = VerSetConditionMask(mask, VER_BUILDNUMBER, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.dwBuildNumber = dwBuildNumber;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, mask) != FALSE;
}

bool SetSkinAccent(HWND hwnd, const int& border, const AccentState& state, const int& backdrop)
{

	if (hModule && SetWindowCompositionAttribute)
	{
		//ACCENTPOLICY policy = { (int)state, border, 0x00000000, 1 }; // set black with 1 in transparency because acrylic does not like transparent
		ACCENTPOLICY policy = { (int)state, border, backdrop, 1 }; // set black with 1 in transparency because acrylic does not like transparent
		WINCOMPATTRDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY) };
		SetWindowCompositionAttribute(hwnd, &data);
		return true;
	}
	return false;
}

void loadModule()
{
	if (!hModule || SetWindowCompositionAttribute == NULL) {
		hModule = LoadLibrary(TEXT("user32.dll"));
		SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");

		if (SetWindowCompositionAttribute == NULL)
		{
			RmLog(LOG_ERROR, L"Could not load the SetWindowCompositionAttribute function from user32.dll, did microsoft remove it?");
		}

		GetWindowCompositionAttribute = (pGetWindowCompositionAttribute)GetProcAddress(hModule, "GetWindowCompositionAttribute");

		if (GetWindowCompositionAttribute == NULL)
		{
			RmLog(LOG_ERROR, L"Could not load the GetWindowCompositionAttribute function from user32.dll, did microsoft remove it?");
		}

		hDwmApi = LoadLibrary(L"DWMAPI.dll");
		SetWindowAttribute = (pDwmSetWindowAttribute)GetProcAddress(hDwmApi, "DwmSetWindowAttribute");

		if (SetWindowAttribute == NULL) {
			RmLog(LOG_ERROR, L"Could not load the DwmSetWindowAttribute function from DWMAPI.dll");
		}
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

struct Measure
{
	HWND skin;
	AccentState prevState = (AccentState)0;
	AccentState nextState = (AccentState)0;
	int prevBorder = 0;
	DWM_WINDOW_CORNER_PREFERENCE prevCorner = DWMWCP_DONOTROUND;
	DWM_WINDOW_CORNER_PREFERENCE originalCorner = DWMWCP_DONOTROUND;
	DWM_WINDOW_CORNER_PREFERENCE nextCorner = DWMWCP_DONOTROUND;
	AccentDrop prevBackdrop = (AccentDrop)0;
	int prevBorderVisible = 1;
	bool doWarn = true;
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* m = new Measure;
	m->skin = RmGetSkinWindow(rm);
	loadModule();
	*data = m;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* m = (Measure*)data;
	m->skin = RmGetSkinWindow(rm);

	// Default Values
	AccentState accent = AccentState::BLURBEHIND;
	int borders = 0;
	DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DONOTROUND;
	AccentDrop backdrop = AccentDrop::DARK;
	int borderVisible = 1;

	// Type Option
	std::wstring type = RmReadString(rm, L"Type", L"Blur");

	if (_wcsicmp(type.c_str(), L"ACRYLIC") == 0) accent = AccentState::ACRYLIC;
	if (_wcsicmp(type.c_str(), L"NONE") == 0) accent = AccentState::DISABLED;

	if (!IsWindows10OrGreater() && !IsWindowsBuildOrGreater(10, 0, 17063) && accent == AccentState::ACRYLIC)
	{
		if (m->doWarn)
			RmLogF(rm, LOG_WARNING, L"Acrylic is not supported on windows 10 builds until build 17063. Falling back to blur.");
		accent = AccentState::BLURBEHIND;
	}

	if (!IsWindows10OrGreater() && (accent == AccentState::ACRYLIC || accent == AccentState::BLURBEHIND))
	{
		if (m->doWarn)
			RmLogF(rm, LOG_WARNING, L"This plugin is not supported on other platforms than Windows 10");

		accent = AccentState::DISABLED;
		m->doWarn = m->prevState != accent;
		m->prevState = accent;
		return;
	}

	// Border Option
	std::wstring borderTypes = RmReadString(rm, L"Border", L"");

	while (!borderTypes.empty())
	{
		if (compare(borderTypes, L"TOP")) borders |= TOPBORDER;
		else if (compare(borderTypes, L"LEFT")) borders |= LEFTBORDER;
		else if (compare(borderTypes, L"RIGHT")) borders |= RIGHTBORDER;
		else if (compare(borderTypes, L"BOTTOM")) borders |= BOTTOMBORDER;
		else if (compare(borderTypes, L"NONE")) break;
		else if (compare(borderTypes, L"ALL"))
		{
			borders = ALLBORDERS;
			break;
		}

		if (!borderTypes.empty() && !compare(borderTypes, L"|"))
		{
			if (m->doWarn)
				RmLogF(rm, LOG_ERROR, L"Invalid border format, expected | between tokens");

			borders = 0;
			break;
		}
	}

	// Corner Option
	std::wstring cornerType = RmReadString(rm, L"Corner", L"");

	if (_wcsicmp(cornerType.c_str(), L"ROUND") == 0) {
		corner = DWMWCP_ROUND;
	}
	else if (_wcsicmp(cornerType.c_str(), L"ROUNDSMALL") == 0) {
		corner = DWMWCP_ROUNDSMALL;
	}

	m->nextCorner = corner;
	m->originalCorner = corner;

	// Backdrop Option
	std::wstring backdropTypes = RmReadString(rm, L"Backdrop", L"");

	if (_wcsicmp(backdropTypes.c_str(), L"DARK2") == 0) backdrop = AccentDrop::DARK2;
	if (_wcsicmp(backdropTypes.c_str(), L"DARK3") == 0) backdrop = AccentDrop::DARK3;
	if (_wcsicmp(backdropTypes.c_str(), L"DARK4") == 0) backdrop = AccentDrop::DARK4;
	if (_wcsicmp(backdropTypes.c_str(), L"DARK5") == 0) backdrop = AccentDrop::DARK5;
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT") == 0) backdrop = AccentDrop::LIGHT;
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT2") == 0) backdrop = AccentDrop::LIGHT2;
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT3") == 0) backdrop = AccentDrop::LIGHT3;
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT4") == 0) backdrop = AccentDrop::LIGHT4;
	if (_wcsicmp(backdropTypes.c_str(), L"LIGHT5") == 0) backdrop = AccentDrop::LIGHT5;

	// BorderVisible Option
	std::wstring BorderVisibleTypes = RmReadString(rm, L"BorderVisible", L"");

	if (_wcsicmp(BorderVisibleTypes.c_str(), L"0") == 0) borderVisible = 0;

	// backwards compability
	if (RmReadInt(rm, L"BlurEnabled", 1) == 0)
	{
		m->nextState = accent;
		accent = AccentState::DISABLED;
		m->nextCorner = corner;
		m->originalCorner = corner;
		corner = DWMWCP_DONOTROUND;
	}

	// If nothing changed, do nothing
	if (m->prevState == accent && m->prevBorder == borders && m->prevCorner == corner && m->prevBackdrop == backdrop && m->prevBorderVisible == borderVisible) return;

	if (IsWindowsBuildOrGreater(10, 0, 22000))
	{
		if (SetWindowAttribute) {
			SetWindowAttribute(RmGetSkinWindow(rm), DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof corner);
			if (borderVisible == 1) SetWindowAttribute(RmGetSkinWindow(rm), DWMWA_BORDER_COLOR, &rgbDefault, sizeof rgbDefault);
			else SetWindowAttribute(RmGetSkinWindow(rm), DWMWA_BORDER_COLOR, &rgbNull, sizeof rgbNull);
		}
	}
	else {
		if (corner != DWMWCP_DONOTROUND) {
			if (m->doWarn) RmLogF(rm, LOG_WARNING, L"Round corner is not supported on windows 11 builds until build 22000. Falling back to do not round.");
			corner = DWMWCP_DONOTROUND;
			m->nextCorner = corner;
			m->originalCorner = corner;
		}
		if (backdrop != NULL) {
			if (m->doWarn) RmLogF(rm, LOG_WARNING, L"Backdrop is not supported on windows 11 builds until build 22000.");
			backdrop = AccentDrop::DARK;
		}
		if (borderVisible != 1)
		{
			if (m->doWarn) RmLogF(rm, LOG_WARNING, L"BorderVisible is not supported on windows 11 builds until build 22000.");
			borderVisible = 1;
		}
	}

	if (!(SetSkinAccent(RmGetSkinWindow(rm), borders, AccentState::DISABLED, backdrop) &&
		SetSkinAccent(RmGetSkinWindow(rm), borders, accent, backdrop))) {
		RmLogF(rm, LOG_ERROR, L"Could not load library user32.dll for some unknown reason.");
	}

	m->doWarn = m->prevState != accent || m->prevBorder != borders || m->prevCorner != corner || m->prevBackdrop != backdrop || m->prevBorderVisible != borderVisible;
	m->prevState = accent;
	m->prevBorder = borders;
	m->prevCorner = corner;
	m->prevBackdrop = backdrop;
	m->prevBorderVisible = borderVisible;
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
	Measure* m = (Measure*)data;
	std::wstring sargs = args;
	bool failed = false;

	if (IsWindowsBuildOrGreater(10, 0, 22000))
	{
		failed = 1;
		if (compare(sargs, L"TOGGLECORNER"))
		{
			if (m->prevState != AccentState::DISABLED)
			{

				if (m->prevCorner == DWMWCP_ROUNDSMALL)
				{
					m->prevCorner = DWMWCP_DONOTROUND;
					m->nextCorner = DWMWCP_ROUNDSMALL;
				}
				else if (m->prevCorner == DWMWCP_ROUND)
				{
					m->prevCorner = DWMWCP_DONOTROUND;
					m->nextCorner = DWMWCP_ROUND;
				}
				else
				{
					m->prevCorner = m->originalCorner;
					m->nextCorner = DWMWCP_DONOTROUND;
				}
			}
			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);

		}
		else if (compare(sargs, L"ENABLECORNER"))
		{
			if (m->prevState != AccentState::DISABLED)
			{
				m->nextCorner = m->originalCorner;
				m->prevCorner = m->nextCorner;
				SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
			}
		}
		else if (compare(sargs, L"DISABLECORNER"))
		{
			if (m->prevState != AccentState::DISABLED)
			{
				if (m->prevCorner == DWMWCP_ROUNDSMALL) m->nextCorner = DWMWCP_ROUNDSMALL;
				else if (m->prevCorner == DWMWCP_ROUND) m->nextCorner = DWMWCP_ROUND;
				if (m->prevCorner == DWMWCP_DONOTROUND) m->nextCorner = DWMWCP_DONOTROUND;
				m->prevCorner = DWMWCP_DONOTROUND;

				SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
			}
		}
		else if (compare(sargs, L"SETCORNER"))
		{
			if (m->prevState != AccentState::DISABLED) {
				if (compare(sargs, L"2"))
				{
					m->prevCorner = DWMWCP_ROUNDSMALL;
				}
				else if (compare(sargs, L"1"))
				{
					m->prevCorner = DWMWCP_ROUND;
				}
				else
				{
					m->prevCorner = DWMWCP_DONOTROUND;
				}
				SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
			}
		}
		if (compare(sargs, L"TOGGLEBACKDROP"))
		{
			if (m->prevBackdrop == AccentDrop::DARK || m->prevBackdrop == AccentDrop::LIGHT) m->prevBackdrop = m->prevBackdrop == AccentDrop::DARK ? AccentDrop::LIGHT : AccentDrop::DARK;
			else if (m->prevBackdrop == AccentDrop::DARK2 || m->prevBackdrop == AccentDrop::LIGHT2) m->prevBackdrop = m->prevBackdrop == AccentDrop::DARK2 ? AccentDrop::LIGHT2 : AccentDrop::DARK2;
			else if (m->prevBackdrop == AccentDrop::DARK3 || m->prevBackdrop == AccentDrop::LIGHT3) m->prevBackdrop = m->prevBackdrop == AccentDrop::DARK3 ? AccentDrop::LIGHT3 : AccentDrop::DARK3;
			else if (m->prevBackdrop == AccentDrop::DARK4 || m->prevBackdrop == AccentDrop::LIGHT4) m->prevBackdrop = m->prevBackdrop == AccentDrop::DARK4 ? AccentDrop::LIGHT4 : AccentDrop::DARK4;
			else if (m->prevBackdrop == AccentDrop::DARK5 || m->prevBackdrop == AccentDrop::LIGHT5) m->prevBackdrop = m->prevBackdrop == AccentDrop::DARK5 ? AccentDrop::LIGHT5 : AccentDrop::DARK2;
			failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
		}
		else if (compare(sargs, L"LIGHTBACKDROP"))
		{
			if (m->prevBackdrop == AccentDrop::DARK) m->prevBackdrop = AccentDrop::LIGHT;
			else if (m->prevBackdrop == AccentDrop::DARK2) m->prevBackdrop = AccentDrop::LIGHT2;
			else if (m->prevBackdrop == AccentDrop::DARK3) m->prevBackdrop = AccentDrop::LIGHT3;
			else if (m->prevBackdrop == AccentDrop::DARK4) m->prevBackdrop = AccentDrop::LIGHT4;
			else if (m->prevBackdrop == AccentDrop::DARK5) m->prevBackdrop = AccentDrop::LIGHT5;
			failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
		}
		else if (compare(sargs, L"DARKBACKDROP"))
		{
			if (m->prevBackdrop == AccentDrop::LIGHT) m->prevBackdrop = AccentDrop::DARK;
			else if (m->prevBackdrop == AccentDrop::LIGHT2) m->prevBackdrop = AccentDrop::DARK2;
			else if (m->prevBackdrop == AccentDrop::LIGHT3) m->prevBackdrop = AccentDrop::DARK3;
			else if (m->prevBackdrop == AccentDrop::LIGHT4) m->prevBackdrop = AccentDrop::DARK4;
			else if (m->prevBackdrop == AccentDrop::LIGHT5) m->prevBackdrop = AccentDrop::DARK5;
			failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
		}
		else if (compare(sargs, L"SETBACKDROP"))
		{
			if (compare(sargs, L"LIGHT"))
			{
				if (compare(sargs, L"5")) m->prevBackdrop = AccentDrop::LIGHT5;
				else if (compare(sargs, L"4")) m->prevBackdrop = AccentDrop::LIGHT4;
				else if (compare(sargs, L"3")) m->prevBackdrop = AccentDrop::LIGHT3;
				else if (compare(sargs, L"2")) m->prevBackdrop = AccentDrop::LIGHT2;
				else if (compare(sargs, L"1")) m->prevBackdrop = AccentDrop::LIGHT;
			}
			else if (compare(sargs, L"DARK"))
			{
				if (compare(sargs, L"5")) m->prevBackdrop = AccentDrop::DARK5;
				else if (compare(sargs, L"4")) m->prevBackdrop = AccentDrop::DARK4;
				else if (compare(sargs, L"3")) m->prevBackdrop = AccentDrop::DARK3;
				else if (compare(sargs, L"2")) m->prevBackdrop = AccentDrop::DARK2;
				else if (compare(sargs, L"1")) m->prevBackdrop = AccentDrop::DARK;
			}
			SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
		}
		if (compare(sargs, L"TOGGLEBORDERS"))
		{
			if (m->prevBorderVisible == 1)
			{
				SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &rgbNull, sizeof rgbNull);
				m->prevBorderVisible = 0;
			}
			else
			{
				SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &rgbDefault, sizeof rgbDefault);
				m->prevBorderVisible = 1;
			}
		}
		else if (compare(sargs, L"ENABLEBORDERS"))
		{
			SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &rgbDefault, sizeof rgbDefault);
			m->prevBorderVisible = 1;
		}
		else if (compare(sargs, L"DISABLEBORDERS"))
		{
			SetWindowAttribute(m->skin, DWMWA_BORDER_COLOR, &rgbNull, sizeof rgbNull);
			m->prevBorderVisible = 0;
		}
	}

	if (compare(sargs, L"TOGGLEBLUR"))
	{
		if (m->prevState == AccentState::BLURBEHIND)
		{
			m->prevState = AccentState::DISABLED;
			m->nextState = AccentState::BLURBEHIND;
		}
		else if (m->prevState == AccentState::ACRYLIC)
		{
			m->prevState = AccentState::DISABLED;
			m->nextState = AccentState::ACRYLIC;
		}
		else m->prevState = m->nextState;

		failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);

		if (IsWindowsBuildOrGreater(10, 0, 22000))
		{
			if (m->prevCorner == DWMWCP_ROUNDSMALL)
			{
				m->prevCorner = DWMWCP_DONOTROUND;
				m->nextCorner = DWMWCP_ROUNDSMALL;
			}
			else if (m->prevCorner == DWMWCP_ROUND)
			{
				m->prevCorner = DWMWCP_DONOTROUND;
				m->nextCorner = DWMWCP_ROUND;
			}
			else
			{
				if (m->prevState == AccentState::DISABLED) m->nextCorner = DWMWCP_DONOTROUND;
				m->prevCorner = m->nextCorner;
			}
			SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
		}
	}
	else if (compare(sargs, L"ENABLEBLUR"))
	{
		if (m->prevState == AccentState::DISABLED)
		{
			failed = SetSkinAccent(m->skin, m->prevBorder, m->nextState, m->prevBackdrop);
			m->prevState = m->nextState;
			if (IsWindowsBuildOrGreater(10, 0, 22000))
			{
				m->prevCorner = m->nextCorner;
				SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
			}
		}
	}
	else if (compare(sargs, L"DISABLEBLUR"))
	{
		if (m->prevState != AccentState::DISABLED)
		{
			if (m->prevState == AccentState::BLURBEHIND) m->nextState = AccentState::BLURBEHIND;
			else if (m->prevState == AccentState::ACRYLIC) m->nextState = AccentState::ACRYLIC;
			m->prevState = AccentState::DISABLED;
			failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
			if (IsWindowsBuildOrGreater(10, 0, 22000))
			{
				if (m->prevCorner == DWMWCP_ROUNDSMALL) m->nextCorner = DWMWCP_ROUNDSMALL;
				else if (m->prevCorner == DWMWCP_ROUND) m->nextCorner = DWMWCP_ROUND;
				if (m->prevCorner == DWMWCP_DONOTROUND) m->nextCorner = DWMWCP_DONOTROUND;
				m->prevCorner = DWMWCP_DONOTROUND;

				SetWindowAttribute(m->skin, DWMWA_WINDOW_CORNER_PREFERENCE, &m->prevCorner, sizeof m->prevCorner);
			}
		}
		
	}
	else if (compare(sargs, L"SETBLUR"))
	{
		if (m->prevState != AccentState::DISABLED){
			if (compare(sargs, L"2")) m->prevState = AccentState::ACRYLIC;
			else if (compare(sargs, L"1")) m->prevState = AccentState::BLURBEHIND;
			else m->prevState = AccentState::BLURBEHIND;
			failed = SetSkinAccent(m->skin, m->prevBorder, m->prevState, m->prevBackdrop);
		}
	}

	if (!failed)
	{
		// Error
		RmLog(LOG_ERROR, L"Could not load library user32.dll for some unknown reason.");
	}
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* m = (Measure*)data;
	// Disable the effect, the skin is unloaded
	if (!SetSkinAccent(m->skin, 0, AccentState::DISABLED, AccentDrop::DARK))
	{
		// Error
		RmLog(LOG_ERROR, L"Could not load library user32.dll for some unknown reason.");
	}

	unloadModule();
	delete m;
}
