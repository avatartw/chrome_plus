std::wstring QuoteSpaceIfNeeded(const std::wstring &str)
{
    if (str.find(L' ') == std::wstring::npos)
        return std::move(str);

    std::wstring escaped(L"\"");
    for (auto c : str)
    {
        if (c == L'"')
            escaped += L'"';
        escaped += c;
    }
    escaped += L'"';
    return std::move(escaped);
}

std::wstring JoinArgsString(std::vector<std::wstring> lines, const std::wstring &delimiter)
{
    std::wstring text;
    bool first = true;
    for (auto &line : lines)
    {
        if (!first)
            text += delimiter;
        else
            first = false;
        text += QuoteSpaceIfNeeded(line);
    }
    return text;
}

// 這段代碼應該可以廢棄了……
//bool IsExistsPortable()
//{
//    std::wstring path = GetAppDir() + L"\\portable";
//    if (PathFileExists(path.data()))
//    {
//        return true;
//    }
//    return false;
//}

//bool IsNeedPortable()
//{
//    return true;
//    static bool need_portable = IsExistsPortable();
//    return need_portable;
//}

// 嘗試讀取 ini 檔案
bool IsIniExist()
{
    std::wstring path = GetAppDir() + L"\\chrome++.ini";
    if (PathFileExists(path.data()))
    {
        return true;
    }
    return false;
}

// 如果 ini 存在，從中讀取 CommandLine；如果 ini 不存在，或者存在，但是 CommandLine 為空，則返回空字串
std::wstring GetCrCommandLine()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        TCHAR CommandLineBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"CommandLine", L"", CommandLineBuffer, MAX_PATH, IniPath.c_str());
        return std::wstring(CommandLineBuffer);
    }
    else
    {
        return std::wstring(L"");
    }
}

// 如果 ini 存在，讀取 UserData 並配置，否則使用默認值
std::wstring GetUserDataDir()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        // 修改 Chrome 默認 Data 路徑
        std::wstring path = GetAppDir() + L"\\..\\Data";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniPath.c_str()))
        {
            return GetAppDir() + L"\\..\\Data";
        }

        TCHAR UserDataBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"DataDir", temp, UserDataBuffer, MAX_PATH, IniPath.c_str());

        // 若 ini 中 DataDir 留空，則按照默認情況處理
        if (UserDataBuffer[0] == 0)
        {
            ::PathCanonicalize(UserDataBuffer, path.data());
        }

        std::wstring ExpandedPath = ExpandEnvironmentPath(UserDataBuffer);

        // 替換 %app%
        ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
        std::wstring DataDir;
        DataDir = GetAbsolutePath(ExpandedPath);

        wcscpy(UserDataBuffer, DataDir.c_str());

        return std::wstring(UserDataBuffer);
    }
    else
    {
        std::wstring path = GetAppDir() + L"\\..\\Data";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());
        return temp;
    }
}

// 如果 ini 存在，讀取 DiskCache 並配置，否則使用默認值
std::wstring GetDiskCacheDir()
{
    if (IsIniExist())
    {
        std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
        // 修改 Chrome 默認 Cache 路徑
        std::wstring path = GetAppDir() + L"\\..\\Cache";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());

        if (!PathFileExists(IniPath.c_str()))
        {
            return GetAppDir() + L"\\..\\Cache";
        }

        TCHAR CacheDirBuffer[MAX_PATH];
        ::GetPrivateProfileStringW(L"General", L"CacheDir", temp, CacheDirBuffer, MAX_PATH, IniPath.c_str());

        // 若 ini 中 CacheDir 留空，則按照默認情況處理
        if (CacheDirBuffer[0] == 0)
        {
            ::PathCanonicalize(CacheDirBuffer, path.data());
        }

        std::wstring ExpandedPath = ExpandEnvironmentPath(CacheDirBuffer);

        // 替換 %app%
        ReplaceStringIni(ExpandedPath, L"%app%", GetAppDir());
        std::wstring CacheDir;
        CacheDir = GetAbsolutePath(ExpandedPath);
        wcscpy(CacheDirBuffer, CacheDir.c_str());

        return std::wstring(CacheDirBuffer);
    }
    else
    {
        std::wstring path = GetAppDir() + L"\\..\\Cache";
        TCHAR temp[MAX_PATH];
        ::PathCanonicalize(temp, path.data());
        return temp;
    }
}

// 構造新命令行
std::wstring GetCommand(LPWSTR param)
{
    std::vector<std::wstring> args;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(param, &argc);

    int insert_pos = 0;
    for (int i = 0; i < argc; i++)
    {
        if (wcscmp(argv[i], L"--") == 0)
        {
            break;
        }
        if (wcscmp(argv[i], L"--single-argument") == 0)
        {
            break;
        }
        insert_pos = i;
    }
    for (int i = 0; i < argc; i++)
    {
        // 保留原來參數
        if (i)
            args.push_back(argv[i]);

        // 追加參數
        if (i == insert_pos)
        {
            args.push_back(L"--portable");

            args.push_back(L"--disable-features=RendererCodeIntegrity,FlashDeprecationWarning");

            // 獲取命令行，然後追加參數
            {
                auto cr_command_line = GetCrCommandLine();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"%s", cr_command_line.c_str());
                args.push_back(temp);
            }

            {
                auto userdata = GetUserDataDir();

                wchar_t temp[MAX_PATH];
                wsprintf(temp, L"--user-data-dir=%s", userdata.c_str());
                args.push_back(temp);
            }
            // if (IsNeedPortable())
            {
                auto diskcache = GetDiskCacheDir();
                auto userdata = GetUserDataDir();

                if (diskcache == userdata)
		{}
                else
                {
                    wchar_t temp[MAX_PATH];
                    wsprintf(temp, L"--disk-cache-dir=%s", diskcache.c_str());
                    args.push_back(temp);
                }
            }
        }
    }
    LocalFree(argv);

    return JoinArgsString(args, L" ");
}

void Portable(LPWSTR param)
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);

    std::wstring args = GetCommand(param);

    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;

    sei.lpParameters = args.c_str();
    if (ShellExecuteEx(&sei))
    {
        ExitProcess(0);
    }
}