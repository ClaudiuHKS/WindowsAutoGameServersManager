# Windows Auto Game Servers Manager

Auto Start Up Your Game Servers If They Don't Already Run And Auto Update Them

**Architecture**

- Use `Release x64`. Use `Release x86` only if `Release x64` fails.

**Running One Game**

- You need a folder containing `AutoSrcds.exe` and `CFG.INI`.
- Configure `CFG.INI` for your game.

**Running Multiple Games**

- You need at least two folders as above.
- Only one folder should contain `AutoSrcds.exe` and `CFG.INI`.
- The other folders should also contain one of the following files: `0`, `1`, `2`, ...

**Details**

- Launches by itself when `Windows` starts up.
- Needs https://aka.ms/vs/17/release/vc_redist.x64.exe for `Release x64`.
- Needs https://aka.ms/vs/17/release/vc_redist.x86.exe for `Release x86`.
