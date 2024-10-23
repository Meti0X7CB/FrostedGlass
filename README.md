# Frosted Glass

A Rainmeter plugin that allows you to easily create blur effect on the skin window - (Original work of [TheAzack9](https://github.com/TheAzack9/FrostedGlass))

Windows 11 rounded corner style is now supported thanks to [khanhas](https://github.com/khanhas).

## Old version.

![OyT05WQgKi](https://github.com/user-attachments/assets/489cbb39-68d5-4b30-b009-a59754c79382)

## New Version + Light Backdrop + Not visible Border.

![euOemFe9sx](https://github.com/user-attachments/assets/52e718dd-22b1-44da-b73f-b8239bc23907)

## Functionality

An example of the plugin in use: 
```ini

[FrostedGlass]
Measure = Plugin
Plugin = FrostedGlass
; ==========================
; ==== NOTE: Plugin commands may will not work
; ========== if you add "DynamicVariables=1"
; ========== and if the skin has an update
; ========== cycle, unless you add an
; ========== "UpdateDivider=-1" to the measure.
; ==========================
; ==== Option: BlueEnabled
; ==== Default Value: 1
; ==== Total Values: 0 | 1
; ==== Description: Disable (0) / Enable (1) blur.
; ==== Recommendation: Use "Disabled" option to remove blur.
BlueEnabled = 1
; ==========================
; ==== Option: DarkMode
; ==== Default Value: 0
; ==== Total Values: 0 | 1
; ==== Description: Disable (0) / Enable (1) dark mode for Mica.
; ================= Less (0) / More (1) Shadow for Round Corner.
; ==== Compatibility: Corner(Round), Type(Mica|MicaAcrylic|MicaAlt) required.
; ==== OS: Windows 11 Required.
DarkMode = 1
; ==========================
; ==== Option: MicaOnFocus
; ==== Default Value: 0
; ==== Total Values: 0 | 1
; ==== Description: (0) Mica effect always enabled.
; ================= (1) Allows to use a solid color if the skin loses focus.
; ==== Compatibility: Type(Mica|MicaAcrylic|MicaAlt) required.
; ==== OS: Windows 11 Required.
; ==== Recommendation: The skin must have an update cycle and the measure too.
MicaOnFocus = 0
; ==========================
; ==== Option: Type
; ==== Default Value: None
; ==== Total Values: None | Blur | Acrylic | Mica | MicaAcrylic | MicaAlt
; ==== Description: Apply a transparency effect to the skin.
; ==== Compatibility: Mica effect requires Windows 11.
Type = Acrylic
; ==========================
; ==== Option: Border
; ==== Default Value: None
; ==== Total Values: None | Top | Left | Right | Bottom | All
; ==== Description: Apply a border effect to the skin.
; ==== Compatibility: Not available if any Corner is enabled.
; =================== Not available if Mica effect is enabled.
Border = Top | Bottom
; ==========================
; ==== Option: Corner
; ==== Default Value: None
; ==== Total Values: None | Round | RoundWs | RoundSmall
; ==== Description: Add round corners to the skin
; ==== OS: Windows 11 Required.
Corner = Round
; ==========================
; ==== Option: BorderVisible
; ==== Default Value: 1
; ==== Total Values: 0 | 1
; ==== Description: Remove (0) / Add (1) border if any Corner is enabled. 
; ==== Compatibility: Corner(Round|RoundSmall) required.
; ==== OS: Windows 11 Required.
BorderVisible = 1
; ==========================
; ==== Option: Backdrop
; ==== Default Value: Dark
; ==== Total Values: Dark | Dark2 | Dark3 | Dark4 | Dark5 
; ================== Light | Light2 | Light3 | Light4 | Light5
; ================== DarkBase | LightBase
; ==== Description: Add a tint to the acrylic effect.
; ==== Compatibility: Type(Acrylic) required.
; ==== OS: Windows 11 Required.
Backdrop = LightBase
; ==========================
; ==== Option: Effect
; ==== Default Value: None
; ==== Total Values: None | Luminance
; ==== Description: Increase the tint luminosity from the 
; ================= <Backdrop> option.
; ==== Compatibility: Type(Acrylic) required.
; ==== OS: Windows 11 Required.
; ==== Recommendation: Use with Backdrop(LightBase|DarkBase).
Effect = Luminance
; ==========================
UpdateDivider = 1

```

## Commands

### Windows 10/11 bangs:
Blur:
- `ToggleBlur`, `EnableBlur`, `DisableBlur`
- `SetBlur` : Set Blur type.

### Windows 11 bangs:

DarkMode:
- `ToggleMode`
- `LightMode` : Light Mode if Mica is enabled | Reduces shadows for round corner.
- `DarkMode` : Dark mode if Mica is enabled | Increase the shadow for round corner.

MicaOnFocus:
- `ToggleFocus`
- `SetFocus` : Set the focus on the skin if there's not mica effect.
- `EnableFocus` : Enable the focus detection.
- `DisableFocus` : Disable the focus detection, mica effect is always enabled.

Blur:
- `SetBlur Acrylic` : Set Acrylic type.
- `SetBlur Mica` : Set Mica type. (DarkMode Compability).
- `SetBlur Mica Acrylic` : Set MicaAcrylic type. (DarkMode Compability).
- `SetBlur Mica Alt` : Set MicaAlt type. (DarkMode Compability).

Corner:
- `ToggleCorner`, `EnableCorner`, `DisableCorner`
- `SetCorner Round` : Set Round Corner. (DarkMode Compability).
- `SetCorner RoundSmall` : Set Round Small Corner.

BorderVisible:
- `ToggleBorders`, `EnableBorders`, `DisableBorders`

Backdrop:
- `ToggleBackdrop`, `EnableBackdrop`, `DisableBackdrop`
- `SetBackdrop Dark 1`, `SetBackdrop Dark 2`, `SetBackdrop Dark 3`, `SetBackdrop Dark 4`, `SetBackdrop Dark 5`
- `SetBackdrop Light 1`,`SetBackdrop Light 2`,`SetBackdrop Light 3`,`SetBackdrop Light 4`,`SetBackdrop Light 5`

## Minimum Requirements:

- Windows 10 or higher
- Rainmeter 4.5.20.3803 or higher

Note: You will need Windows 11 for the next options to work!
- DarkMode
- MicaOnFocus
- Corner
- Backdrop
- BorderVisible

## Installation

Download the latest version from [Releases](https://github.com/KazukiGames82/FrostedGlass/releases) tab.
Install the skin by double-clicking the .rmskin file and follow the steps in the installer.

### Optional

You can also download the .zip file if you want to bundle the plugin in your skin.

## Note
This project is the work of [TheAzack9](https://github.com/TheAzack9) with the modification for Win 11 style support added by [khanhas](https://github.com/khanhas), compiled the project and packaged the plugin for public use by [Meti0X7CB](https://github.com/Meti0X7CB).
- KazukiGames82 added new functions and fixed mesaure commands.
