<?
    If ( !Isset ( $_GET['appid'] ) || !Isset ( $_GET['version'] ) )
    {
        Echo "How to use this page: gameUpToDate.Php?appid=730&version=512.128.8192";
    }
    Else
    {
        $AppId = Mb_Strtolower ( Trim ( $_GET['appid'] ) );
        $Version = Mb_Strtolower ( Trim ( $_GET['version'] ) );
        If ( Mb_Strstr ( $AppId, "%" ) || Mb_Strstr ( $Version, "%" ) )
        {
            Echo "How to use this page: gameUpToDate.Php?appid=730&version=512.128.8192";
        }
        Else
        {
            $Error = False;
            For ( $It = 0; $It < Mb_Strlen ( $Version ); $It++ )
            {
                If ( ( $Version[ $It ] != '.' ) && ( ( $Version[ $It ] < '0' ) || ( $Version[ $It ] > '9' ) ) )
                {
                    $Error = True;
                }
            }
            If ( $Error )
            {
                Echo "How to use this page: gameUpToDate.Php?appid=730&version=512.128.8192";
            }
            Else
            {
                $Buffer = Sprintf ( "http://api.steampowered.com/ISteamApps/UpToDateCheck/v0001/?appid=%d&version=%s", ( ( Int ) ( $AppId ) ), $Version );
                $Contents = @File_Get_Contents ( $Buffer );
                If ( $Contents )
                {
                    Echo Mb_Strtolower ( Trim ( $Contents ) );
                }
                Else
                {
                    Echo "Steam error.";
                }
            }
        }
    }
?>