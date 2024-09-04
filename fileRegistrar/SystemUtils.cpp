#include "SystemUtils.h"

#if JUCE_WINDOWS
#include <Windows.h>
#include <ShlObj.h>
#endif //JUCE_WINDOWS

#define REG_RROJ_ROOT "HKEY_CLASSES_ROOT\\"
#define REG_PROJ_MIME "VocalShaper.Project.4"
#define REG_PROJ_EXT ".vsp4"
#define REG_PROJ_DESCRIPTION "VocalShaper Project Version 4"

bool checkAdmin() {
#if JUCE_WINDOWS
    UINT PID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
    if (hProcess == NULL) {
        return false;
    }

    HANDLE hToken = NULL;
    if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) == FALSE) {
        CloseHandle(hProcess);
        return false;
    }

    SID_IDENTIFIER_AUTHORITY Authority = {};
    Authority.Value[5] = 5;

    PSID psidAdmin = NULL;
    if (AllocateAndInitializeSid(&Authority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdmin) == FALSE) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return false;
    }

    DWORD dwCount = 0;
    GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwCount);
    TOKEN_GROUPS* pTokenGroups = (TOKEN_GROUPS*)new BYTE[dwCount];
    GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwCount, &dwCount);

    DWORD dwGroupCount = pTokenGroups->GroupCount;
    bool isAdmin = false;
    for (DWORD i = 0; i < dwGroupCount; i++) {
        if (EqualSid(psidAdmin, pTokenGroups->Groups[i].Sid)) {
            DWORD dwAttributes = pTokenGroups->Groups[i].Attributes;
            isAdmin = (dwAttributes & SE_GROUP_USE_FOR_DENY_ONLY) != SE_GROUP_USE_FOR_DENY_ONLY;
            break;
        }
    }

    delete[] pTokenGroups;
    FreeSid(psidAdmin);
    CloseHandle(hToken);
    CloseHandle(hProcess);

    return isAdmin;

#elif JUCE_LINUX
    return geteuid() == 0;

#else //JUCE_WINDOWS
    return false;

#endif //JUCE_WINDOWS
}

bool regProjectFileInSystem(const juce::String& appPath) {
#if JUCE_WINDOWS
    bool result = juce::WindowsRegistry::setValue(REG_RROJ_ROOT REG_PROJ_EXT "\\", REG_PROJ_MIME)
        && juce::WindowsRegistry::setValue(REG_RROJ_ROOT REG_PROJ_MIME "\\", REG_PROJ_DESCRIPTION)
        && juce::WindowsRegistry::setValue(REG_RROJ_ROOT REG_PROJ_MIME "\\shell\\open\\command\\", appPath + " \"%1\"")
        && juce::WindowsRegistry::setValue(REG_RROJ_ROOT REG_PROJ_MIME "\\DefaultIcon\\", appPath + "," + juce::String{ 1 });

    return result;

#else //JUCE_WINDOWS
    return false;

#endif //JUCE_WINDOWS
}

bool unregProjectFileFromSystem() {
#if JUCE_WINDOWS
    bool flagOk = true, flagExists = false;
    if (juce::WindowsRegistry::keyExists(REG_RROJ_ROOT REG_PROJ_EXT)) {
        flagOk = juce::WindowsRegistry::deleteKey(REG_RROJ_ROOT REG_PROJ_EXT) && flagOk;
        flagExists = true;
    }
    if (juce::WindowsRegistry::keyExists(REG_RROJ_ROOT REG_PROJ_MIME)) {
        flagOk = juce::WindowsRegistry::deleteKey(REG_RROJ_ROOT REG_PROJ_MIME) && flagOk;
        flagExists = true;
    }

    return flagOk && flagExists;

#else //JUCE_WINDOWS
    return false;

#endif //JUCE_WINDOWS
}

void refreshSystemIconCache() {
#if JUCE_WINDOWS
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

#endif //JUCE_WINDOWS
}
