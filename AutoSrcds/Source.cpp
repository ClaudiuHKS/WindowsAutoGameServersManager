
#include < nlohmann/json.hpp >

#include < vector >
#include < string_view >
#include < string >
#include < algorithm >
#include < iostream >
#include < fstream >
#include < random >
#include < experimental/string >

#include < winsock2.h >
#include < winsock.h >
#include < mswsock.h >
#include < windows.h >
#include < psapi.h >
#include < comdef.h >
#include < wbemidl.h >
#include < stdio.h >

static bool __forceinline slfPrcRunningAlready ( void ) noexcept
{
    static unsigned long Prcs [ 4096I16 ] { }, Res { }, Slf { }, Itr { };
    static wchar_t slfName [ 4096I16 ] { }, othName [ 4096I16 ] { };
    static void * pPrc { };

    if ( !::K32GetModuleFileNameExW ( ::GetCurrentProcess ( ), { }, slfName, ( ( sizeof ( slfName ) / sizeof ( wchar_t ) ) - 1I8 ) ) ||
         !::K32EnumProcesses ( Prcs, ( sizeof ( Prcs ) ), &Res ) )
    {
        return { };
    }

    for ( Itr = { }, Slf = ::GetCurrentProcessId ( ); Itr < ( Res / ( sizeof ( unsigned long ) ) ); Itr++ )
    {
        if ( ( Prcs [ Itr ] ) && ( Prcs [ Itr ] != Slf ) )
        {
            {
                pPrc = ::OpenProcess ( 2097151UL, { }, Prcs [ Itr ] );
            }

            if ( pPrc )
            {
                {
                    ::K32GetModuleFileNameExW ( pPrc, { }, othName, ( ( sizeof ( slfName ) / sizeof ( wchar_t ) ) - 1I8 ) );
                }

                {
                    ::CloseHandle ( pPrc );

                    {
                        pPrc = { };
                    }
                }

                if ( !::lstrcmpiW ( othName, slfName ) )
                {
                    return true;
                }
            }
        }
    }

    return { };
}

static ::std::wstring __forceinline truncPathToAddFileName ( ::std::wstring Path ) noexcept
{
    static const ::std::wstring Itm { L"\\/", };

    static ::std::size_t Pos { };

    if ( Path.empty ( ) || Path.ends_with ( Itm [ 0I8 ] ) || Path.ends_with ( Itm [ 1I8 ] ) )
    {
        return Path;
    }

    if ( ( Pos = Path.find_last_of ( Itm [ 0I8 ] ) ) != ::std::wstring::npos ||
         ( Pos = Path.find_last_of ( Itm [ 1I8 ] ) ) != ::std::wstring::npos )
    {
        Path.erase ( Pos + 1I8 );
    }

    return Path;
}

static ::std::string __forceinline truncPathToAddFileName ( ::std::string Path ) noexcept
{
    static const ::std::string Itm { "\\/", };

    static ::std::size_t Pos { };

    if ( Path.empty ( ) || Path.ends_with ( Itm [ 0I8 ] ) || Path.ends_with ( Itm [ 1I8 ] ) )
    {
        return Path;
    }

    if ( ( Pos = Path.find_last_of ( Itm [ 0I8 ] ) ) != ::std::string::npos ||
         ( Pos = Path.find_last_of ( Itm [ 1I8 ] ) ) != ::std::string::npos )
    {
        Path.erase ( Pos + 1I8 );
    }

    return Path;
}

static ::std::wstring __forceinline toUcd ( ::std::string Str ) noexcept
{
    static wchar_t Data [ 8192I16 ] { };

    {
        ::MultiByteToWideChar ( 65001UI16,
                                { },
                                Str.c_str ( ),
                                -1I8,
                                Data,
                                ( ( sizeof ( Data ) / sizeof ( wchar_t ) ) - 1I8 ) );
    }

    return Data;
}

static ::std::string __forceinline toMbc ( ::std::wstring Str ) noexcept
{
    static char Data [ 8192I16 ] { };

    {
        ::WideCharToMultiByte ( 65001UI16,
                                { },
                                Str.c_str ( ),
                                -1I8,
                                Data,
                                ( ( sizeof ( Data ) / sizeof ( char ) ) - 1I8 ),
                                { },
                                { } );
    }

    return Data;
}

static ::std::wstring __forceinline toLwr ( ::std::wstring Str ) noexcept
{
    {
        ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::towlower );
    }

    return Str;
}

static ::std::string __forceinline toLwr ( ::std::string Str ) noexcept
{
    {
        ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::tolower );
    }

    return Str;
}

static ::std::wstring __forceinline toUpr ( ::std::wstring Str ) noexcept
{
    {
        ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::towupper );
    }

    return Str;
}

static ::std::string __forceinline toUpr ( ::std::string Str ) noexcept
{
    {
        ::std::transform ( Str.begin ( ), Str.end ( ), Str.begin ( ), ::toupper );
    }

    return Str;
}

static ::std::wstring __forceinline rplAll ( ::std::wstring Str, ::std::wstring Fr, ::std::wstring To ) noexcept
{
    if ( Str.empty ( ) || Fr.empty ( ) || To.empty ( ) )
    {
        return Str;
    }

    ::std::size_t Pos { };

    ::std::size_t frLen { Fr.length ( ), };
    ::std::size_t toLen { To.length ( ), };

    while ( ( Pos = Str.find ( Fr, Pos ) ) != ::std::wstring::npos )
    {
        {
            Str.replace ( Pos, frLen, To );
        }

        Pos += toLen;
    }

    return Str;
}

static ::std::string __forceinline rplAll ( ::std::string Str, ::std::string Fr, ::std::string To ) noexcept
{
    if ( Str.empty ( ) || Fr.empty ( ) || To.empty ( ) )
    {
        return Str;
    }

    ::std::size_t Pos { };

    ::std::size_t frLen { Fr.length ( ), };
    ::std::size_t toLen { To.length ( ), };

    while ( ( Pos = Str.find ( Fr, Pos ) ) != ::std::wstring::npos )
    {
        {
            Str.replace ( Pos, frLen, To );
        }

        Pos += toLen;
    }

    return Str;
}

class qryPrcsData
{
public:

    __forceinline qryPrcsData ( void ) noexcept
    {
        {
            pLoc = { };
            pSvc = { };
            pObj = { };
            pEnm = { };
        }

        {
            pLng = { };
            pQry = { };
            pSrv = { };
        }

        {
            Prc = { };
            Prm = { };
            Pth = { };
        }
    }

    __forceinline ~qryPrcsData ( void ) noexcept
    {
        freeAll ( );
    }

public:

    void __forceinline constexpr freeAll ( void ) noexcept
    {
        {
            if ( pLoc ) pLoc->Release ( ), pLoc = { };
            if ( pSvc ) pSvc->Release ( ), pSvc = { };
            if ( pObj ) pObj->Release ( ), pObj = { };
            if ( pEnm ) pEnm->Release ( ), pEnm = { };
        }

        {
            if ( pLng ) ::SysFreeString ( pLng ), pLng = { };
            if ( pQry ) ::SysFreeString ( pQry ), pQry = { };
            if ( pSrv ) ::SysFreeString ( pSrv ), pSrv = { };
        }

        {
            Prc = { };
            Prm = { };
            Pth = { };
        }
    }

public:

    ::IWbemLocator * pLoc { };
    ::IWbemServices * pSvc { };
    ::IWbemClassObject * pObj { };
    ::IEnumWbemClassObject * pEnm { };

public:

    wchar_t * pLng { };
    wchar_t * pQry { };
    wchar_t * pSrv { };

public:

    ::tagVARIANT Prc { };
    ::tagVARIANT Prm { };
    ::tagVARIANT Pth { };
};

static bool __forceinline qryPrcs ( ::std::vector < unsigned long > & Prcs,
                                    ::std::vector < ::std::wstring > & Prms,
                                    ::std::vector < ::std::wstring > & Pths ) noexcept
{
    ::qryPrcsData Data { };

    {
        Prcs.clear ( );
        Prms.clear ( );
        Pths.clear ( );
    }

    if ( ::CoInitializeEx ( { }, { } ) ||
         ::CoInitializeSecurity ( { }, -1L, { }, { }, { }, 3UL, { }, { }, { } ) ||
         ::CoCreateInstance ( ::CLSID_WbemLocator, { }, 1UL, ::IID_IWbemLocator, ( void ** ) &Data.pLoc ) ||
         !Data.pLoc ||
         !( Data.pSrv = ::SysAllocString ( L"ROOT\\CIMV2" ) ) ||
         Data.pLoc->ConnectServer ( Data.pSrv, { }, { }, { }, { }, { }, { }, &Data.pSvc ) ||
         !Data.pSvc ||
         !( Data.pLng = ::SysAllocString ( L"WQL" ) ) ||
         !( Data.pQry = ::SysAllocString ( L"SELECT ProcessId, CommandLine, ExecutablePath FROM Win32_Process" ) ) ||
         Data.pSvc->ExecQuery ( Data.pLng, Data.pQry, { }, { }, &Data.pEnm ) ||
         !Data.pEnm )
    {
        {
            Data.freeAll ( );
        }

        {
            ::CoUninitialize ( );
        }

        return { };
    }

    unsigned long _ { };

    while ( !Data.pEnm->Next ( -1L, 1UL, &Data.pObj, &_ ) )
    {
        if ( Data.pObj )
        {
            Data.pObj->Get ( L"ProcessId", { }, &Data.Prc, { }, { } );
            Data.pObj->Get ( L"CommandLine", { }, &Data.Prm, { }, { } );
            Data.pObj->Get ( L"ExecutablePath", { }, &Data.Pth, { }, { } );

            {
                {
                    Prcs.emplace_back ( Data.Prc.ulVal );
                }

                {
                    ::std::wstring prmStr ( Data.Prm.bstrVal, ::SysStringLen ( Data.Prm.bstrVal ) );

                    {
                        Prms.emplace_back ( prmStr );
                    }

                    ::std::wstring pthStr ( Data.Pth.bstrVal, ::SysStringLen ( Data.Pth.bstrVal ) );

                    {
                        Pths.emplace_back ( pthStr );
                    }
                }
            }

            {
                {
                    Data.pObj->Release ( );
                }

                Data.pObj = { };
            }
        }

        {
            _ = { };
        }
    }

    {
        Data.freeAll ( );
    }

    {
        ::CoUninitialize ( );
    }

    return true;
}

int __cdecl wmain ( int, wchar_t **, wchar_t ** ) noexcept
{
    ::WSAData Data { };

    if ( ::WSAStartup ( 514UI16, &Data ) )
    {
        {
            ::WSACleanup ( );
        }

        return 1I8;
    }

    {
        ::std::setlocale ( 2I8, ".utf8" );
    }

    {
        ::SetPriorityClass ( ::GetCurrentProcess ( ), 128UL );
    }

    {
        ::SetProcessPriorityBoost ( ::GetCurrentProcess ( ), { } );
    }

    if ( ::slfPrcRunningAlready ( ) || ::FindWindowW ( { }, L"Auto Gaming Servers" ) )
    {
        {
            ::WSACleanup ( );
        }

        return 1I8;
    }

    if ( !::GetConsoleWindow ( ) )
    {
        ::AllocConsole ( );
    }

    if ( ::GetConsoleWindow ( ) )
    {
        ::SetConsoleTitleW ( L"Auto Gaming Servers" );
    }

    wchar_t Buffer [ 4096I16 ] { };

    {
        ::GetModuleFileNameW ( { }, Buffer, ( ( sizeof ( Buffer ) / sizeof ( wchar_t ) ) - 1I8 ) );
    }

    ::std::wstring buffStr { };

    {
        buffStr = ::truncPathToAddFileName ( Buffer );
    }

    ::HKEY__ * pKey { };

    {
        ::RegOpenKeyExW ( ( ( ::HKEY__ * ) ( 2147483649ULL ) ), L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", { }, 983103UL, &pKey );
    }

    if ( pKey )
    {
        {
            ::RegSetValueExW ( pKey,
                               L"AutoSrcds",
                               { },
                               1UL,
                               ( ( unsigned char * ) ( Buffer ) ),
                               ( ( sizeof ( wchar_t ) ) * ( ( ( unsigned long ) ( ::std::wcslen ( Buffer ) ) ) + 1UL ) ) );
        }

        {
            ::RegCloseKey ( pKey );

            {
                pKey = { };
            }
        }
    }

    ::std::ifstream File { };

    {
        File.open ( buffStr + L"CFG.INI", 1I8, 64I8 );

        if ( !File.is_open ( ) )
        {
            {
                File.clear ( );
            }

            {
                ::WSACleanup ( );
            }

            return 1I8;
        }
    }

    ::nlohmann::json Cfg { };

    {
        Cfg = ::nlohmann::json::parse ( File, { }, { }, true );

        {
            File.close ( );
            File.clear ( );
        }

        if ( Cfg.is_discarded ( ) || Cfg.empty ( ) || !Cfg.is_object ( ) )
        {
            {
                Cfg.clear ( );
            }

            {
                ::WSACleanup ( );
            }

            return 1I8;
        }

        if ( Cfg [ "Srvs" ].is_discarded ( ) || Cfg [ "Srvs" ].empty ( ) || !Cfg [ "Srvs" ].is_object ( ) )
        {
            {
                Cfg.clear ( );
            }

            {
                ::WSACleanup ( );
            }

            return 1I8;
        }
    }

    if ( !Cfg [ "Scan" ].is_discarded ( ) && !Cfg [ "Scan" ].empty ( ) && Cfg [ "Scan" ].is_number_unsigned ( ) )
    {
        ::std::wcout << L"Scan Interval " << Cfg [ "Scan" ].get < unsigned long > ( ) << L" ms" << ::std::endl;
    }

    else
    {
        ::std::wcout << L"Scan Interval 20000 ms" << ::std::endl;
    }

    if ( !Cfg [ "Skip" ].is_discarded ( ) && !Cfg [ "Skip" ].empty ( ) && Cfg [ "Skip" ].is_number_unsigned ( ) )
    {
        ::std::wcout << L"Skip Interval " << Cfg [ "Skip" ].get < unsigned long > ( ) << L" ms" << ::std::endl << ::std::endl;
    }

    else
    {
        ::std::wcout << L"Skip Interval 10000 ms" << ::std::endl << ::std::endl;
    }

    ::std::vector < unsigned long > Prcs { };

    ::std::vector < ::std::wstring > Prms { };
    ::std::vector < ::std::wstring > Pths { };

    ::std::wstring Keys [ 6I8 ] { { }, { }, { }, { }, { }, { }, };

    ::std::wstring Port { };
    ::std::wstring Args { };

    bool bRun { };

    while ( ::gethostbyname ( "google.com" ) )
    {
        if ( ::qryPrcs ( Prcs, Prms, Pths ) )
        {
            for ( const auto & Srv : Cfg [ "Srvs" ] )
            {
                {
                    Port = ::toUcd ( Srv [ "Port" ].get < ::std::string > ( ) );
                }

                {
                    Keys [ 0I8 ] = L"-port " + Port;
                    Keys [ 1I8 ] = L"+port " + Port;
                }

                {
                    Keys [ 2I8 ] = L"-hostport " + Port;
                    Keys [ 3I8 ] = L"+hostport " + Port;
                }

                {
                    Keys [ 4I8 ] = L"-host_port " + Port;
                    Keys [ 5I8 ] = L"+host_port " + Port;
                }

                {
                    bRun = true;
                }

                for ( const auto & Prm : Prms )
                {
                    {
                        Args = ::rplAll ( ::toLwr ( Prm ), L"  ", L" " );
                    }

                    if ( ( ::std::wcsstr ( Args.c_str ( ), L"-console" ) ||
                           ::std::wcsstr ( Args.c_str ( ), L"+console" ) ) &&
                         ( ::std::wcsstr ( Args.c_str ( ), L"-game" ) ||
                           ::std::wcsstr ( Args.c_str ( ), L"+game" ) ) &&
                         ( ::std::wcsstr ( Args.c_str ( ), Keys [ 0I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Keys [ 1I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Keys [ 2I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Keys [ 3I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Keys [ 4I8 ].c_str ( ) ) ||
                           ::std::wcsstr ( Args.c_str ( ), Keys [ 5I8 ].c_str ( ) ) ) )
                    {
                        {
                            bRun = false;
                        }
                    }
                }

                if ( bRun )
                {
                    ::_STARTUPINFOW runInfo { };
                    ::_PROCESS_INFORMATION prcInfo { };

                    {
                        runInfo.cb = ( sizeof ( runInfo ) );
                    }

                    if ( ::CreateProcessW ( ::toUcd ( Srv [ "Path" ].get < ::std::string > ( ) ).c_str ( ),
                                            ( ( wchar_t * ) ( ::toUcd ( Srv [ "Args" ].get < ::std::string > ( ) ).c_str ( ) ) ),
                                            { },
                                            { },
                                            { },
                                            144UL,
                                            { },
                                            { },
                                            &runInfo,
                                            &prcInfo ) )
                    {
                        {
                            ::CloseHandle ( prcInfo.hProcess );
                            ::CloseHandle ( prcInfo.hThread );
                        }

                        {
                            ::std::time_t Time { };

                            {
                                ::std::time ( &Time );
                            }

                            {
                                ::std::wcout << ::toUcd ( ::std::ctime ( &Time ) ) << L"Launched " << Port << ::std::endl << ::std::endl;
                            }
                        }

                        {
                            ::Sleep ( ( !Cfg [ "Skip" ].is_discarded ( ) && !Cfg [ "Skip" ].empty ( ) && Cfg [ "Skip" ].is_number_unsigned ( ) ) ?
                                      ( Cfg [ "Skip" ].get < unsigned long > ( ) ) : ( 10000UL ) );
                        }

                        {
                            bRun = { };
                        }
                    }
                }
            }
        }

        {
            ::Sleep ( ( !Cfg [ "Scan" ].is_discarded ( ) && !Cfg [ "Scan" ].empty ( ) && Cfg [ "Scan" ].is_number_unsigned ( ) ) ?
                      ( Cfg [ "Scan" ].get < unsigned long > ( ) ) : ( 20000UL ) );
        }
    }

    {
        Cfg.clear ( );
    }

    {
        ::WSACleanup ( );
    }

    return { };
}
