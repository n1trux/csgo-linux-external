linux-csgo-external USE AT OWN RISK !!!
===================

Linux x86 cheat for x86 and x64 linux systems

Compiled and tested on:
- Ubuntu
- Xubuntu
- Arch
- Debian (Jessie)
- ElementaryOS
- ApricityOS

![hackin](http://i.imgur.com/fiKLjnQ.jpg)

Credits:
- Original author: s0beit
- Original author credits: BoToX & buFFy
- Maintainer/Updater: gogobebe2, laazyboy13 & community

# Install
`cd linux-csgo-external-master`

`cmake .`

`make`

# Create Toggle Keybind

## Synopsis
* Solves #2 
    - (Kind of)
    - No toggle for just radar.
* Press a keybind to toggle the cheat.

## Notes
* Requires Xlib (both `libx11-dev:i386` and `libx11-dev`, `# apt-get install libx11-dev libx11-dev:i386` on Debian based systems
* Bind in this Pull Request is Ctrl+Shift+X, but this is not optimal for gameplay.

## Known Bugs
* Does not work with Numlock enabled.
