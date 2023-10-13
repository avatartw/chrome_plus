#include <windows.h>
#include <stdio.h>
#include <psapi.h>

HMODULE hInstance;

#define MAGIC_CODE 0x1603ABD9

#include "MinHook.h"
#include "version.h"

#include "hijack.h"
#include "utils.h"
#include "patch.h"
#include "config.h"
#include "TabBookmark.h"
#include "portable.h"
#include "PakPatch.h"
#include "appid.h"
#include "green.h"

typedef int (*Startup)();
Startup ExeMain = NULL;

void ChromePlus()
{
    // 快捷方式
    SetAppId();

    // 便攜化補丁
    MakeGreen();

    // 標籤頁，書籤，地址欄增強
    TabBookmark();

    // 給pak檔案打補丁
    PakPatch();
}

void ChromePlusCommand(LPWSTR param)
{
    if (!wcsstr(param, L"--portable"))
    {
        Portable(param);
    }
    else
    {
        ChromePlus();
    }
}

int Loader()
{
    // 硬補丁
    MakePatch();

    // 只關注主介面
    LPWSTR param = GetCommandLineW();
    // DebugLog(L"param %s", param);
    if (!wcsstr(param, L"-type="))
    {
        ChromePlusCommand(param);
    }

    // 返回到主程序
    return ExeMain();
}

void InstallLoader()
{
    // 獲取程序入口點
    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &mi, sizeof(MODULEINFO));
    PBYTE entry = (PBYTE)mi.EntryPoint;

    // 入口點跳轉到Loader
    MH_STATUS status = MH_CreateHook(entry, Loader, (LPVOID *)&ExeMain);
    if (status == MH_OK)
    {
        MH_EnableHook(entry);
    }
    else
    {
        DebugLog(L"MH_CreateHook InstallLoader failed:%d", status);
    }
}
#define EXTERNC extern "C"

//
EXTERNC __declspec(dllexport) void portable()
{
}

EXTERNC BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        hInstance = hModule;

        // 保持系統dll原有功能
        LoadSysDll(hModule);

        // 初始化HOOK庫成功以後安裝載入器
        MH_STATUS status = MH_Initialize();
        if (status == MH_OK)
        {
            InstallLoader();
        }
        else
        {
            DebugLog(L"MH_Initialize failed:%d", status);
        }
    }
    return TRUE;
}
