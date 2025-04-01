#include <windows.h>
#include <wlanapi.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

int show_cursor(int show) {
    if (printf(show ? "\e[?25h" : "\e[?25l") < 0) {
        return -1;
    }
    return fflush(stdout);
}

// Show cursor upon ctrl+c
void handle_sigint(int sig) {
    (void)sig;
    show_cursor(1);
    exit(0); 
}

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
        return NULL;
    }
    return pConnectInfo;
}

int main()
{
    // Hide cursor, and restore it upon ctrl+c
    show_cursor(0);
    signal(SIGINT, handle_sigint);

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
        PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = get_connection_info(
            hClient,
            &pIfList->InterfaceInfo[connectedInterfaceIndex].InterfaceGuid
        );

        if (pConnectInfo) {
            // Get time
            char *current_time = get_current_time();
            // Get signal quality
            ULONG signalQuality = pConnectInfo->wlanAssociationAttributes.wlanSignalQuality;
            // Get SSID (convert from DOT11_SSID to a readable string)
            char ssidStr[DOT11_SSID_MAX_LENGTH + 1] = {0};
            memcpy(ssidStr, pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID,
                   pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength);

            printf("\r%s %3lu%% (%s)", current_time, signalQuality, ssidStr);
            fflush(stdout);

            WlanFreeMemory(pConnectInfo);
        } else {
            printf("\nLost connection to wireless network interface.\n");
            break; 
        }
        
        Sleep(1000);
    }

    // Cleanup
    WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);

    show_cursor(1);

    return 0;
}