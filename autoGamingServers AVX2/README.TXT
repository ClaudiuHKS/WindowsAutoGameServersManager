
{
    "Item" :    "HLDS | CZ",

    "Scan" :    20000,  /* def = 20000 (20.0s), min = 10000 (10.0s) */  // wait a bit between scanning game servers to save processor
    "Skip" :    10000,  /* def = 10000 (10.0s), min = 5000 (5.0s) */    // wait a bit right after launching a game server instance to save processor

    "Srvs" :
    {
        "27115" :
        {
            "Port" :    "27115",
            "Path" :    "G:/STEAMCMDCZ/STEAMAPPS/COMMON/HALF-LIFE/HLDS.EXE",    /* '/' or '\\' */
            "Args" :    "-pingboost 64 +pingboost 64 -usercon +usercon -nohltv +nohltv -nobots +nobots -ip 192.168.100.2 +ip 192.168.100.2 -hushasserts +hushasserts -threads 8 +threads 8 -high +high -console +console -game czero +game czero -map fy_snow +map fy_snow -maxplayers 32 +maxplayers 32 -tickrate 512 +tickrate 512 -port 27115 +port 27115 -exec server.cfg +exec server.cfg"   // no comma (last entry)
        }   // no comma (last entry)
    },

    "UpdateMainFile" :  "G:/STEAMCMDCZ/UPDATE_CZ",  // if this file ever exists it will be removed and steam cmd launched
    "UpdateKeyFile" :   "G:/STEAMCMDCZ/STEAMAPPS/COMMON/HALF-LIFE/CZERO/STEAM.INF", // steam.inf file path
    "AppId" :           80, // https://developer.valvesoftware.com/wiki/Dedicated_Servers_List  /* https://developer.valvesoftware.com/wiki/Steam_Application_IDs */
    "AwayUpdate" :      180000, /* def = 180000 (180.0s), min = 30000 (30.0s) */    // a computer user must be away for this long in order to start steam cmd or game servers
    "KillSteamApp" :    true,   // kill steam app before steam cmd update and before srcds.exe or hlds.exe launch, if installed and running
    "RunSteamApp" :     true,   // run steam app when all srcds.exe or hlds.exe instances are launched, if steam app is installed and not already running

    "SteamCMD" :
    {
        "Path" :    "G:/STEAMCMDCZ/STEAMCMD.EXE",   /* '/' or '\\' */
        "Args" :    "+login anonymous +app_update 90 +app_set_config 90 mod czero +app_update 90 +exit" // no comma (last entry)
    }   // no comma (last entry)
}
