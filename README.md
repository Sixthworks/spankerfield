> [!NOTE]
> If you are going to use / share this, pack the compiled dll with VMProtect / Themida or anything else.

![image](https://i.imgur.com/9aj6wfa.png)

### Images
![image](https://i.imgur.com/oIAjluQ.gif)

![image](https://i.imgur.com/ofC2zpF.png)

![image](https://i.imgur.com/zVPTuvl.png)

![image](https://i.imgur.com/xjrL0wq.jpg)

![image](https://i.imgur.com/SXgmNoB.png)

![image](https://i.imgur.com/KFZsStZ.png)

![image](https://i.imgur.com/Frb1kYx.png)

### Features
- **User Interface**
  - ImGui-based interface
  - Runtime unloading
  - Log console with g3log
  - ImGui rendering

- **Combat Features**
  - Aimbot
  - C4 Bot
  - Infantry alert
  - Overheat control
  - Player blacklist
  - Auto-spot

- **ESP Features**
  - Player ESP
  - Vehicle ESP 
  - Explosives ESP
  - Health bars
  - Crosshair

- **Vehicle Features**
  - Controllable jet speed

- **Utility Features**
  - Spectator list
  - Name spoofer
  - Streamer mode
  - Anti-AFK    (Only works if you don't tab out)

- **Anti-Cheat Bypass**
  - PunkBuster bypass
  - FairFight bypass

### Build settings
- Configuration Properties -> General -> `Configuration Type: Dynamic Link Library (.dll)`
- Configuration Properties -> General -> `Platform Toolset: Visual Studio 2022 (v143)`
- Configuration Properties -> General -> `C++ Language Standard: ISO C++20 Standard (/std:c++20)`
- Configuration Properties -> Advanced -> `Character Set: Use Unicode Character Set`
- C/C++ -> Preprocessor -> Preprocessor Definitions: `_CRT_SECURE_NO_WARNINGS`
- C/C++ -> Precompiled Headers -> Precompiled Header File: `common.h`
- C/C++ -> General -> Additional Include Directories: `$(ProjectDir)include;$(ProjectDir)src;%(AdditionalIncludeDirectories)`
- Linker -> Input -> Additional Dependencies: ```fmt.lib
DirectXTK.lib
freetype.lib
g3log.lib
minhook.x64.lib
%(AdditionalDependencies)```
- Linker -> General -> Additional Library Directories: `$(ProjectDir)lib;%(AdditionalLibraryDirectories)`

### TODO
- [ ] Loadout editor
- [ ] Kill say functionality

I would be happy if anyone works on this cheat!
