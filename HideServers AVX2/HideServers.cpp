
#include < Windows.h >
#include < Psapi.h >

constexpr int __stdcall e(::HWND__* w, long long) noexcept
{
	wchar_t t[2048]{ };
	unsigned long p{ };
	void* h{ };

	if (w)
	{
		::GetWindowThreadProcessId(w, &p);

		if (p > 0)
		{
			if (h = ::OpenProcess(2097151, { }, p))
			{
				::memset(t, { }, sizeof t);
				{
					::K32GetModuleFileNameExW(h, nullptr, t, ((sizeof t) / 8) - 1);
					{
						if (::wcsstr(t, L"hlds.exe") || ::wcsstr(t, L"srcds.exe") || ::wcsstr(t, L"HLDS.exe") || ::wcsstr(t, L"SRCDS.exe") || ::wcsstr(t, L"HLDS.EXE") || ::wcsstr(t, L"SRCDS.EXE") || ::wcsstr(t, L"Hlds.exe") || ::wcsstr(t, L"Srcds.exe"))
						{
							::ShowWindow(w, { });
						}
					}
				}

				::CloseHandle(h);
			}
		}
	}

	return 1;
}

void main() noexcept
{
	::EnumWindows(::e, { });
}
