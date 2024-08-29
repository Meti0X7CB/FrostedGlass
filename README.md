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
Type = Acrylic
Border = Left|Right
; Using Corner option disable Border option.
Corner = Round
; Backdrop only works with Acrylic.
Backdrop = Light
; VisibleBorder only works with Round/RoundSmall Corner.
VisibleBorder= 0
```

## Type

Valid options for `Type` are:

`Blur`, `Acrylic`, `None`
<br><br>
## Border

Valid options for `Border` are:

`Top`, `Left`, `Right`, `Bottom`, `All`, `None`
<br><br>
## Corner

Valid options for `Corner` are:

`Round`, `RoundSmall`, `None`
<br><br>
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

## BorderVisible

Valid options for `BorderVisible` are:

`0`, `1`
<br><br>
## Commands

General Plugin:
- `ToggleBlur`, `EnableBlur`, `DisableBlur`
- `SetBlur 1` : Set Blur type.
- `SetBlur 2` : Set Acrylic type.

Corner:
- `ToggleCorner`, `EnableCorner`, `DisableCorner`
- `SetCorner 1` : Set Round Corner.
- `SetCorner 2` : Set Round Small Corner.

Backdrop:
- `ToggleBackdrop`, `EnableBackdrop`, `DisableBackdrop`
- `SetBackdrop Dark 1`, `SetBackdrop Dark 2`, `SetBackdrop Dark 3`, `SetBackdrop Dark 4`, `SetBackdrop Dark 5`
- `SetBackdrop Light 1`,`SetBackdrop Light 2`,`SetBackdrop Light 3`,`SetBackdrop Light 4`,`SetBackdrop Light 5`

BorderVisible:
- `ToggleBorders`, `EnableBorders`, `DisableBorders`
<br><br>
## Minimum Requirements:

- Windows 10 or higher
- Rainmeter 4.5.17.3700 or higher

Note: You will need Windows 11 for the next options to work!
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
