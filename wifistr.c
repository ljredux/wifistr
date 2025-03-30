#include <windows.h>
#include <wlanapi.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

char *get_current_time()
{
    static char buffer[9]; // "hh:mm:ss\0"
    time_t now = time(NULL);
    strftime(buffer, sizeof(buffer), "%T", localtime(&now));
    return buffer;
}

PWLAN_CONNECTION_ATTRIBUTES get_connection_info(HANDLE hClient, const GUID *pInterfaceGuid)
{
    PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
    DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
    WLAN_OPCODE_VALUE_TYPE opCode = wlan_opcode_value_type_invalid;

    DWORD dwResult = WlanQueryInterface(
        hClient,
        pInterfaceGuid,
        wlan_intf_opcode_current_connection,
        NULL,
        &connectInfoSize,
        (PVOID *)&pConnectInfo,
        &opCode);

    if (dwResult != ERROR_SUCCESS)
    {
        printf("WlanQueryInterface failed with error: %lu\n", dwResult);
        return NULL;
    }

    return pConnectInfo;
}

int main()
{
    // Opens a handle to the WLAN service
    DWORD dwVersion = 0;
    DWORD dwResult = 0;
    HANDLE hClient = NULL;

    dwResult = WlanOpenHandle(2, NULL, &dwVersion, &hClient);
    if (dwResult != ERROR_SUCCESS)
    {
        printf("WlanOpenHandle failed with error: %lu\n", dwResult);
        return 1;
    }

    // Get all wireless network interfaces
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS)
    {
        printf("WlanEnumInterfaces failed with error: %lu\n", dwResult);
        WlanCloseHandle(hClient, NULL);
        return 1;
    }

    // Get first connected interface
    PWLAN_INTERFACE_INFO pIfInfo = NULL;
    int connectedInterfaceIndex = -1;

    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++)
    {
        pIfInfo = &pIfList->InterfaceInfo[i];
        if (pIfInfo->isState == wlan_interface_state_connected)
        {
            connectedInterfaceIndex = i;
            break;
        }
    }

    if (connectedInterfaceIndex == -1)
    {
        printf("Unable to find a connected wireless network interface.\n");
        WlanFreeMemory(pIfList);
        WlanCloseHandle(hClient, NULL);
        return 1;
    }

    // Info loop
    while (1)
    {
        char *current_time = get_current_time();
        printf("\r%s x%% (SSID)\033[?25l", current_time); // \033[?25l hides the cursor
        fflush(stdout);
        sleep(1);
    }

    // Cleanup
    WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);

    return 0;
}