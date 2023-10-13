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

// 是否保留最後一個標籤，是則返回 IsOnlyOneTab 為 True，否則返回 False
bool IsKeepLastTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"keep_last_tab", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 是否開啟滑鼠停留在標籤欄時滾輪切換標籤
bool IsWheelTabFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 是否開啟在任何位置按住右鍵時滾輪切換標籤
bool IsWheelTabWhenPressRButtonFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"wheel_tab_when_press_rbutton", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}

// 是否執行雙擊關閉
bool IsDblClkFun()
{
    std::wstring IniPath = GetAppDir() + L"\\chrome++.ini";
    if (::GetPrivateProfileIntW(L"Tabs", L"double_click_close", 1, IniPath.c_str()) == 0)
    {
        return false;
    }

    return true;
}