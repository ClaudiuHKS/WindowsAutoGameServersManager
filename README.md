# Windows Auto Game Servers Manager

Auto Start Up Your Game Servers If They Don't Already Run And Auto Update Them

**Running One Game**

- You need a folder containing `AutoSrcds.exe` and `CFG.INI`.
- Configure `CFG.INI` for your game.

**Running Multiple Games**

- You need at least two folders as above.
- Only one folder should contain `AutoSrcds.exe` and `CFG.INI`.
- The other folders should also contain one of the following files: `0`, `1`, `2`, ...

**Automatically Updating The Game Servers**

- Your game server must create the `UpdateKeyFile` file by itself at the right time.
- You will need a custom plugin for that.
