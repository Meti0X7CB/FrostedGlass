# Frosted Glass

A Rainmeter plugin that allows you to easily create blur effect on the skin window - (Original work of [TheAzack9](https://github.com/TheAzack9/FrostedGlass))

Windows 11 rounded corner style is now supported thanks to [khanhas](https://github.com/khanhas).

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

`Dark`, `Light`
<br><br>
## BorderVisible

Valid options for `BorderVisible` are:

`0`, `1`
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
