
#include < nlohmann/json.hpp >

#include < iostream >
#include < fstream >

#include < winsock2.h >
#include < psapi.h >
#include < wbemidl.h >
#include < mstask.h >

class qryPrData
{
public:

    ::IWbemLocator* pLc{ };
    ::IWbemServices* pSv{ };
    ::IWbemClassObject* pOb{ };
    ::IEnumWbemClassObject* pEn{ };

    wchar_t* pLn{ };
    wchar_t* pQr{ };
    wchar_t* pRs{ };

    ::tagVARIANT Pr{ };
    ::tagVARIANT Pm{ };
    ::tagVARIANT Ph{ };
};

::std::wstring toLwr(::std::wstring _) noexcept
{
    ::std::transform(_.begin(), _.end(), _.begin(), ::towlower);

    return _;
}

::std::wstring invSl(::std::wstring _) noexcept
{
    if (_.empty())
        return _;

    for (auto& Ch : _)
    {
        if (Ch == L'\\')
            Ch = L'/';

        else if (Ch == L'/')
            Ch = L'\\';
    }

    return _;
}

unsigned long slfPrRun(void) noexcept
{
    static unsigned long Pr[2048I16]{ }, Rs{ }, Slf{ }, It{ };
    static wchar_t slfNm[512I16]{ }, othNm[512I16]{ };
    static void* pPr{ }, * pSlf{ };

    if (!(pSlf = ::GetCurrentProcess()) ||
        !::K32GetModuleFileNameExW(pSlf, { }, slfNm, (((sizeof(slfNm)) / (sizeof(decltype (*slfNm)))) - 1I8)) ||
        !::K32EnumProcesses(Pr, (sizeof(Pr)), &Rs) ||
        !(Slf = ::GetCurrentProcessId()))
        return { };

    for (It = { }; It < ((Rs) / (sizeof(decltype (*Pr)))); It++)
    {
        if ((Pr[It]) && (Pr[It] != Slf) && (pPr = ::OpenProcess(2097151UL, { }, Pr[It])))
        {
            ::K32GetModuleFileNameExW(pPr, { }, othNm, (((sizeof(othNm)) / (sizeof(decltype (*othNm)))) - 1I8));

            ::CloseHandle(pPr);
            pPr = { };

            if (!(::lstrcmpiW(othNm, slfNm)))
                return (Pr[It]);
        }
    }

    return { };
}

unsigned long prRun(::std::wstring _) noexcept
{
    static unsigned long Pr[2048I16]{ }, Rs{ }, Slf{ }, It{ };
    static wchar_t Nm[512I16]{ };
    static void* pPr{ };

    if (!::K32EnumProcesses(Pr, (sizeof(Pr)), &Rs) ||
        !(Slf = ::GetCurrentProcessId()))
        return { };

    for (It = { }; It < ((Rs) / (sizeof(decltype (*Pr)))); It++)
    {
        if ((Pr[It]) && (Pr[It] != Slf) && (pPr = ::OpenProcess(2097151UL, { }, Pr[It])))
        {
            ::K32GetModuleFileNameExW(pPr, { }, Nm, (((sizeof(Nm)) / (sizeof(decltype (*Nm)))) - 1I8));

            ::CloseHandle(pPr);
            pPr = { };

            if (::std::wcsstr(::toLwr(Nm).c_str(), ::toLwr(_).c_str()) ||
                ::std::wcsstr(::toLwr(Nm).c_str(), ::toLwr(::invSl(_)).c_str()))
                return (Pr[It]);
        }
    }

    return { };
}

::std::wstring tcPh(::std::wstring _) noexcept
{
    static ::std::size_t Ps{ };

    if (_.empty() || _.ends_with(L'/') || _.ends_with(L'\\'))
        return _;

    if ((Ps = _.find_last_of(L'/')) != ::std::wstring::npos ||
        (Ps = _.find_last_of(L'\\')) != ::std::wstring::npos)
        _.erase(Ps + 1I8);

    return _;
}

::std::wstring toUc(::std::string _) noexcept
{
    static wchar_t Str[1024I16]{ };

    ::MultiByteToWideChar(65001UI16, { }, _.c_str(), -1I8, Str,
        (((sizeof(Str)) / (sizeof(decltype (*Str)))) - 1I8));

    return Str;
}

::std::string toMb(::std::wstring _) noexcept
{
    static char Str[1024I16]{ };

    ::WideCharToMultiByte(65001UI16, { }, _.c_str(), -1I8, Str,
        (((sizeof(Str)) / (sizeof(decltype (*Str)))) - 1I8), { }, { });

    return Str;
}

::std::wstring smExe(void) noexcept
{
    static ::HKEY__* _{ };
    static wchar_t Str[512I16]{ };
    static unsigned long Sz{ };

    if (!::RegOpenKeyExW(((::HKEY__*)(2147483649ULL)), L"software\\valve\\steam", { }, 983103UL, &_) && _)
    {
        ::std::memset(Str, { }, (sizeof(Str)));

        Sz = (((sizeof(Str)) / (sizeof(decltype (*Str)))) - 1UI8);

        ::RegQueryValueExW(_, L"SteamExe", { }, { }, ((unsigned char*)(Str)), &Sz);

        ::RegCloseKey(_);
        _ = { };

        if (::std::wcslen(Str))
            return ::toLwr(Str);
    }

    else if (_)
    {
        ::RegCloseKey(_);
        _ = { };
    }

    return { };
}

::std::wstring rmAll(::std::wstring Str, ::std::wstring Rm) noexcept
{
    static ::std::size_t Ps{ };

    if (Str.empty() || Rm.empty())
        return Str;

    while ((Ps = Str.find(Rm)) != ::std::wstring::npos)
        Str.erase(Ps, Rm.length());

    return Str;
}

::std::wstring rpAll(::std::wstring Str, ::std::wstring Fr, ::std::wstring To) noexcept
{
    static ::std::size_t Ps{ }, frLn{ }, toLn{ };

    if (Str.empty() || Fr.empty() || To.empty())
        return Str;

    Ps = { };

    frLn = Fr.length();
    toLn = To.length();

    while ((Ps = Str.find(Fr, Ps)) != ::std::wstring::npos)
    {
        Str.replace(Ps, frLn, To);

        Ps += toLn;
    }

    return Str;
}

void qryPrFree(::qryPrData& _) noexcept
{
    if (_.pLc)
    {
        _.pLc->Release();
        _.pLc = { };
    }

    if (_.pSv)
    {
        _.pSv->Release();
        _.pSv = { };
    }

    if (_.pOb)
    {
        _.pOb->Release();
        _.pOb = { };
    }

    if (_.pEn)
    {
        _.pEn->Release();
        _.pEn = { };
    }

    if (_.pLn)
    {
        ::SysFreeString(_.pLn);
        _.pLn = { };
    }

    if (_.pQr)
    {
        ::SysFreeString(_.pQr);
        _.pQr = { };
    }

    if (_.pRs)
    {
        ::SysFreeString(_.pRs);
        _.pRs = { };
    }

    ::VariantClear(&_.Pr);
    _.Pr = { };

    ::VariantClear(&_.Pm);
    _.Pm = { };

    ::VariantClear(&_.Ph);
    _.Ph = { };

    _ = { };
}

bool qryPr(::std::vector < unsigned long >& Pr, ::std::vector < ::std::wstring >& Pm, ::std::vector < ::std::wstring >& Ph) noexcept
{
    static ::qryPrData Var{ };
    static unsigned long _{ };

    Pr.clear();
    Pr = { };

    Pm.clear();
    Pm = { };

    Ph.clear();
    Ph = { };

    if (::CoInitializeEx({ }, { }) ||
        ::CoInitializeSecurity({ }, -1L, { }, { }, { }, 3UL, { }, { }, { }) ||
        ::CoCreateInstance(::CLSID_WbemLocator, { }, 1UL, ::IID_IWbemLocator, (void**)&Var.pLc) ||
        !Var.pLc || !(Var.pRs = ::SysAllocString(L"ROOT\\CIMV2")) ||
        Var.pLc->ConnectServer(Var.pRs, { }, { }, { }, { }, { }, { }, &Var.pSv) ||
        !Var.pSv || !(Var.pLn = ::SysAllocString(L"WQL")) ||
        !(Var.pQr = ::SysAllocString(L"SELECT ProcessId, CommandLine, ExecutablePath FROM Win32_Process")) ||
        Var.pSv->ExecQuery(Var.pLn, Var.pQr, { }, { }, &Var.pEn) || !Var.pEn)
    {
        ::qryPrFree(Var);
        Var = { };

        ::CoUninitialize();

        return { };
    }

    _ = { };

    while (!Var.pEn->Next(-1L, 1UL, &Var.pOb, &_))
    {
        if (Var.pOb)
        {
            Var.pOb->Get(L"ProcessId", { }, &Var.Pr, { }, { });
            Pr.emplace_back(Var.Pr.ulVal);
            ::VariantClear(&Var.Pr);
            Var.Pr = { };

            Var.pOb->Get(L"CommandLine", { }, &Var.Pm, { }, { });
            ::std::wstring pmStr(Var.Pm.bstrVal, ::SysStringLen(Var.Pm.bstrVal));
            Pm.emplace_back(pmStr);
            ::VariantClear(&Var.Pm);
            Var.Pm = { };

            Var.pOb->Get(L"ExecutablePath", { }, &Var.Ph, { }, { });
            ::std::wstring phStr(Var.Ph.bstrVal, ::SysStringLen(Var.Ph.bstrVal));
            Ph.emplace_back(phStr);
            ::VariantClear(&Var.Ph);
            Var.Ph = { };

            Var.pOb->Release();
            Var.pOb = { };
        }

        _ = { };
    }

    ::qryPrFree(Var);
    Var = { };

    ::CoUninitialize();

    return true;
}

unsigned long prByNm(::std::wstring Nm, ::std::vector < ::std::wstring >& Ph, ::std::vector < unsigned long >& Pr) noexcept
{
    static ::std::size_t It{ };

    It = { };

    for (auto& Ph : Ph)
    {
        ++It;

        if (::std::wcsstr(::toLwr(Ph).c_str(), ::toLwr(Nm).c_str()) ||
            ::std::wcsstr(::toLwr(Ph).c_str(), ::toLwr(::invSl(Nm)).c_str()))
            return (Pr[(It - 1UI8)]);
    }

    return { };
}

bool allSvRun(::nlohmann::json& _, ::std::vector < ::std::wstring >& Pm) noexcept
{
    static ::std::size_t Sv{ }, Rn{ };

    Sv = { };
    Rn = { };

    for (auto& Srv : _["Srvs"])
    {
        if (Srv.size() < 3UI8 ||
            Srv["Port"].is_discarded() || Srv["Port"].empty() || !Srv["Port"].is_string() || Srv["Port"].get < ::std::string >().empty() ||
            Srv["Args"].is_discarded() || Srv["Args"].empty() || !Srv["Args"].is_string() || Srv["Args"].get < ::std::string >().empty() ||
            Srv["Path"].is_discarded() || Srv["Path"].empty() || !Srv["Path"].is_string() || Srv["Path"].get < ::std::string >().empty() ||
            !::std::filesystem::exists(::toUc(Srv["Path"].get < ::std::string >())))
            continue;

        Sv++;

        for (auto& Pm : Pm)
        {
            if (::std::wcsstr(::rpAll(::rmAll(::toLwr(Pm), L"\""), L"  ", L" ").c_str(),
                ::rpAll(::rmAll(::toLwr(::toUc(Srv["Args"].get < ::std::string >())), L"\""), L"  ", L" ").c_str()))
                Rn++;
        }
    }

    return ((bool)(Rn == Sv));
}

bool killAllSv(::nlohmann::json& _, ::std::vector < ::std::wstring >& Pm, ::std::vector < unsigned long >& Pr) noexcept
{
    static ::std::size_t prIt{ }, pmIt{ };
    static long long Tm{ };
    static void* pPr{ };
    static bool Er{ }, Rs{ };

    Rs = { };

    for (auto& Sv : _["Srvs"])
    {
        if (Sv.size() < 3UI8 ||
            Sv["Port"].is_discarded() || Sv["Port"].empty() || !Sv["Port"].is_string() || Sv["Port"].get < ::std::string >().empty() ||
            Sv["Args"].is_discarded() || Sv["Args"].empty() || !Sv["Args"].is_string() || Sv["Args"].get < ::std::string >().empty() ||
            Sv["Path"].is_discarded() || Sv["Path"].empty() || !Sv["Path"].is_string() || Sv["Path"].get < ::std::string >().empty() ||
            !::std::filesystem::exists(::toUc(Sv["Path"].get < ::std::string >())))
            continue;

        prIt = { };
        pmIt = { };

        for (auto& Pm : Pm)
        {
            ++pmIt;

            if (::std::wcsstr(::rpAll(::rmAll(::toLwr(Pm), L"\""), L"  ", L" ").c_str(),
                ::rpAll(::rmAll(::toLwr(::toUc(Sv["Args"].get < ::std::string >())), L"\""), L"  ", L" ").c_str()))
            {
                for (auto& Pr : Pr)
                {
                    if ((++prIt == pmIt) && (pPr = ::OpenProcess(2097151UL, { }, Pr)))
                    {
                        Er = !((::TerminateProcess(pPr, { })) && !(::WaitForSingleObject(pPr, 4294967295UL)));

                        ::CloseHandle(pPr);
                        pPr = { };

                        ::std::time(&Tm);

                        if (!Er)
                            ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Killed A Game Server (" << Pr << L")" << ::std::endl << ::std::endl;

                        else
                        {
                            ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Tried To Kill A Game Server (" << Pr << L")" << ::std::endl << ::std::endl;

                            Rs = true;
                        }
                    }
                }
            }
        }
    }

    return ((bool)(!(Rs)));
}

::std::vector < unsigned char > rdUpFl(::std::string Sv, unsigned short Pr, ::std::string Vs, ::std::string Ph, long long To) noexcept
{
    static ::std::vector < unsigned char > Dv{ };
    static ::std::size_t Sk{ };
    static char Db[2048UI16]{ };
    static long long Ts{ };
    static int Rs{ }, It{ };
    static ::fd_set Fd{ };
    static ::timeval Tv{ };
    static ::hostent* pHe{ };
    static ::sockaddr_in Sa{ };
    static ::WSAData _{ };

    ::WSACleanup();
    {
        _ = { };
    }

    Dv.clear();
    {
        Dv = { };
    }

    if (::WSAStartup(514UI16, &_) || !(pHe = ::gethostbyname(Sv.c_str())))
        return Dv;

    Sk = ::socket(2I8, 1I8, 6I8);

    ::std::memset(&Sa, { }, (sizeof(Sa)));

    Sa.sin_family = 2I8;
    Sa.sin_port = ::htons(Pr);

    ::std::memcpy(&Sa.sin_addr, pHe->h_addr_list[0I8], pHe->h_length);

    Rs = ::connect(Sk, ((::sockaddr*)(&Sa)), (sizeof(Sa)));

    if (Rs == -1I8)
    {
        ::closesocket(Sk);
        Sk = { };

        return Dv;
    }

    Rs = ::std::snprintf(Db, (((sizeof(Db)) / (sizeof(decltype (*Db)))) - 1I8),
        "GET %s HTTP/%s\r\nHost: %s\r\n\r\n", Ph.c_str(), Vs.c_str(), Sv.c_str());

    Rs = ::send(Sk, Db, Rs, { });

    if (Rs == -1I8)
    {
        ::closesocket(Sk);
        Sk = { };

        return Dv;
    }

    Ts = ::std::time({ });

    while (true)
    {
        ::Sleep(1UI8);

        Tv.tv_sec = { };
        Tv.tv_usec = 25I8;

        ::std::memset(&Fd, { }, (sizeof(Fd)));

        (&Fd)->fd_count = 1I8;
        (&Fd)->fd_array[0I8] = Sk;

        if (((::select(((int)(Sk + 1UI8)), &Fd, { }, { }, &Tv)) > 0I8) ||
            ((::std::time({ }) - Ts) > To))
            break;
    }

    while (((Rs = ::recv(Sk, Db, (((sizeof(Db)) / (sizeof(decltype (*Db)))) - 1I8), { })) > 0I8))
    {
        for (It = { }; It < Rs; It++)
            Dv.emplace_back(((unsigned char)(Db[It])));
    }

    ::closesocket(Sk);
    Sk = { };

    return Dv;
}

::std::wstring rdVs(::nlohmann::json& _) noexcept
{
    static ::std::wifstream Fl{ };
    static ::std::wstring Ln{ }, Vs{ };

    Vs.clear();
    Vs = { };

    if (_["UpdateKeyFile"].is_discarded() || _["UpdateKeyFile"].empty() || !_["UpdateKeyFile"].is_string() ||
        _["UpdateKeyFile"].get < ::std::string >().empty() || !::std::filesystem::exists(::toUc(_["UpdateKeyFile"].get < ::std::string >())))
        return Vs;

    Fl.open(::toUc(_["UpdateKeyFile"].get < ::std::string >()), 1I8, 64I8);

    if (!Fl.is_open())
    {
        Fl.clear();
        Fl = { };

        return Vs;
    }

    Ln.clear();
    Ln = { };

    while (::std::getline(Fl, Ln))
    {
        if (::std::wcsstr(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::toLwr(
            Ln), L"\""), L"\t"), L"\n"), L"\r"), L"\v"), L"\f"), L" ").c_str(), L"patchversion="))
            Vs = ::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::rmAll(::toLwr(
                Ln), L"\""), L"\t"), L"\n"), L"\r"), L"\v"), L"\f"), L" "), L"patchversion=");

        Ln.clear();
        Ln = { };
    }

    Fl.close();

    Fl.clear();
    Fl = { };

    return Vs;
}

bool ndUp(::nlohmann::json& _) noexcept
{
    static ::std::vector < unsigned char > Var{ };
    static ::std::string Vs{ };

    if (_["AppId"].is_discarded() || _["AppId"].empty() || !_["AppId"].is_number_unsigned())
        return { };

    Vs = ::toMb(::rdVs(_));

    if (Vs.empty())
        return { };

    Var.clear();
    Var = { };

    Var = ::rdUpFl("hattrick.go.ro", 80UI8, "1.1",
        ("/gameUpToDate.Php?appid=" + (::std::to_string(_["AppId"].get < int >())) + "&version=" + Vs), 8I8);

    if (Var.empty())
        return { };

    if (Var.back())
        Var.emplace_back(0UI8);

    if (!(::std::strstr(((char*)(Var.data())), "\"up_to_date\":")))
        return { };

    return ((bool)(!(::std::strstr(((char*)(Var.data())), "\"up_to_date\":true"))));
}

bool rnTk(::std::wstring Tk) noexcept
{
    static ::std::wstring Cd{ };

    static ::_iobuf* pPp{ };

    static wchar_t Ln[1024I16]{ };

    Cd = L"SchTasks /Run /TN " + Tk;

    pPp = ::_wpopen(Cd.c_str(), L"r");

    Cd.clear();

    if (!pPp)
    {
        return { };
    }

    while (!::std::feof(pPp))
    {
        if (::std::fgetws(Ln, (((sizeof(Ln)) / (sizeof(decltype (*Ln)))) - 1I8), pPp))
        {
            Cd += Ln;

            continue;
        }

        break;
    }

    ::_pclose(pPp);

    return ((bool)(::std::wcsstr(::toLwr(Cd).c_str(), L"success")));
}

int __cdecl wmain(void) noexcept
{
    ::WSAData _{ };

    if (::WSAStartup(514UI16, &_))
    {
        ::WSACleanup();
        _ = { };

        ::std::wcout << L"WSAStartup Failed" << ::std::endl;

        return 1I8;
    }

    ::std::setlocale(2I8, ".utf8");

    ::SetPriorityClass(::GetCurrentProcess(), 128UL);
    ::SetProcessPriorityBoost(::GetCurrentProcess(), { });

    if (::slfPrRun())
    {
        ::WSACleanup();
        _ = { };

        ::std::wcout << L"This Application Is Already Running" << ::std::endl;

        return 1I8;
    }

    if (!::GetConsoleWindow())
        ::AllocConsole();

    if (::GetConsoleWindow())
        ::SetConsoleTitleW(L"Auto Gaming Servers");

    wchar_t Str[512I16]{ };

    ::GetModuleFileNameW({ }, Str, (((sizeof(Str)) / (sizeof(decltype (*Str)))) - 1I8));

    ::std::wstring Ph{ ::tcPh(Str), };

    ::HKEY__* pKey{ };

    if (::RegOpenKeyExW(((::HKEY__*)(2147483649ULL)), L"software\\microsoft\\windows\\currentversion\\run", { }, 983103UL, &pKey) ||
        !pKey)
    {
        if (pKey)
        {
            ::RegCloseKey(pKey);
            pKey = { };
        }

        ::WSACleanup();
        _ = { };

        ::std::wcout << L"RegOpenKeyExW Failed" << ::std::endl;

        return 1I8;
    }

    if (pKey)
    {
        if (::std::filesystem::exists(Ph + L"0"))
            ::RegSetValueExW(pKey, L"AutoSrcds_0", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"1"))
            ::RegSetValueExW(pKey, L"AutoSrcds_1", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"2"))
            ::RegSetValueExW(pKey, L"AutoSrcds_2", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"3"))
            ::RegSetValueExW(pKey, L"AutoSrcds_3", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"4"))
            ::RegSetValueExW(pKey, L"AutoSrcds_4", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"5"))
            ::RegSetValueExW(pKey, L"AutoSrcds_5", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"6"))
            ::RegSetValueExW(pKey, L"AutoSrcds_6", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"7"))
            ::RegSetValueExW(pKey, L"AutoSrcds_7", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"8"))
            ::RegSetValueExW(pKey, L"AutoSrcds_8", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else if (::std::filesystem::exists(Ph + L"9"))
            ::RegSetValueExW(pKey, L"AutoSrcds_9", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        else
            ::RegSetValueExW(pKey, L"AutoSrcds", { }, 1UL, ((unsigned char*)(Str)),
                ((sizeof(decltype (*Str))) * (((unsigned long)(::std::wcslen(Str))) + 1UL)));

        ::RegCloseKey(pKey);
        pKey = { };
    }

    ::std::ifstream Fl{ };

    Fl.open(Ph + L"CFG.INI", 1I8, 64I8);

    if (!Fl.is_open())
    {
        Fl.clear();
        Fl = { };

        ::WSACleanup();
        _ = { };

        ::std::wcout << L"CFG.INI Failed" << ::std::endl;

        return 1I8;
    }

    ::nlohmann::json Cfg{ ::nlohmann::json::parse(Fl, { }, { }, true), };

    Fl.close();

    Fl.clear();
    Fl = { };

    if (Cfg.is_discarded() || Cfg.empty() || !Cfg.is_object() || Cfg["Srvs"].is_discarded() || Cfg["Srvs"].empty() || !Cfg["Srvs"].is_object())
    {
        Cfg.clear();
        Cfg = { };

        ::WSACleanup();
        _ = { };

        ::std::wcout << L"JSON Validation Failed" << ::std::endl;

        return 1I8;
    }

    if (!Cfg["Item"].is_discarded() && !Cfg["Item"].empty() && Cfg["Item"].is_string())
        ::SetConsoleTitleW(::toUc(Cfg["Item"].get < ::std::string >()).c_str());

    if (!Cfg["Scan"].is_discarded() && !Cfg["Scan"].empty() && Cfg["Scan"].is_number_unsigned())
        ::std::wcout << L"Scan Interval " << ::std::max(Cfg["Scan"].get < unsigned long >(), 10000UL) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Scan Interval 20000 ms" << ::std::endl;

    if (!Cfg["Skip"].is_discarded() && !Cfg["Skip"].empty() && Cfg["Skip"].is_number_unsigned())
        ::std::wcout << L"Skip Interval " << ::std::max(Cfg["Skip"].get < unsigned long >(), 5000UL) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Skip Interval 10000 ms" << ::std::endl;

    if (!Cfg["AwayUpdate"].is_discarded() && !Cfg["AwayUpdate"].empty() && Cfg["AwayUpdate"].is_number_unsigned())
        ::std::wcout << L"Away Interval " << ::std::max(Cfg["AwayUpdate"].get < unsigned long >(), 30000UL) << L" ms" << ::std::endl;

    else
        ::std::wcout << L"Away Interval 180000 ms" << ::std::endl;

    ::std::wcout << ::std::endl;

    ::std::vector < ::std::wstring > Prm{ };
    ::std::vector < ::std::wstring > Pth{ };
    ::std::vector < unsigned long > Prc{ };

    ::std::wstring Key[6I8]{ { }, { }, { }, { }, { }, { }, };
    ::std::wstring Prt{ };
    ::std::wstring Arg{ };
    ::std::wstring Exe{ };

    ::tagLASTINPUTINFO liIf{ };
    ::_PROCESS_INFORMATION prIf{ };
    ::_STARTUPINFOW suIf{ };

    unsigned long Pr{ };
    long long Tm{ };
    void* pPr{ };
    bool Rn{ }, Er{ }, Up{ };

    while (true)
    {
        ::std::memset(&liIf, { }, (sizeof(liIf)));

        liIf.cbSize = (sizeof(liIf));

        if (!(::gethostbyname("google.com")) || (::prByNm(L"steamcmd.exe", Pth, Prc)) || (::prRun(L"steamcmd.exe")) ||
            ((::GetLastInputInfo(&liIf)) &&
                ((::GetTickCount64() - liIf.dwTime) < ((!Cfg["AwayUpdate"].is_discarded() &&
                    !Cfg["AwayUpdate"].empty() &&
                    Cfg["AwayUpdate"].is_number_unsigned()) ?
                    (::std::max(Cfg["AwayUpdate"].get < unsigned long >(), 30000UL)) : (180000UL)))))
        {

        }

        else if (((Up = ::ndUp(Cfg)) && !Cfg["SteamCMD"].is_discarded() && !Cfg["SteamCMD"].empty() && Cfg["SteamCMD"].is_object() &&
            !Cfg["SteamCMD"]["Path"].is_discarded() && !Cfg["SteamCMD"]["Path"].empty() && Cfg["SteamCMD"]["Path"].is_string() &&
            !Cfg["SteamCMD"]["Path"].get < ::std::string >().empty() &&
            !Cfg["SteamCMD"]["Args"].is_discarded() && !Cfg["SteamCMD"]["Args"].empty() && Cfg["SteamCMD"]["Args"].is_string() &&
            !Cfg["SteamCMD"]["Args"].get < ::std::string >().empty() &&
            ::std::filesystem::exists(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >()))) ||
            (!Cfg["UpdateMainFile"].is_discarded() && !Cfg["UpdateMainFile"].empty() && Cfg["UpdateMainFile"].is_string() &&
                !Cfg["UpdateMainFile"].get < ::std::string >().empty() &&
                ::std::filesystem::exists(::toUc(Cfg["UpdateMainFile"].get < ::std::string >())) &&
                !Cfg["SteamCMD"].is_discarded() && !Cfg["SteamCMD"].empty() && Cfg["SteamCMD"].is_object() &&
                !Cfg["SteamCMD"]["Path"].is_discarded() && !Cfg["SteamCMD"]["Path"].empty() && Cfg["SteamCMD"]["Path"].is_string() &&
                !Cfg["SteamCMD"]["Path"].get < ::std::string >().empty() &&
                !Cfg["SteamCMD"]["Args"].is_discarded() && !Cfg["SteamCMD"]["Args"].empty() && Cfg["SteamCMD"]["Args"].is_string() &&
                !Cfg["SteamCMD"]["Args"].get < ::std::string >().empty() &&
                ::std::filesystem::exists(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >()))))
        {
            if (!Cfg["KillSteamApp"].is_discarded() && !Cfg["KillSteamApp"].empty() && Cfg["KillSteamApp"].is_boolean() && Cfg["KillSteamApp"].get < bool >())
            {
                Exe = ::smExe();

                if (!Exe.empty() && ::std::filesystem::exists(Exe) &&
                    ((Pr = ::prByNm(Exe, Pth, Prc)) || (Pr = ::prRun(Exe))) &&
                    (pPr = ::OpenProcess(2097151UL, { }, Pr)))
                {
                    Er = !((::TerminateProcess(pPr, { })) && !(::WaitForSingleObject(pPr, 4294967295UL)));

                    ::CloseHandle(pPr);
                    pPr = { };

                    ::std::time(&Tm);

                    if (!Er)
                        ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Killed Steam APP (" << Pr << L")" << ::std::endl << ::std::endl;

                    else
                        ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Tried To Kill Steam APP (" << Pr << L")" << ::std::endl << ::std::endl;
                }

                else
                    Er = { };
            }

            else
                Er = { };

            if (!Er && ::std::filesystem::exists(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >())) &&
                (::std::filesystem::exists(::toUc(Cfg["UpdateMainFile"].get < ::std::string >())) || Up) &&
                !::prRun(L"steamcmd.exe") && ::qryPr(Prc, Prm, Pth) && ::killAllSv(Cfg, Prm, Prc) &&
                ::std::filesystem::exists(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >())) &&
                (::std::filesystem::exists(::toUc(Cfg["UpdateMainFile"].get < ::std::string >())) || Up) &&
                !::prByNm(L"steamcmd.exe", Pth, Prc) && !::prRun(L"steamcmd.exe"))
            {
                ::std::memset(&suIf, { }, (sizeof(suIf)));
                ::std::memset(&prIf, { }, (sizeof(prIf)));

                suIf.cb = (sizeof(suIf));

                if (::CreateProcessW(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >()).c_str(),
                    ((wchar_t*)((L"\"" + ::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >()) + L"\" " +
                        ::toUc(Cfg["SteamCMD"]["Args"].get < ::std::string >())).c_str())),
                    { }, { }, { }, 144UL, { }, ::tcPh(::toUc(Cfg["SteamCMD"]["Path"].get < ::std::string >())).c_str(),
                    &suIf, &prIf) && prIf.hProcess)
                {
                    if (::std::filesystem::exists(::toUc(Cfg["UpdateMainFile"].get < ::std::string >())))
                        ::std::filesystem::remove(::toUc(Cfg["UpdateMainFile"].get < ::std::string >()));

                    ::std::time(&Tm);
                    ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Launched Steam CMD (" << prIf.dwProcessId << L")" << ::std::endl << ::std::endl;

                    ::WaitForSingleObject(prIf.hProcess, 4294967295UL);

                    ::CloseHandle(prIf.hProcess);
                    prIf.hProcess = { };

                    ::CloseHandle(prIf.hThread);
                    prIf.hThread = { };

                    ::std::time(&Tm);
                    ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Steam CMD Ended (" << prIf.dwProcessId << L")" << ::std::endl << ::std::endl;
                }
            }
        }

        else if (::qryPr(Prc, Prm, Pth))
        {
            for (auto& Sv : Cfg["Srvs"])
            {
                if (Sv.size() < 3UI8 ||
                    Sv["Port"].is_discarded() || Sv["Port"].empty() || !Sv["Port"].is_string() || Sv["Port"].get < ::std::string >().empty() ||
                    Sv["Args"].is_discarded() || Sv["Args"].empty() || !Sv["Args"].is_string() || Sv["Args"].get < ::std::string >().empty() ||
                    Sv["Path"].is_discarded() || Sv["Path"].empty() || !Sv["Path"].is_string() || Sv["Path"].get < ::std::string >().empty() ||
                    !::std::filesystem::exists(::toUc(Sv["Path"].get < ::std::string >())))
                    continue;

                Prt = ::toUc(Sv["Port"].get < ::std::string >());

                Key[0I8] = L"-port " + Prt;
                Key[1I8] = L"+port " + Prt;

                Key[2I8] = L"-hostport " + Prt;
                Key[3I8] = L"+hostport " + Prt;

                Key[4I8] = L"-host_port " + Prt;
                Key[5I8] = L"+host_port " + Prt;

                Rn = true;

                for (auto& Pm : Prm)
                {
                    Arg = ::rpAll(::rmAll(::toLwr(Pm), L"\""), L"  ", L" ");

                    if ((::std::wcsstr(Arg.c_str(), L"-console") || ::std::wcsstr(Arg.c_str(), L"+console")) &&
                        (::std::wcsstr(Arg.c_str(), L"-game") || ::std::wcsstr(Arg.c_str(), L"+game")) &&
                        (::std::wcsstr(Arg.c_str(), Key[0I8].c_str()) || ::std::wcsstr(Arg.c_str(), Key[1I8].c_str()) ||
                            ::std::wcsstr(Arg.c_str(), Key[2I8].c_str()) || ::std::wcsstr(Arg.c_str(), Key[3I8].c_str()) ||
                            ::std::wcsstr(Arg.c_str(), Key[4I8].c_str()) || ::std::wcsstr(Arg.c_str(), Key[5I8].c_str())))
                        Rn = false;
                }

                if (Rn)
                {
                    if (!Cfg["KillSteamApp"].is_discarded() && !Cfg["KillSteamApp"].empty() && Cfg["KillSteamApp"].is_boolean() &&
                        Cfg["KillSteamApp"].get < bool >())
                    {
                        Exe = ::smExe();

                        if (!Exe.empty() && ::std::filesystem::exists(Exe) && ((Pr = ::prByNm(Exe, Pth, Prc)) || (Pr = ::prRun(Exe))) && (pPr = ::OpenProcess(2097151UL, { }, Pr)))
                        {
                            Er = !((::TerminateProcess(pPr, { })) && !(::WaitForSingleObject(pPr, 4294967295UL)));

                            ::CloseHandle(pPr);
                            pPr = { };

                            ::std::time(&Tm);

                            if (Er)
                                ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Killed Steam APP (" << Pr << L")" << ::std::endl << ::std::endl;

                            else
                                ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Tried To Kill Steam APP (" << Pr << L")" << ::std::endl << ::std::endl;
                        }

                        else
                            Er = { };
                    }

                    else
                        Er = { };

                    if (!Er)
                    {
                        ::std::memset(&suIf, { }, (sizeof(suIf)));
                        ::std::memset(&prIf, { }, (sizeof(prIf)));

                        suIf.cb = (sizeof(suIf));

                        if (::rnTk(Prt) || (::CreateProcessW(::toUc(Sv["Path"].get < ::std::string >()).c_str(),
                            ((wchar_t*)((L"\"" + ::toUc(Sv["Path"].get < ::std::string >()) + L"\" " + ::toUc(Sv["Args"].get < ::std::string >())).c_str())),
                            { }, { }, { }, 144UL, { }, ::tcPh(::toUc(Sv["Path"].get < ::std::string >())).c_str(), &suIf, &prIf) && prIf.hProcess))
                        {
                            if (prIf.hProcess)
                            {
                                ::CloseHandle(prIf.hProcess);
                                {
                                    prIf.hProcess = { };
                                }
                            }

                            if (prIf.hThread)
                            {
                                ::CloseHandle(prIf.hThread);
                                {
                                    prIf.hThread = { };
                                }
                            }

                            ::std::time(&Tm);

                            if (prIf.dwProcessId > 0UL)
                            {
                                ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Launched " << Prt << L" (" << prIf.dwProcessId << L")" << ::std::endl << ::std::endl;
                            }

                            else
                            {
                                ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Launched " << Prt << L" (Task Scheduler)" << ::std::endl << ::std::endl;
                            }

                            ::Sleep((!Cfg["Skip"].is_discarded() && !Cfg["Skip"].empty() && Cfg["Skip"].is_number_unsigned()) ?
                                (::std::max(Cfg["Skip"].get < unsigned long >(), 5000UL)) : (10000UL));

                            if (::qryPr(Prc, Prm, Pth) && ::allSvRun(Cfg, Prm) && !Cfg["RunSteamApp"].is_discarded() &&
                                !Cfg["RunSteamApp"].empty() && Cfg["RunSteamApp"].is_boolean() && Cfg["RunSteamApp"].get < bool >())
                            {
                                Exe = ::smExe();

                                if (!Exe.empty() && ::std::filesystem::exists(Exe) && !::prByNm(Exe, Pth, Prc) && !::prRun(Exe))
                                {
                                    ::std::memset(&suIf, { }, (sizeof(suIf)));
                                    ::std::memset(&prIf, { }, (sizeof(prIf)));

                                    suIf.cb = (sizeof(suIf));

                                    if (::CreateProcessW(Exe.c_str(), ((wchar_t*)((L"\"" + Exe + L"\" /high -high +high").c_str())),
                                        { }, { }, { }, 144UL, { }, ::tcPh(Exe).c_str(), &suIf, &prIf) && prIf.hProcess)
                                    {
                                        ::CloseHandle(prIf.hProcess);
                                        prIf.hProcess = { };

                                        ::CloseHandle(prIf.hThread);
                                        prIf.hThread = { };

                                        ::std::time(&Tm);
                                        ::std::wcout << ::toUc(::std::ctime(&Tm)) << L"Launched Steam APP (" << prIf.dwProcessId << L")" << ::std::endl << ::std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ::Sleep((!Cfg["Scan"].is_discarded() && !Cfg["Scan"].empty() && Cfg["Scan"].is_number_unsigned()) ?
            (::std::max(Cfg["Scan"].get < unsigned long >(), 10000UL)) : (20000UL));
    }

    Cfg.clear();
    Cfg = { };

    ::WSACleanup();
    _ = { };

    return { };
}
