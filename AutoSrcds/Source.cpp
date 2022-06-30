
#include < nlohmann/json.hpp >

#include < iostream >
#include < fstream >

#include < winsock2.h >
#include < psapi.h >
#include < wbemidl.h >

class qryPrcsData
{
public:

    __forceinline qryPrcsData ( void ) noexcept
    {
        pLoc = { };
        pSvc = { };
        pObj = { };
        pEnm = { };

        pLng = { };
        pQry = { };
        pSrv = { };

        Prc = { };
        Prm = { };
        Pth = { };
    }

    __forceinline ~qryPrcsData ( void ) noexcept
    {
        freeAll ( );
    }

public:

    void __forceinline freeAll ( void ) noexcept
    {
        if ( pLoc ) pLoc->Release ( ), pLoc = { };
        if ( pSvc ) pSvc->Release ( ), pSvc = { };
        if ( pObj ) pObj->Release ( ), pObj = { };
        if ( pEnm ) pEnm->Release ( ), pEnm = { };

        if ( pLng ) ::SysFreeString ( pLng ), pLng = { };
        if ( pQry ) ::SysFreeString ( pQry ), pQry = { };
        if ( pSrv ) ::SysFreeString ( pSrv ), pSrv = { };

        ::VariantClear ( &Prc ), Prc = { };
        ::VariantClear ( &Prm ), Prm = { };
        ::VariantClear ( &Pth ), Pth = { };
    }

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

static ::std::wstring __forceinline toLwr ( ::std::wstring Str ) noexcept // unicode string to lower
{
    ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::towlower );
    return Str;
}

static unsigned long __forceinline slfPrcRun ( void ) noexcept // self proc already running, same path, same name, insensitive
{
    static unsigned long Prcs [ 4096I16 ] { }, Rs { }, Slf { }, It { };
    static wchar_t slfNme [ 4096I16 ] { }, othNme [ 4096I16 ] { };
    static void * pPrc { }, * pCur { };

    if ( !( pCur = ::GetCurrentProcess ( ) ) || !::K32GetModuleFileNameExW ( pCur, { }, slfNme, ( ( ( sizeof ( slfNme ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) ) ||
         !::K32EnumProcesses ( Prcs, ( sizeof ( Prcs ) ), &Rs ) || !( Slf = ::GetCurrentProcessId ( ) ) )
        return { };

    for ( It = { }; It < ( ( Rs ) / ( sizeof ( unsigned long ) ) ); It++ )
    {
        if ( ( Prcs [ It ] ) && ( Prcs [ It ] != Slf ) && ( pPrc = ::OpenProcess ( 2097151UL, { }, Prcs [ It ] ) ) )
        {
            ::K32GetModuleFileNameExW ( pPrc, { }, othNme, ( ( ( sizeof ( othNme ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );
            ::CloseHandle ( pPrc );
            pPrc = { };

            if ( !::lstrcmpiW ( othNme, slfNme ) )
                return Prcs [ It ];
        }
    }

    return { };
}

static unsigned long __forceinline prcRun ( ::std::wstring prcNm ) noexcept // a proc is running right now, part of name, insensitive
{
    static unsigned long Prcs [ 4096I16 ] { }, Rs { }, Slf { }, It { };
    static wchar_t Nm [ 4096I16 ] { };
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
                return Prcs [ It ];
        }
    }

    return { };
}

static ::std::wstring __forceinline trncPth ( ::std::wstring Pth ) noexcept // convert "e:/steamcmd/any.exe" to "e:/steamcmd/"
{
    if ( Pth.empty ( ) || Pth.ends_with ( L'/' ) || Pth.ends_with ( L'\\' ) )
        return Pth;

    ::std::size_t Ps { };
    if ( ( Ps = Pth.find_last_of ( L'/' ) ) != ::std::wstring::npos || ( Ps = Pth.find_last_of ( L'\\' ) ) != ::std::wstring::npos )
        Pth.erase ( Ps + 1I8 );

    return Pth;
}

static ::std::wstring __forceinline toUcd ( ::std::string Str ) noexcept // multibyte string to unicode
{
    static wchar_t Data [ 8192I16 ] { };
    ::MultiByteToWideChar ( 65001UI16, { }, Str.c_str ( ), -1I8, Data, ( ( ( sizeof ( Data ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );
    return Data;
}

static ::std::string __forceinline toMbc ( ::std::wstring Str ) noexcept // unicode string to multibyte
{
    static char Data [ 8192I16 ] { };
    ::WideCharToMultiByte ( 65001UI16, { }, Str.c_str ( ), -1I8, Data, ( ( ( sizeof ( Data ) ) / ( sizeof ( char ) ) ) - 1I8 ), { }, { } );
    return Data;
}

static ::std::wstring __forceinline stmExe ( void ) noexcept // get steam app exe file path
{
    static ::HKEY__ * pKey { };
    static wchar_t Str [ 4096I16 ] { };
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

    return { };
}

static ::std::wstring __forceinline rmAll ( ::std::wstring Str, ::std::wstring Rm ) noexcept // remove all in string, sensitive
{
    if ( Str.empty ( ) || Rm.empty ( ) )
        return Str;

    ::std::size_t Ps { };
    while ( ( Ps = Str.find ( Rm ) ) != ::std::wstring::npos )
        Str.erase ( Ps, Rm.length ( ) );

    return Str;
}

static ::std::wstring __forceinline rplAll ( ::std::wstring Str, ::std::wstring Fr, ::std::wstring To ) noexcept // replace all in string, sensitive
{
    if ( Str.empty ( ) || Fr.empty ( ) || To.empty ( ) )
        return Str;

    ::std::size_t Ps { };
    while ( ( Ps = Str.find ( Fr ) ) != ::std::wstring::npos )
        Str.replace ( Ps, Fr.length ( ), To );

    return Str;
}

static bool __forceinline qryPrcs ( ::std::vector < unsigned long > & Prcs, ::std::vector < ::std::wstring > & Prms, ::std::vector < ::std::wstring > & Pths ) noexcept
{
    ::qryPrcsData Data { };

    Prcs.clear ( );
    Prms.clear ( );
    Pths.clear ( );

    if ( ::CoInitializeEx ( { }, { } ) || ::CoInitializeSecurity ( { }, -1L, { }, { }, { }, 3UL, { }, { }, { } ) ||
         ::CoCreateInstance ( ::CLSID_WbemLocator, { }, 1UL, ::IID_IWbemLocator, ( void ** ) &Data.pLoc ) || !Data.pLoc || !( Data.pSrv = ::SysAllocString ( L"ROOT\\CIMV2" ) ) ||
         Data.pLoc->ConnectServer ( Data.pSrv, { }, { }, { }, { }, { }, { }, &Data.pSvc ) || !Data.pSvc || !( Data.pLng = ::SysAllocString ( L"WQL" ) ) ||
         !( Data.pQry = ::SysAllocString ( L"SELECT ProcessId, CommandLine, ExecutablePath FROM Win32_Process" ) ) ||
         Data.pSvc->ExecQuery ( Data.pLng, Data.pQry, { }, { }, &Data.pEnm ) || !Data.pEnm )
    {
        Data.freeAll ( );
        ::CoUninitialize ( );
        return { };
    }

    unsigned long _ { };
    while ( !Data.pEnm->Next ( -1L, 1UL, &Data.pObj, &_ ) )
    {
        if ( Data.pObj )
        {
            Data.pObj->Get ( L"ProcessId", { }, &Data.Prc, { }, { } );
            Prcs.emplace_back ( Data.Prc.ulVal );

            Data.pObj->Get ( L"CommandLine", { }, &Data.Prm, { }, { } );
            ::std::wstring prmStr ( Data.Prm.bstrVal, ::SysStringLen ( Data.Prm.bstrVal ) );
            Prms.emplace_back ( prmStr );

            Data.pObj->Get ( L"ExecutablePath", { }, &Data.Pth, { }, { } );
            ::std::wstring pthStr ( Data.Pth.bstrVal, ::SysStringLen ( Data.Pth.bstrVal ) );
            Pths.emplace_back ( pthStr );

            Data.pObj->Release ( );
            Data.pObj = { };
        }

        _ = { };
    }

    Data.freeAll ( );
    ::CoUninitialize ( );
    return true;
}

static unsigned long __forceinline prcByNme ( ::std::wstring Nm, const ::std::vector < ::std::wstring > & Pths, const ::std::vector < unsigned long > & Prcs ) noexcept
{ // a proc by this part of name was running at some point, insensitive
    ::std::size_t It { };
    for ( const auto & Pth : Pths )
    {
        if ( ++It && ::std::wcsstr ( ::toLwr ( Pth ).c_str ( ), ::toLwr ( Nm ).c_str ( ) ) )
            return Prcs [ ( It - 1UI8 ) ];
    }

    return { };
}

static bool __forceinline allSvRun ( const ::nlohmann::json & Cfg, const ::std::vector < ::std::wstring > & Args ) noexcept // all game servers running
{
    unsigned long long Sv { }, Rn { };
    for ( const auto & Srv : Cfg [ "Srvs" ] )
    {
        if ( Srv.size ( ) < 3UI8 ||
             Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
             !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
            continue;

        Sv++;

        for ( const auto & Arg : Args )
        {
            if ( ::std::wcsstr ( ::rmAll ( ::toLwr ( ::rplAll ( Arg, L"  ", L" " ) ), L"\"" ).c_str ( ),
                                 ::rmAll ( ::toLwr ( ::rplAll ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ), L"  ", L" " ) ), L"\"" ).c_str ( ) ) )
                Rn++;
        }
    }

    return ( ( bool ) ( Rn == Sv ) );
}

static void __forceinline killAllSv ( const ::nlohmann::json & Cfg, const ::std::vector < ::std::wstring > & Args, const ::std::vector < unsigned long > & Prcs ) noexcept
{
    static long long Tme { };
    static unsigned long long prIt { }, agIt { };
    static void * pPr { };

    for ( const auto & Srv : Cfg [ "Srvs" ] )
    {
        if ( Srv.size ( ) < 3UI8 ||
             Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
             Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
             !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
            continue;

        prIt = { };
        agIt = { };

        for ( const auto & Arg : Args )
        {
            if ( ++agIt && ::std::wcsstr ( ::rmAll ( ::toLwr ( ::rplAll ( Arg, L"  ", L" " ) ), L"\"" ).c_str ( ),
                                           ::rmAll ( ::toLwr ( ::rplAll ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ), L"  ", L" " ) ), L"\"" ).c_str ( ) ) )
            {
                for ( const auto & Prc : Prcs )
                {
                    if ( ++prIt == agIt && ( pPr = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
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

static ::std::vector < unsigned char > __forceinline rdUpFl ( ::std::string Sv, unsigned short Pr, ::std::string Vs, ::std::string Ph, long long To ) noexcept
{ // gets online multibyte file by server, port, http version, path and timeout in seconds
    static ::std::vector < unsigned char > Dv { }; // data vector
    static ::std::size_t Sk { }; // socket
    static ::hostent * pSh { }; // server handle
    static char Db [ 4096UI16 ] { }; // data buffer
    static int Rs { }, It { }; // result, iterator
    static ::timeval Tv { };
    static ::fd_set Fd { };
    static long long Ts { }; // timestamp, seconds

    Dv.clear ( );
    Dv = { };
    if ( !( pSh = ::gethostbyname ( Sv.c_str ( ) ) ) )
        return Dv;

    ::sockaddr_in Sa { }; // socket address
    Sk = ::socket ( 2I8, 1I8, 6I8 );
    Sa.sin_family = 2I8;
    ::std::memcpy ( &Sa.sin_addr, pSh->h_addr_list [ 0I8 ], pSh->h_length );
    Sa.sin_port = ::htons ( Pr );
    Rs = ::connect ( Sk, ( ( ::sockaddr * ) ( &Sa ) ), ( sizeof ( Sa ) ) );
    if ( Rs == -1I8 )
    {
        ::closesocket ( Sk );
        return Dv;
    }

    Rs = ::std::snprintf ( Db, ( ( ( sizeof ( Db ) ) / ( sizeof ( char ) ) ) - 1I8 ), "GET %s HTTP/%s\r\nHost: %s\r\n\r\n", Ph.c_str ( ), Vs.c_str ( ), Sv.c_str ( ) );
    Rs = ::send ( Sk, Db, Rs, { } );
    if ( Rs == -1I8 )
    {
        ::closesocket ( Sk );
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
    return Dv;
}

static ::std::wstring __forceinline rdVs ( const ::nlohmann::json & Cfg ) noexcept // read actual version from steam.inf
{
    if ( Cfg [ "UpdateKeyFile" ].is_discarded ( ) || Cfg [ "UpdateKeyFile" ].empty ( ) || !Cfg [ "UpdateKeyFile" ].is_string ( ) ||
         Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ).empty ( ) || !::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ) ) ) )
        return { };

    ::std::wifstream Fl { };
    Fl.open ( ::toUcd ( Cfg [ "UpdateKeyFile" ].get < ::std::string > ( ) ), 1I8, 64I8 );
    if ( !Fl.is_open ( ) )
    {
        Fl.clear ( );
        return { };
    }

    ::std::wstring Ln { }, Vs { };
    while ( ::std::getline ( Fl, Ln ) )
    {
        if ( ::std::wcsstr ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rplAll ( ::toLwr (
            Ln ), L"  ", L" " ), L"\"" ), L"\t" ), L"\n" ), L"\r" ), L"\v" ), L"\f" ), L" " ).c_str ( ), L"patchversion=" ) )
            Vs = ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rmAll ( ::rplAll ( ::toLwr (
                Ln ), L"  ", L" " ), L"\"" ), L"\t" ), L"\n" ), L"\r" ), L"\v" ), L"\f" ), L" " ), L"patchversion=" );

        Ln.clear ( );
        Ln = { };
    }

    Fl.close ( );
    Fl.clear ( );
    return Vs;
}

static bool __forceinline needUpd ( const ::nlohmann::json & Cfg ) noexcept
{
    if ( Cfg [ "AppId" ].is_discarded ( ) || Cfg [ "AppId" ].empty ( ) || !Cfg [ "AppId" ].is_number_unsigned ( ) )
        return { };

    ::std::string Vrs { ::toMbc ( ::rdVs ( Cfg ) ), };
    if ( Vrs.empty ( ) )
        return { };

    ::std::vector < unsigned char > Data { ::rdUpFl ( "hattrick.go.ro", 80UI8, "1.1",
                                                      ( "/gameUpToDate.Php?appid=" + ( ::std::to_string ( Cfg [ "AppId" ].get < int > ( ) ) ) + "&version=" + Vrs ), 8I8 ), };
    if ( Data.empty ( ) )
        return { };

    if ( Data.back ( ) )
        Data.emplace_back ( 0UI8 );
    if ( Data.back ( ) )
        Data.push_back ( 0UI8 );

    if ( !::std::strstr ( ( ( char * ) ( Data.data ( ) ) ), "up_to_date" ) )
        return { };

    return ( ( bool ) ( !::std::strstr ( ( ( char * ) ( Data.data ( ) ) ), "\"up_to_date\":true" ) ) );
}

int __cdecl wmain ( int, wchar_t **, wchar_t ** ) noexcept
{
    ::WSAData Data { };
    if ( ::WSAStartup ( 514UI16, &Data ) )
    {
        ::WSACleanup ( );
        ::std::wcout << L"WSAStartup Failed" << ::std::endl;
        return 1I8;
    }

    ::std::setlocale ( 2I8, ".utf8" );
    ::SetPriorityClass ( ::GetCurrentProcess ( ), 128UL );
    ::SetProcessPriorityBoost ( ::GetCurrentProcess ( ), { } );

    if ( ::slfPrcRun ( ) )
    {
        ::WSACleanup ( );
        ::std::wcout << L"This Application Is Already Running" << ::std::endl;
        return 1I8;
    }

    if ( !::GetConsoleWindow ( ) )
        ::AllocConsole ( );

    if ( ::GetConsoleWindow ( ) )
        ::SetConsoleTitleW ( L"Auto Gaming Servers" );

    wchar_t Buffer [ 4096I16 ] { };
    ::GetModuleFileNameW ( { }, Buffer, ( ( ( sizeof ( Buffer ) ) / ( sizeof ( wchar_t ) ) ) - 1I8 ) );
    ::std::wstring buffStr { ::trncPth ( Buffer ), };

    ::HKEY__ * pKey { };
    if ( ::RegOpenKeyExW ( ( ( ::HKEY__ * ) ( 2147483649ULL ) ), L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", { }, 983103UL, &pKey ) )
    {
        if ( pKey )
        {
            ::RegCloseKey ( pKey );
            pKey = { };
        }

        ::WSACleanup ( );
        ::std::wcout << L"RegOpenKeyExW Failed" << ::std::endl;
        return 1I8;
    }

    if ( pKey )
    {
        if ( ::std::filesystem::exists ( buffStr + L"0" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_0", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"1" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_1", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"2" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_2", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"3" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_3", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"4" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_4", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"5" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_5", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"6" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_6", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"7" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_7", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"8" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_8", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else if ( ::std::filesystem::exists ( buffStr + L"9" ) )
            ::RegSetValueExW ( pKey, L"AutoSrcds_9", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        else
            ::RegSetValueExW ( pKey, L"AutoSrcds", { }, 1UL, ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );

        ::RegCloseKey ( pKey );
        pKey = { };
    }

    ::std::ifstream File { };
    File.open ( buffStr + L"CFG.INI", 1I8, 64I8 );
    if ( !File.is_open ( ) )
    {
        File.clear ( );
        ::WSACleanup ( );
        ::std::wcout << L"CFG.INI Failed" << ::std::endl;
        return 1I8;
    }

    ::nlohmann::json Cfg { ::nlohmann::json::parse ( File, { }, { }, true ), };
    File.close ( );
    File.clear ( );

    if ( Cfg.is_discarded ( ) || Cfg.empty ( ) || !Cfg.is_object ( ) || Cfg [ "Srvs" ].is_discarded ( ) || Cfg [ "Srvs" ].empty ( ) || !Cfg [ "Srvs" ].is_object ( ) )
    {
        Cfg.clear ( );
        ::WSACleanup ( );
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

    ::std::vector < ::std::wstring > Prms { }; // proc args
    ::std::vector < ::std::wstring > Pths { }; // proc paths
    ::std::vector < unsigned long > Prcs { }; // proc ids
    ::std::wstring Key [ 6I8 ] { { }, { }, { }, { }, { }, { }, }; // match game server port option
    ::std::wstring Port { }; // game server port
    ::std::wstring Args { }; // proc params, raw
    ::std::wstring sExe { }; // steam exe file path
    ::tagLASTINPUTINFO lInf { }; // last user input info
    ::_STARTUPINFOW sInf { }; // proc startup info
    ::_PROCESS_INFORMATION pInf { }; // proc info
    unsigned long Prc { }; // proc id
    long long Tme { }; // time
    void * pPrc { }; // proc hndl
    bool bRun { }; // run the game server

    while ( ::gethostbyname ( "google.com" ) ) // valid internet connection
    {
        ::std::memset ( &lInf, { }, ( sizeof ( lInf ) ) );
        lInf.cbSize = ( sizeof ( lInf ) );

        if ( ::prcByNme ( L"steamcmd.exe", Pths, Prcs ) || ::prcRun ( L"steamcmd.exe" ) ) // steam cmd running, maybe other instances
        {

        }

        else if ( ( ::GetLastInputInfo ( &lInf ) ) &&
                  ( ( ::GetTickCount64 ( ) - lInf.dwTime ) < ( ( !Cfg [ "AwayUpdate" ].is_discarded ( ) &&
                                                                 !Cfg [ "AwayUpdate" ].empty ( ) &&
                                                                 Cfg [ "AwayUpdate" ].is_number_unsigned ( ) ) ?
                                                               ( ::std::max ( Cfg [ "AwayUpdate" ].get < unsigned long > ( ), 30000UL ) ) : ( 180000UL ) ) ) )
        {
            // user is not away, do not disturb
        }

        else if ( ( ::needUpd ( Cfg ) ) || ( !Cfg [ "UpdateMainFile" ].is_discarded ( ) && !Cfg [ "UpdateMainFile" ].empty ( ) && Cfg [ "UpdateMainFile" ].is_string ( ) &&
                                             !Cfg [ "UpdateMainFile" ].get < ::std::string > ( ).empty ( ) &&
                                             ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) &&
                                             !Cfg [ "SteamCMD" ].is_discarded ( ) && !Cfg [ "SteamCMD" ].empty ( ) && Cfg [ "SteamCMD" ].is_object ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Path" ].is_discarded ( ) && !Cfg [ "SteamCMD" ][ "Path" ].empty ( ) && Cfg [ "SteamCMD" ][ "Path" ].is_string ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ).empty ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Args" ].is_discarded ( ) && !Cfg [ "SteamCMD" ][ "Args" ].empty ( ) && Cfg [ "SteamCMD" ][ "Args" ].is_string ( ) &&
                                             !Cfg [ "SteamCMD" ][ "Args" ].get < ::std::string > ( ).empty ( ) &&
                                             ::std::filesystem::exists ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ) ) ) ) // should update the game server
        {
            if ( !Cfg [ "KillSteamApp" ].is_discarded ( ) && !Cfg [ "KillSteamApp" ].empty ( ) && Cfg [ "KillSteamApp" ].is_boolean ( ) && Cfg [ "KillSteamApp" ].get < bool > ( ) )
            {
                sExe = ::stmExe ( );

                if ( !sExe.empty ( ) && ::std::filesystem::exists ( sExe ) &&
                     ( ( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) || ( Prc = ::prcRun ( L"steam.exe" ) ) ) && ( pPrc = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
                {
                    ::TerminateProcess ( pPrc, 1UI8 );
                    ::WaitForSingleObject ( pPrc, 4294967295UL );
                    ::CloseHandle ( pPrc );
                    pPrc = { };

                    ::std::time ( &Tme );
                    ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Killed Steam APP (" << Prc << L")" << ::std::endl << ::std::endl;
                }
            } // kill steam

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
                        ::std::memset ( &sInf, { }, ( sizeof ( sInf ) ) );
                        ::std::memset ( &pInf, { }, ( sizeof ( pInf ) ) );
                        sInf.cb = ( sizeof ( sInf ) );

                        if ( ::CreateProcessW ( ::toUcd ( Cfg [ "SteamCMD" ][ "Path" ].get < ::std::string > ( ) ).c_str ( ),
                                                ( ( wchar_t * ) ( ::toUcd ( Cfg [ "SteamCMD" ][ "Args" ].get < ::std::string > ( ) ).c_str ( ) ) ),
                                                { }, { }, { }, 144UL, { }, { }, &sInf, &pInf ) && pInf.hProcess )
                        {
                            if ( ::std::filesystem::exists ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) ) )
                                ::std::filesystem::remove ( ::toUcd ( Cfg [ "UpdateMainFile" ].get < ::std::string > ( ) ) );

                            ::WaitForSingleObject ( pInf.hProcess, 4294967295UL );
                            ::CloseHandle ( pInf.hProcess );
                            ::CloseHandle ( pInf.hThread );

                            ::std::time ( &Tme );
                            ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched Steam CMD (" << pInf.dwProcessId << L")" << ::std::endl << ::std::endl;
                        }
                    }
                }
            } // should update
        } // should update

        else if ( ::qryPrcs ( Prcs, Prms, Pths ) ) // maybe a game server crashed by now
        {
            for ( const auto & Srv : Cfg [ "Srvs" ] )
            {
                if ( Srv.size ( ) < 3UI8 ||
                     Srv [ "Port" ].is_discarded ( ) || Srv [ "Port" ].empty ( ) || !Srv [ "Port" ].is_string ( ) || Srv [ "Port" ].get < ::std::string > ( ).empty ( ) ||
                     Srv [ "Args" ].is_discarded ( ) || Srv [ "Args" ].empty ( ) || !Srv [ "Args" ].is_string ( ) || Srv [ "Args" ].get < ::std::string > ( ).empty ( ) ||
                     Srv [ "Path" ].is_discarded ( ) || Srv [ "Path" ].empty ( ) || !Srv [ "Path" ].is_string ( ) || Srv [ "Path" ].get < ::std::string > ( ).empty ( ) ||
                     !::std::filesystem::exists ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ) ) )
                    continue; // invalid server

                Port = ::toUcd ( Srv [ "Port" ].get < ::std::string > ( ) ); // got a game server

                Key [ 0I8 ] = L"-port " + Port;
                Key [ 1I8 ] = L"+port " + Port;
                Key [ 2I8 ] = L"-hostport " + Port;
                Key [ 3I8 ] = L"+hostport " + Port;
                Key [ 4I8 ] = L"-host_port " + Port;
                Key [ 5I8 ] = L"+host_port " + Port;

                bRun = true;

                for ( const auto & Prm : Prms )
                {
                    Args = ::rplAll ( ::rmAll ( ::toLwr ( Prm ), L"\"" ), L"  ", L" " );

                    if ( ( ::std::wcsstr ( Args.c_str ( ), L"-console" ) || ::std::wcsstr ( Args.c_str ( ), L"+console" ) ) &&
                         ( ::std::wcsstr ( Args.c_str ( ), L"-game" ) || ::std::wcsstr ( Args.c_str ( ), L"+game" ) ) &&
                         ( ::std::wcsstr ( Args.c_str ( ), Key [ 0I8 ].c_str ( ) ) || ::std::wcsstr ( Args.c_str ( ), Key [ 1I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Key [ 2I8 ].c_str ( ) ) || ::std::wcsstr ( Args.c_str ( ), Key [ 3I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Key [ 4I8 ].c_str ( ) ) || ::std::wcsstr ( Args.c_str ( ), Key [ 5I8 ].c_str ( ) ) ) )
                        bRun = false; // this server is already running
                }

                if ( bRun ) // launch a game server
                {
                    if ( !Cfg [ "KillSteamApp" ].is_discarded ( ) && !Cfg [ "KillSteamApp" ].empty ( ) && Cfg [ "KillSteamApp" ].is_boolean ( ) &&
                         Cfg [ "KillSteamApp" ].get < bool > ( ) )
                    {
                        sExe = ::stmExe ( );

                        if ( !sExe.empty ( ) && ::std::filesystem::exists ( sExe ) &&
                             ( ( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) || ( Prc = ::prcRun ( L"steam.exe" ) ) ) && ( pPrc = ::OpenProcess ( 2097151UL, { }, Prc ) ) )
                        {
                            ::TerminateProcess ( pPrc, 1UI8 );
                            ::WaitForSingleObject ( pPrc, 4294967295UL );
                            ::CloseHandle ( pPrc );
                            pPrc = { };

                            ::std::time ( &Tme );
                            ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Killed Steam APP (" << Prc << L")" << ::std::endl << ::std::endl;
                        }
                    } // kill steam

                    ::std::memset ( &sInf, { }, ( sizeof ( sInf ) ) );
                    ::std::memset ( &pInf, { }, ( sizeof ( pInf ) ) );
                    sInf.cb = ( sizeof ( sInf ) );

                    if ( ::CreateProcessW ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ).c_str ( ),
                                            ( ( wchar_t * ) ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ).c_str ( ) ) ),
                                            { }, { }, { }, 144UL, { }, { }, &sInf, &pInf ) && pInf.hProcess )
                    { // launch server
                        ::CloseHandle ( pInf.hProcess );
                        ::CloseHandle ( pInf.hThread );

                        ::std::time ( &Tme );
                        ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched " << Port << L" (" << pInf.dwProcessId << L")" << ::std::endl << ::std::endl;

                        ::Sleep ( ( !Cfg [ "Skip" ].is_discarded ( ) && !Cfg [ "Skip" ].empty ( ) && Cfg [ "Skip" ].is_number_unsigned ( ) ) ?
                                  ( ::std::max ( Cfg [ "Skip" ].get < unsigned long > ( ), 5000UL ) ) : ( 10000UL ) ); // wait

                        if ( ::qryPrcs ( Prcs, Prms, Pths ) && ::allSvRun ( Cfg, Prms ) && !Cfg [ "RunSteamApp" ].is_discarded ( ) &&
                             !Cfg [ "RunSteamApp" ].empty ( ) && Cfg [ "RunSteamApp" ].is_boolean ( ) && Cfg [ "RunSteamApp" ].get < bool > ( ) )
                        { // if all the game servers are running try launching the steam app
                            sExe = ::stmExe ( );

                            if ( !sExe.empty ( ) && ::std::filesystem::exists ( sExe ) && !( Prc = ::prcByNme ( L"steam.exe", Pths, Prcs ) ) && !( Prc = ::prcRun ( L"steam.exe" ) ) )
                            {
                                ::std::memset ( &sInf, { }, ( sizeof ( sInf ) ) );
                                ::std::memset ( &pInf, { }, ( sizeof ( pInf ) ) );
                                sInf.cb = ( sizeof ( sInf ) );

                                if ( ::CreateProcessW ( sExe.c_str ( ), ( ( wchar_t * ) ( L"/high -high +high" ) ), { }, { }, { }, 144UL, { }, { }, &sInf, &pInf ) && pInf.hProcess )
                                {
                                    ::CloseHandle ( pInf.hProcess );
                                    ::CloseHandle ( pInf.hThread );

                                    ::std::time ( &Tme );
                                    ::std::wcout << ::toUcd ( ::std::ctime ( &Tme ) ) << L"Launched Steam APP (" << pInf.dwProcessId << L")" << ::std::endl << ::std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }

        ::Sleep ( ( !Cfg [ "Scan" ].is_discarded ( ) && !Cfg [ "Scan" ].empty ( ) && Cfg [ "Scan" ].is_number_unsigned ( ) ) ?
                  ( ::std::max ( Cfg [ "Scan" ].get < unsigned long > ( ), 10000UL ) ) : ( 20000UL ) ); // wait
    }

    Cfg.clear ( );
    ::WSACleanup ( );
    return { };
}
