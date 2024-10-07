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
Measure=Plugin
Plugin=FrostedGlass
BlurEnabled=1
; Type = None | Blur | Acrylic | Mica | MicaAcrylic | MicaAlt
; Mica, MicaAcrylic, MicaAlt only works for Windows 11.
Type=Acrylic
; Border = None | Top | Left | Right | Bottom | All
; Borders are ignored if Mica is enabled.
Border=Top | Left | Right | Bottom
; DarkMode = 0 | 1 ; Only Wworks for Windows 11.
; When using Mica it can does toggle between dark/light mode.
; When using Round Corner it can does more shadow.
DarkMode=1
; Corner = None | Round | RoundSmall ; Only Wworks for Windows 11.
; Using Corner disable Border option.
Corner=Round
; BorderVisible = 0 | 1 ; Only Wworks for Windows 11.
; Only works if Corner is Round or Roundsmall.
BorderVisible=1
; Backdrop = Dark | Dark1... | Dark5 | Light | Light1... | Light5 ; Only Wworks for Windows 11.
; Only works if Acrylic is enabled.
Backdrop=Dark
; MicaOnFocus = 0 | 1 ; Only Wworks for Windows 11.
; If it's enabled, be sure of the skin can update and measure too.
; Ex. [Rainmeter] Update=1000 and [FrostedGlass] UpdateDivider = 1.
MicaOnFocus=0
; NOTE: Command Measures are useless if the Measure has an update cycle.
; so if you want use Command Measures, set the UpdateDivider to -1,
DynamicVariables=1
UpdateDivider=-1
```

## Type

Valid options for `Type` are:
- Win +10 Required > `None`, `Blur`. `Acrylic` 
- Win 11 Required > `Mica`, `MicaAcrylic`, `MicaAlt`

## DarkMode
Valid options for `DarkMode` are:
`0`, `1`

## MicaOnFocus
Valid options for `MicaOnFocus` are:
`0`, `1`

## Border

Valid options for `Border` are:
`Top`, `Left`, `Right`, `Bottom`, `All`, `None`

## Corner

Valid options for `Corner` are:
`Round`, `RoundSmall`, `None`

## BorderVisible

Valid options for `BorderVisible` are:
`0`, `1`

## Backdrop

Valid options for `Backdrop` are:

<details>

<summary>Dark - Dark5</summary>

## Dark
 ![Rainmeter_gaYVD99tTR](https://github.com/user-attachments/assets/75772a61-1b90-4e85-a6c7-feb382194ac2)
## Dark2
![Rainmeter_QCI16Qv4PK](https://github.com/user-attachments/assets/aec6f2c1-5148-4d35-8918-126f58bdbe83)
## Dark3
![Rainmeter_wosNOmKq5m](https://github.com/user-attachments/assets/ab3d87fd-e331-457d-a5b4-8a7c7a573ecd)
## Dark4
![Rainmeter_etag4SvTR5](https://github.com/user-attachments/assets/e3937d3e-46b0-46c1-b7ca-d66de0cd2296)
## Dark5
![Rainmeter_LAisynEaux](https://github.com/user-attachments/assets/d07e7708-a4c1-46e7-b6b0-e5f735e44300)
</details>
<details>

<summary>Light - Light5</summary>

## Light
![Rainmeter_4rZxfu0Uft](https://github.com/user-attachments/assets/38584a50-b7d4-473f-9004-9642ea80cda2)
## Light2
![Rainmeter_V88cVKPh80](https://github.com/user-attachments/assets/28c9ca23-b1b8-4386-88a9-9791b250e85f)
## Light3
![Rainmeter_PhyKG2Sdbn](https://github.com/user-attachments/assets/c5d88ef1-37eb-4551-8b6d-2156b869b6bd)
## Light4
![Rainmeter_37Rs3vAgmJ](https://github.com/user-attachments/assets/db2d7a31-b94e-44b4-9184-2fecb2f906cd)
## Light5
![Rainmeter_5Sjp7hn0Oh](https://github.com/user-attachments/assets/ffe6bd8c-02b6-4369-a307-4dc3b42fb11c)
</details>

<br>

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
