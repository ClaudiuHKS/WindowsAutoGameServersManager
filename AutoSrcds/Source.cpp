
#pragma warning ( push )

#pragma warning ( disable : 4530 )
#pragma warning ( disable : 4007 )

#include < nlohmann/json.hpp >

#include < iostream >
#include < fstream >

#include < winsock2.h >
#include < psapi.h >
#include < wbemidl.h >

class qryPrcsData
{
public:

    ::IWbemLocator * pLoc { };
    ::IWbemServices * pSvc { };
    ::IWbemClassObject * pObj { };
    ::IEnumWbemClassObject * pEnm { };

    wchar_t * pLng { };
    wchar_t * pQry { };
    wchar_t * pSrv { };

    ::tagVARIANT Prc { };
    ::tagVARIANT Prm { };
    ::tagVARIANT Pth { };
};

::std::wstring toLwr ( ::std::wstring Str ) noexcept
{
    ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::towlower );

    return Str;
}

unsigned long slfPrcRun ( void ) noexcept
{
    static unsigned long Prcs [ 2048I16 ] { }, Rs { }, Slf { }, It { };
    static wchar_t slfNme [ 512I16 ] { }, othNme [ 512I16 ] { };
    static void * pPrc { }, * pCur { };

    if ( !( pCur = ::GetCurrentProcess ( ) ) ||
         !::K32GetModuleFileNameExW ( pCur, { }, slfNme, ( ( ( sizeof ( slfNme ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) ) ||
         !::K32EnumProcesses ( Prcs, ( sizeof ( Prcs ) ), &Rs ) || !( Slf = ::GetCurrentProcessId ( ) ) )
        return { };

    for ( It = { }; It < ( ( Rs ) / ( sizeof ( unsigned long ) ) ); It++ )
    {
        if ( ( Prcs [ It ] ) && ( Prcs [ It ] != Slf ) && ( pPrc = ::OpenProcess ( 2097151UL, { }, Prcs [ It ] ) ) )
        {
            ::K32GetModuleFileNameExW ( pPrc, { }, othNme, ( ( ( sizeof ( othNme ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );

            ::CloseHandle ( pPrc );
            pPrc = { };

            if ( !( ::lstrcmpiW ( othNme, slfNme ) ) )
                return ( Prcs [ It ] );
        }
    }

    return { };
}

unsigned long prcRun ( ::std::wstring prcNm ) noexcept
{
    static unsigned long Prcs [ 2048I16 ] { }, Rs { }, Slf { }, It { };
    static wchar_t Nm [ 512I16 ] { };
    static void * pPrc { };

    if ( !::K32EnumProcesses ( Prcs, ( sizeof ( Prcs ) ), &Rs ) || !( Slf = ::GetCurrentProcessId ( ) ) )
        return { };

    for ( It = { }; It < ( ( Rs ) / ( sizeof ( unsigned long ) ) ); It++ )
    {
        if ( ( Prcs [ It ] ) && ( Prcs [ It ] != Slf ) && ( pPrc = ::OpenProcess ( 2097151UL, { }, Prcs [ It ] ) ) )
        {
            ::K32GetModuleFileNameExW ( pPrc, { }, Nm, ( ( ( sizeof ( Nm ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );

            ::CloseHandle ( pPrc );
            pPrc = { };

            if ( ::std::wcsstr ( ::toLwr ( Nm ).c_str ( ), ::toLwr ( prcNm ).c_str ( ) ) )
                return ( Prcs [ It ] );
        }
    }

    return { };
}

::std::wstring trncPth ( ::std::wstring Pth ) noexcept
{
    static ::std::size_t Ps { };

    if ( Pth.empty ( ) || Pth.ends_with ( L'/' ) || Pth.ends_with ( L'\\' ) )
        return Pth;

    if ( ( Ps = Pth.find_last_of ( L'/' ) ) != ::std::wstring::npos || ( Ps = Pth.find_last_of ( L'\\' ) ) != ::std::wstring::npos )
        Pth.erase ( Ps + 1I8 );

    return Pth;
}

::std::wstring toUcd ( ::std::string Str ) noexcept
{
    static wchar_t Data [ 1024I16 ] { };

    ::MultiByteToWideChar ( 65001UI16, { }, Str.c_str ( ), -1I8, Data,
                            ( ( ( sizeof ( Data ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );

    return Data;
}

::std::string toMbc ( ::std::wstring Str ) noexcept
{
    static char Data [ 1024I16 ] { };

    ::WideCharToMultiByte ( 65001UI16, { }, Str.c_str ( ), -1I8, Data,
                            ( ( ( sizeof ( Data ) ) / ( sizeof ( char ) ) ) - 1I8 ), { }, { } );

    return Data;
}

::std::wstring stmExe ( void ) noexcept
{
    static ::HKEY__ * pKey { };
    static wchar_t Str [ 512I16 ] { };
    static unsigned long Sz { };

    if ( !::RegOpenKeyExW ( ( ( ::HKEY__ * ) ( 2147483649ULL ) ), L"SOFTWARE\\Valve\\Steam", { }, 983103UL, &pKey ) && pKey )
    {
        ::std::memset ( Str, { }, ( sizeof ( Str ) ) );

        Sz = ( ( ( sizeof ( Str ) ) / ( sizeof ( wchar_t ) ) ) - 1UI8 );

        ::RegQueryValueExW ( pKey, L"SteamExe", { }, { }, ( ( unsigned char * ) ( Str ) ), &Sz );

        ::RegCloseKey ( pKey );
        pKey = { };

        if ( ::std::wcslen ( Str ) )
            return Str;
    }

    else if ( pKey )
    {
        ::RegCloseKey ( pKey );
        pKey = { };
    }

    return { };
}

::std::wstring rmAll ( ::std::wstring Str, ::std::wstring Rm ) noexcept
{
    static ::std::size_t Ps { };

    if ( Str.empty ( ) || Rm.empty ( ) )
        return Str;

    while ( ( Ps = Str.find ( Rm ) ) != ::std::wstring::npos )
        Str.erase ( Ps, Rm.length ( ) );

    return Str;
}

::std::wstring rplAll ( ::std::wstring Str, ::std::wstring Fr, ::std::wstring To ) noexcept
{
    static ::std::size_t Ps { };

    if ( Str.empty ( ) || Fr.empty ( ) || To.empty ( ) )
        return Str;

    while ( ( Ps = Str.find ( Fr ) ) != ::std::wstring::npos )
        Str.replace ( Ps, Fr.length ( ), To );

    return Str;
}

void qryPrcsFree ( ::qryPrcsData & Var ) noexcept
{
    if ( Var.pLoc )
    {
        Var.pLoc->Release ( );
        Var.pLoc = { };
    }

    if ( Var.pSvc )
    {
        Var.pSvc->Release ( );
        Var.pSvc = { };
    }

    if ( Var.pObj )
    {
        Var.pObj->Release ( );
        Var.pObj = { };
    }

    if ( Var.pEnm )
    {
        Var.pEnm->Release ( );
        Var.pEnm = { };
    }

    if ( Var.pLng )
    {
        ::SysFreeString ( Var.pLng );
        Var.pLng = { };
    }

    if ( Var.pQry )
    {
        ::SysFreeString ( Var.pQry );
        Var.pQry = { };
    }

    if ( Var.pSrv )
    {
        ::SysFreeString ( Var.pSrv );
        Var.pSrv = { };
    }

    ::VariantClear ( &Var.Prc );
    Var.Prc = { };

    ::VariantClear ( &Var.Prm );
    Var.Prm = { };

    ::VariantClear ( &Var.Pth );
    Var.Pth = { };

    Var = { };
}

bool qryPrcs ( ::std::vector < unsigned long > & Prcs, ::std::vector < ::std::wstring > & Prms, ::std::vector < ::std::wstring > & Pths ) noexcept
{
    static ::qryPrcsData Data { };
    static unsigned long _ { };

    Prcs.clear ( );
    Prcs = { };

    Prms.clear ( );
    Prms = { };

    Pths.clear ( );
    Pths = { };

    if ( ::CoInitializeEx ( { }, { } ) ||
         ::CoInitializeSecurity ( { }, -1L, { }, { }, { }, 3UL, { }, { }, { } ) ||
         ::CoCreateInstance ( ::CLSID_WbemLocator, { }, 1UL, ::IID_IWbemLocator, ( void ** ) &Data.pLoc ) ||
         !Data.pLoc || !( Data.pSrv = ::SysAllocString ( L"ROOT\\CIMV2" ) ) ||
         Data.pLoc->ConnectServer ( Data.pSrv, { }, { }, { }, { }, { }, { }, &Data.pSvc ) ||
         !Data.pSvc || !( Data.pLng = ::SysAllocString ( L"WQL" ) ) ||
         !( Data.pQry = ::SysAllocString ( L"SELECT ProcessId, CommandLine, ExecutablePath FROM Win32_Process" ) ) ||
         Data.pSvc->ExecQuery ( Data.pLng, Data.pQry, { }, { }, &Data.pEnm ) || !Data.pEnm )
    {
        ::qryPrcsFree ( Data );
        Data = { };

        ::CoUninitialize ( );

        return { };
    }

    _ = { };

    while ( !Data.pEnm->Next ( -1L, 1UL, &Data.pObj, &_ ) )
    {
        if ( Data.pObj )
        {
            Data.pObj->Get ( L"ProcessId", { }, &Data.Prc, { }, { } );
            Prcs.emplace_back ( Data.Prc.ulVal );
            ::VariantClear ( &Data.Prc );
            Data.Prc = { };

            Data.pObj->Get ( L"CommandLine", { }, &Data.Prm, { }, { } );
            ::std::wstring prmStr ( Data.Prm.bstrVal, ::SysStringLen ( Data.Prm.bstrVal ) );
            Prms.emplace_back ( prmStr );
            ::VariantClear ( &Data.Prm );
            Data.Prm = { };

            Data.pObj->Get ( L"ExecutablePath", { }, &Data.Pth, { }, { } );
            ::std::wstring pthStr ( Data.Pth.bstrVal, ::SysStringLen ( Data.Pth.bstrVal ) );
            Pths.emplace_back ( pthStr );
            ::VariantClear ( &Data.Pth );
            Data.Pth = { };

            Data.pObj->Release ( );
            Data.pObj = { };
        }

        _ = { };
    }

    ::qryPrcsFree ( Data );
    Data = { };

    ::CoUninitialize ( );

    return true;
}

unsigned long prcByNme ( ::std::wstring Nm, ::std::vector < ::std::wstring > & Pths, ::std::vector < unsigned long > & Prcs ) noexcept
{
    static ::std::size_t It { };

    It = { };

    for ( auto & Pth : Pths )
    {
        ++It;

        if ( ::std::wcsstr ( ::toLwr ( Pth ).c_str ( ), ::toLwr ( Nm ).c_str ( ) ) )
            return ( Prcs [ ( It - 1UI8 ) ] );
    }

    return { };
}

bool allSvRun ( ::nlohmann::json & Cfg, ::std::vector < ::std::wstring > & Args ) noexcept
{
    static ::std::size_t Sv { }, Rn { };

    Sv = { };
    Rn = { };

    for ( auto & Srv : Cfg [ "Srvs" ] )
    {
        if ( Srv.size ( ) < 3UI8 ||
             Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
             !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
            continue;

        Sv++;

        for ( auto & Arg : Args )
        {
            if ( ::std::wcsstr ( ::rplAll ( ::rmAll ( ::toLwr ( Arg ), L"\"" ), L"  ", L" " ).c_str ( ),
                                 ::rplAll ( ::rmAll ( ::toLwr ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ) ), L"\"" ), L"  ", L" " ).c_str ( ) ) )
                Rn++;
        }
    }

    return ( ( bool ) ( Rn == Sv ) );
}

void killAllSv ( ::nlohmann::json & Cfg, ::std::vector < ::std::wstring > & Args, ::std::vector < unsigned long > & Prcs ) noexcept
{
    static ::std::size_t prIt { }, agIt { };
    static long long Tme { };
    static void * pPr { };

    for ( auto & Srv : Cfg [ "Srvs" ] )
    {
        if ( Srv.size ( ) < 3UI8 ||
             Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
             !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
            continue;

        prIt = { };
        agIt = { };

        for ( auto & Arg : Args )
        {
            ++agIt;

            if ( ::std::wcsstr ( ::rplAll ( ::rmAll ( ::toLwr ( Arg ), L"\"" ), L"  ", L" " ).c_str ( ),
                                 ::rplAll ( ::rmAll ( ::toLwr ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ) ), L"\"" ), L"  ", L" " ).c_str ( ) ) )
            {
                for ( auto & Prc : Prcs )
                {
                    if ( ( ++prIt == agIt ) && ( pPr = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
                    {
                        ::TerminateProcess ( pPr, 1UI8 );
                        ::WaitForSingleObject ( pPr, 4294967295UL );

                        ::CloseHandle ( pPr );
                        pPr = { };

                        ::std::time ( &Tme );
                        ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Killed A Game Server (" << Prc << L")" << ::std::endl << ::std::endl;
                    }
                }
            }
        }
    }
}

::std::vector < unsigned char > rdUpFl ( ::std::string Sv, unsigned short Pr, ::std::string Vs, ::std::string Ph, long long To ) noexcept
{
    static ::std::vector < unsigned char > Dv { };
    static ::std::size_t Sk { };
    static char Db [ 2048UI16 ] { };
    static long long Ts { };
    static int Rs { }, It { };
    static ::fd_set Fd { };
    static ::timeval Tv { };
    static ::hostent * pSh { };
    static ::sockaddr_in Sa { };

    Dv.clear ( );
    Dv = { };

    if ( !( pSh = ::gethostbyname ( Sv.c_str ( ) ) ) )
        return Dv;

    Sk = ::socket ( 2I8, 1I8, 6I8 );

    ::std::memset ( &Sa, { }, ( sizeof ( Sa ) ) );

    Sa.sin_family = 2I8;
    Sa.sin_port = ::htons ( Pr );

    ::std::memcpy ( &Sa.sin_addr, pSh->h_addr_list [ 0I8 ], pSh->h_length );

    Rs = ::connect ( Sk, ( ( ::sockaddr * ) ( &Sa ) ), ( sizeof ( Sa ) ) );

    if ( Rs == -1I8 )
    {
        ::closesocket ( Sk );
        Sk = { };

        return Dv;
    }

    Rs = ::std::snprintf ( Db, ( ( ( sizeof ( Db ) ) / ( sizeof ( char ) ) ) - 1I8 ),
                           "GET %s HTTP/%s\r\nHost: %s\r\n\r\n", Ph.c_str ( ), Vs.c_str ( ), Sv.c_str ( ) );

    Rs = ::send ( Sk, Db, Rs, { } );

    if ( Rs == -1I8 )
    {
        ::closesocket ( Sk );
        Sk = { };

        return Dv;
    }

    Ts = ::std::time ( { } );

    while ( true )
    {
        ::Sleep ( 1UI8 );

        Tv.tv_sec = { };
        Tv.tv_usec = 25I8;

        ::std::memset ( &Fd, { }, ( sizeof ( Fd ) ) );

        ( &Fd )->fd_count = 1I8;
        ( &Fd )->fd_array [ 0I8 ] = Sk;

        if ( ( ( ::select ( ( ( int ) ( Sk + 1UI8 ) ), &Fd, { }, { }, &Tv ) ) > 0I8 ) || ( ( ::std::time ( { } ) - Ts ) > To ) )
            break;
    }

    while ( ( ( Rs = ::recv ( Sk, Db, ( ( ( sizeof ( Db ) ) / ( sizeof ( char ) ) ) - 1I8 ), { } ) ) > 0I8 ) )
    {
        for ( It = { }; It < Rs; It++ )
            Dv.emplace_back ( ( ( unsigned char ) ( Db [ It ] ) ) );
    }

    ::closesocket ( Sk );
    Sk = { };

    return Dv;
}

::std::wstring rdVs ( ::nlohmann::json & Cfg ) noexcept
{
    static ::std::wifstream Fl { };
    static ::std::wstring Ln { }, Vs { };

    Vs.clear ( );
    Vs = { };

    if ( Cfg [ "UpdateKeyFile" ].is_discarded ( ) || Cfg [ "UpdateKeyFile" ].empty ( ) || !Cfg [ "UpdateKeyFile" ].is_string ( ) ||
         Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ).empty ( ) || !::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ) ) ) )
        return Vs;

    Fl.open ( ::toUcd ( Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ) ), 1I8, 64I8 );

    if ( !Fl.is_open ( ) )
    {
        Fl.clear ( );
        Fl = { };

        return Vs;
    }

    Ln.clear ( );
    Ln = { };

    while ( ::std::getline ( Fl, Ln ) )
    {
        if ( ::std::wcsstr ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::toLwr (
            Ln ), L"\"" ), L"\t" ), L"\n" ), L"\r" ), L"\v" ), L"\f" ), L" " ).c_str ( ), L"patchversion=" ) )
            Vs = ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::toLwr (
                Ln ), L"\"" ), L"\t" ), L"\n" ), L"\r" ), L"\v" ), L"\f" ), L" " ), L"patchversion=" );

        Ln.clear ( );
        Ln = { };
    }

    Fl.close ( );

    Fl.clear ( );
    Fl = { };

    return Vs;
}

bool needUpd ( ::nlohmann::json & Cfg ) noexcept
{
    static ::std::vector < unsigned char > Data { };
    static ::std::string Vrs { };

    if ( Cfg [ "AppId" ].is_discarded ( ) || Cfg [ "AppId" ].empty ( ) || !Cfg [ "AppId" ].is_number_unsigned ( ) )
        return { };

    Vrs = ::toMbc ( ::rdVs ( Cfg ) );

    if ( Vrs.empty ( ) )
        return { };

    Data.clear ( );
    Data = { };

    Data = ::rdUpFl ( "hattrick.go.ro", 80UI8, "1.1",
                      ( "/gameUpToDate.Php?appid=" + ( ::std::to_string ( Cfg [ "AppId" ].get < int > ( ) ) ) +
                        "&version=" + Vrs ), 8I8 );

    if ( Data.empty ( ) )
        return { };

    if ( Data.back ( ) )
        Data.emplace_back ( 0UI8 );

    if ( Data.back ( ) )
        Data.push_back ( 0UI8 );

    if ( !( ::std::strstr ( ( ( char * ) ( Data.data ( ) ) ), "\"up_to_date\":" ) ) )
        return { };

    return ( ( bool ) ( !( ::std::strstr ( ( ( char * ) ( Data.data ( ) ) ), "\"up_to_date\":true" ) ) ) );
}

int wmain ( void ) noexcept
{
    ::WSAData Data { };

    if ( ::WSAStartup ( 514UI16, &Data ) )
    {
        ::WSACleanup ( );
        Data = { };

        ::std::wcout << L"WSAStartup Failed" << ::std::endl;

        return 1I8;
    }

    ::std::setlocale ( 2I8, ".utf8" );

    ::SetPriorityClass ( ::GetCurrentProcess ( ), 128UL );
    ::SetProcessPriorityBoost ( ::GetCurrentProcess ( ), { } );

    if ( ::slfPrcRun ( ) )
    {
        ::WSACleanup ( );
        Data = { };

        ::std::wcout << L"This Application Is Already Running" << ::std::endl;

        return 1I8;
    }

    if ( !::GetConsoleWindow ( ) )
        ::AllocConsole ( );

    if ( ::GetConsoleWindow ( ) )
        ::SetConsoleTitleW ( L"Auto Gaming Servers" );

    wchar_t Buffer [ 512I16 ] { };

    ::GetModuleFileNameW ( { }, Buffer, ( ( ( sizeof ( Buffer ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );

    ::std::wstring Path { ::trncPth ( Buffer ), };

    ::HKEY__ * pKey { };

    if ( ::RegOpenKeyExW ( ( ( ::HKEY__ * ) ( 2147483649ULL ) ), L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", { }, 983103UL, &pKey ) ||
         !pKey )
    {
        if ( pKey )
        {
            ::RegCloseKey ( pKey );
            pKey = { };
        }

        ::WSACleanup ( );
        Data = { };

        ::std::wcout << L"RegOpenKeyExW Failed" << ::std::endl;

        return 1I8;
    }

    if ( pKey )
    {
        if ( ::std::filesystem::exists ( Path + L"0" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_0", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"1" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_1", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"2" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_2", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"3" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_3", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"4" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_4", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"5" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_5", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"6" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_6", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"7" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_7", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"8" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_8", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( Path + L"9" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_9", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else
            ::RegSetValueExW ( pKey, L"AutoSrcds", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        ::RegCloseKey ( pKey );
        pKey = { };
    }

    ::std::ifstream File { };

    File.open ( Path + L"CFG.INI", 1I8, 64I8 );

    if ( !File.is_open ( ) )
    {
        File.clear ( );
        File = { };

        ::WSACleanup ( );
        Data = { };

        ::std::wcout << L"CFG.INI Failed" << ::std::endl;

        return 1I8;
    }

    ::nlohmann::json Cfg { ::nlohmann::json::parse ( File, { }, { }, true ), };

    File.close ( );

    File.clear ( );
    File = { };

    if ( Cfg.is_discarded ( ) || Cfg.empty ( ) || !Cfg.is_object ( ) || Cfg [ "Srvs" ].is_discarded ( ) || Cfg [ "Srvs" ].empty ( ) || !Cfg [ "Srvs" ].is_object ( ) )
    {
        Cfg.clear ( );
        Cfg = { };

        ::WSACleanup ( );
        Data = { };

        ::std::wcout << L"JSON Validation Failed" << ::std::endl;

        return 1I8;
    }

    if ( !Cfg [ "Scan" ].is_discarded ( ) && !Cfg [ "Scan" ].empty ( ) && Cfg [ "Scan" ].is_number_unsigned ( ) )
        ::std::wcout << L"Scan Interval " << ::std::max ( Cfg [ "Scan" ].get < unsigned long > ( ), 10000UL ) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Scan Interval 20000 ms" << ::std::endl;

    if ( !Cfg [ "Skip" ].is_discarded ( ) && !Cfg [ "Skip" ].empty ( ) && Cfg [ "Skip" ].is_number_unsigned ( ) )
        ::std::wcout << L"Skip Interval " << ::std::max ( Cfg [ "Skip" ].get < unsigned long > ( ), 5000UL ) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Skip Interval 10000 ms" << ::std::endl;

    if ( !Cfg [ "AwayUpdate" ].is_discarded ( ) && !Cfg [ "AwayUpdate" ].empty ( ) && Cfg [ "AwayUpdate" ].is_number_unsigned ( ) )
        ::std::wcout << L"Away Interval " << ::std::max ( Cfg [ "AwayUpdate" ].get < unsigned long > ( ), 30000UL ) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Away Interval 180000 ms" << ::std::endl;

    ::std::wcout << ::std::endl;

    ::std::vector < ::std::wstring > Prms { };
    ::std::vector < ::std::wstring > Pths { };
    ::std::vector < unsigned long > Prcs { };

    ::std::wstring Key [ 6I8 ] { { }, { }, { }, { }, { }, { }, };
    ::std::wstring Por { };
    ::std::wstring Arg { };
    ::std::wstring Exe { };

    ::tagLASTINPUTINFO liInf { };
    ::_PROCESS_INFORMATION prInf { };
    ::_STARTUPINFOW suInf { };

    unsigned long Prc { };
    long long Tme { };
    void * pPr { };
    bool Run { };

    while ( true )
    {
        ::std::memset ( &liInf, { }, ( sizeof ( liInf ) ) );

        liInf.cbSize = ( sizeof ( liInf ) );

        if ( !( ::gethostbyname ( "google.com" ) ) || ( ::prcByNme ( L"steamcmd.exe", Pths, Prcs ) ) || ( ::prcRun ( L"steamcmd.exe" ) ) ||
             ( ( ::GetLastInputInfo ( &liInf ) ) &&
               ( ( ::GetTickCount64 ( ) - liInf.dwTime ) < ( ( !Cfg [ "AwayUpdate" ].is_discarded ( ) &&
                                                               !Cfg [ "AwayUpdate" ].empty ( ) &&
                                                               Cfg [ "AwayUpdate" ].is_number_unsigned ( ) ) ?
                                                             ( ::std::max ( Cfg [ "AwayUpdate" ].get < unsigned long > ( ), 30000UL ) ) : ( 180000UL ) ) ) ) )
        {

        }

        else if ( ( ::needUpd ( Cfg ) ) || ( !Cfg [ "UpdateMainFile" ].is_discarded ( ) && !Cfg [ "UpdateMainFile" ].empty ( ) && Cfg [ "UpdateMainFile" ].is_string ( ) &&
                                             !Cfg [ "UpdateMainFile" ].get < ::std::string > ( ).empty ( ) &&
                                             ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) &&
                                             !Cfg [ "SteamCMD" ].is_discarded ( ) && !Cfg [ "SteamCMD" ].empty ( ) && Cfg [ "SteamCMD" ].is_object ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Path" ].is_discarded ( ) && !Cfg [ "SteamCMD" ][ "Path" ].empty ( ) && Cfg [ "SteamCMD" ][ "Path" ].is_string ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ).empty ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Args" ].is_discarded ( ) && !Cfg [ "SteamCMD" ][ "Args" ].empty ( ) && Cfg [ "SteamCMD" ][ "Args" ].is_string ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Args" ].get < ::std::string > ( ).empty ( ) &&
                                             ::std::filesystem::exists ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ) ) ) )
        {
            if ( !Cfg [ "KillSteamApp" ].is_discarded ( ) && !Cfg [ "KillSteamApp" ].empty ( ) && Cfg [ "KillSteamApp" ].is_boolean ( ) && Cfg [ "KillSteamApp" ].get < bool > ( ) )
            {
                Exe = ::stmExe ( );

                if ( !Exe.empty ( ) && ::std::filesystem::exists ( Exe ) &&
                     ( ( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) || ( Prc = ::prcRun ( L"steam.exe" ) ) ) &&
                     ( pPr = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
                {
                    ::TerminateProcess ( pPr, 1UI8 );
                    ::WaitForSingleObject ( pPr, 4294967295UL );

                    ::CloseHandle ( pPr );
                    pPr = { };

                    ::std::time ( &Tme );
                    ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Killed Steam APP (" << Prc << L")" << ::std::endl << ::std::endl;
                }
            }

            if ( ::std::filesystem::exists ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ) ) &&
                 ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) && !::prcRun ( L"steamcmd.exe" ) )
            {
                if ( ::qryPrcs ( Prcs, Prms, Pths ) )
                {
                    ::killAllSv ( Cfg, Prms, Prcs );

                    if ( ::std::filesystem::exists ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ) ) &&
                         ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) &&
                         !::prcByNme ( L"steamcmd.exe", Pths, Prcs ) && !::prcRun ( L"steamcmd.exe" ) )
                    {
                        ::std::memset ( &suInf, { }, ( sizeof ( suInf ) ) );
                        ::std::memset ( &prInf, { }, ( sizeof ( prInf ) ) );

                        suInf.cb = ( sizeof ( suInf ) );

                        if ( ::CreateProcessW ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ).c_str ( ),
                                                ( ( wchar_t * ) ( ( L"\"" + ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ) + L"\" " +
                                                                    ::toUcd ( Cfg [ "SteamCMD" ][ "Args" ].get < ::std::string > ( ) ) ).c_str ( ) ) ),
                                                { }, { }, { }, 144UL, { }, { },
                                                &suInf, &prInf ) && prInf.hProcess )
                        {
                            if ( ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) )
                                ::std::filesystem::remove ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) );

                            ::std::time ( &Tme );
                            ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched Steam CMD (" << prInf.dwProcessId << L")" << ::std::endl << ::std::endl;

                            ::WaitForSingleObject ( prInf.hProcess, 4294967295UL );

                            ::CloseHandle ( prInf.hProcess );
                            prInf.hProcess = { };

                            ::CloseHandle ( prInf.hThread );
                            prInf.hThread = { };

                            ::std::time ( &Tme );
                            ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Steam CMD Ended (" << prInf.dwProcessId << L")" << ::std::endl << ::std::endl;
                        }
                    }
                }
            }
        }

        else if ( ::qryPrcs ( Prcs, Prms, Pths ) )
        {
            for ( auto & Srv : Cfg [ "Srvs" ] )
            {
                if ( Srv.size ( ) < 3UI8 ||
                     Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
                     Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
                     Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
                     !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
                    continue;

                Por = ::toUcd ( Srv [ "Port" ].get < ::std::string > ( ) );

                Key [ 0I8 ] = L"-port " + Por;
                Key [ 1I8 ] = L"+port " + Por;

                Key [ 2I8 ] = L"-hostport " + Por;
                Key [ 3I8 ] = L"+hostport " + Por;

                Key [ 4I8 ] = L"-host_port " + Por;
                Key [ 5I8 ] = L"+host_port " + Por;

                Run = true;

                for ( auto & Prm : Prms )
                {
                    Arg = ::rplAll ( ::rmAll ( ::toLwr ( Prm ), L"\"" ), L"  ", L" " );

                    if ( ( ::std::wcsstr ( Arg.c_str ( ), L"-console" ) || ::std::wcsstr ( Arg.c_str ( ), L"+console" ) ) &&
                         ( ::std::wcsstr ( Arg.c_str ( ), L"-game" ) || ::std::wcsstr ( Arg.c_str ( ), L"+game" ) ) &&
                         ( ::std::wcsstr ( Arg.c_str ( ), Key [ 0I8 ].c_str ( ) ) || ::std::wcsstr ( Arg.c_str ( ), Key [ 1I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Arg.c_str ( ), Key [ 2I8 ].c_str ( ) ) || ::std::wcsstr ( Arg.c_str ( ), Key [ 3I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Arg.c_str ( ), Key [ 4I8 ].c_str ( ) ) || ::std::wcsstr ( Arg.c_str ( ), Key [ 5I8 ].c_str ( ) ) ) )
                        Run = false;
                }

                if ( Run )
                {
                    if ( !Cfg [ "KillSteamApp" ].is_discarded ( ) && !Cfg [ "KillSteamApp" ].empty ( ) && Cfg [ "KillSteamApp" ].is_boolean ( ) &&
                         Cfg [ "KillSteamApp" ].get < bool > ( ) )
                    {
                        Exe = ::stmExe ( );

                        if ( !Exe.empty ( ) && ::std::filesystem::exists ( Exe ) &&
                             ( ( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) || ( Prc = ::prcRun ( L"steam.exe" ) ) ) &&
                             ( pPr = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
                        {
                            ::TerminateProcess ( pPr, 1UI8 );
                            ::WaitForSingleObject ( pPr, 4294967295UL );

                            ::CloseHandle ( pPr );
                            pPr = { };

                            ::std::time ( &Tme );
                            ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Killed Steam APP (" << Prc << L")" << ::std::endl << ::std::endl;
                        }
                    }

                    ::std::memset ( &suInf, { }, ( sizeof ( suInf ) ) );
                    ::std::memset ( &prInf, { }, ( sizeof ( prInf ) ) );

                    suInf.cb = ( sizeof ( suInf ) );

                    if ( ::CreateProcessW ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ).c_str ( ),
                                            ( ( wchar_t * ) ( ( L"\"" + ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) + L"\" " +
                                                                ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ) ).c_str ( ) ) ),
                                            { }, { }, { }, 144UL, { }, { },
                                            &suInf, &prInf ) && prInf.hProcess )
                    {
                        ::CloseHandle ( prInf.hProcess );
                        prInf.hProcess = { };

                        ::CloseHandle ( prInf.hThread );
                        prInf.hThread = { };

                        ::std::time ( &Tme );
                        ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched " << Por << L" (" << prInf.dwProcessId << L")" << ::std::endl << ::std::endl;

                        ::Sleep ( ( !Cfg [ "Skip" ].is_discarded ( ) && !Cfg [ "Skip" ].empty ( ) && Cfg [ "Skip" ].is_number_unsigned ( ) ) ?
                                  ( ::std::max ( Cfg [ "Skip" ].get < unsigned long > ( ), 5000UL ) ) : ( 10000UL ) );

                        if ( ::qryPrcs ( Prcs, Prms, Pths ) && ::allSvRun ( Cfg, Prms ) && !Cfg [ "RunSteamApp" ].is_discarded ( ) &&
                             !Cfg [ "RunSteamApp" ].empty ( ) && Cfg [ "RunSteamApp" ].is_boolean ( ) && Cfg [ "RunSteamApp" ].get < bool > ( ) )
                        {
                            Exe = ::stmExe ( );

                            if ( !Exe.empty ( ) && ::std::filesystem::exists ( Exe ) &&
                                 !( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) && !( Prc = ::prcRun ( L"steam.exe" ) ) )
                            {
                                ::std::memset ( &suInf, { }, ( sizeof ( suInf ) ) );
                                ::std::memset ( &prInf, { }, ( sizeof ( prInf ) ) );

                                suInf.cb = ( sizeof ( suInf ) );

                                if ( ::CreateProcessW ( Exe.c_str ( ),
                                                        ( ( wchar_t * ) ( ( L"\"" + Exe + L"\" /high -high +high" ).c_str ( ) ) ),
                                                        { }, { }, { }, 144UL, { }, { },
                                                        &suInf, &prInf ) && prInf.hProcess )
                                {
                                    ::CloseHandle ( prInf.hProcess );
                                    prInf.hProcess = { };

                                    ::CloseHandle ( prInf.hThread );
                                    prInf.hThread = { };

                                    ::std::time ( &Tme );
                                    ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched Steam APP (" << prInf.dwProcessId << L")" << ::std::endl << ::std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }

        ::Sleep ( ( !Cfg [ "Scan" ].is_discarded ( ) && !Cfg [ "Scan" ].empty ( ) && Cfg [ "Scan" ].is_number_unsigned ( ) ) ?
                  ( ::std::max ( Cfg [ "Scan" ].get < unsigned long > ( ), 10000UL ) ) : ( 20000UL ) );
    }

    Cfg.clear ( );
    Cfg = { };

    ::WSACleanup ( );
    Data = { };

    return { };
}

#pragma warning ( pop )
