#include "SystemUtils.h"

#if JUCE_WINDOWS
#include <Windows.h>
#endif //JUCE_WINDOWS

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
    juce::String root = "HKEY_CLASSES_ROOT\\";
    juce::String key = root + "VocalShaper.Project.4";

    bool result = juce::WindowsRegistry::setValue(root + ".vsp4" + "\\", "VocalShaper.Project.4")
        && juce::WindowsRegistry::setValue(key + "\\", "VocalShaper Project")
        && juce::WindowsRegistry::setValue(key + "\\shell\\open\\command\\", appPath + " \"%1\"")
        && juce::WindowsRegistry::setValue(key + "\\DefaultIcon\\", appPath + "," + juce::String{ 0 });

    return result;

#else //JUCE_WINDOWS
    return false;

#endif //JUCE_WINDOWS
}

bool unregProjectFileFromSystem() {
#if JUCE_WINDOWS
    juce::String root = "HKEY_CLASSES_ROOT\\";
    juce::String key = root + "VocalShaper.Project.4";

    bool flagOk = true, flagExists = false;
    if (juce::WindowsRegistry::keyExists(root + ".vsp4")) {
        flagOk = juce::WindowsRegistry::deleteKey(root + ".vsp4") && flagOk;
        flagExists = true;
    }
    if (juce::WindowsRegistry::keyExists(key)) {
        flagOk = juce::WindowsRegistry::deleteKey(key) && flagOk;
        flagExists = true;
    }

    return flagOk && flagExists;

#else //JUCE_WINDOWS
    return false;

#endif //JUCE_WINDOWS
}