# wifistr

A command-line Windows executable that continuously displays the signal strength of the connected Wi-Fi adapter.  No script engine required, no confusing GUI to navigate.  Just constant, no nonsense feedback while you position your ~~wire coathanger~~ *antenna* for best possible reception.

![Preview](wifistr-demo.gif)

## Requirements
- Windows 10 or higher.
- No external dependencies (uses native Windows API).

## Instructions

1. **Clone the repo:**
    ```powershell
    git clone https://github.com/ljredux/wifistr.git
    ```

2. **Install a GCC compiler and MinGW-w64 (I use [winlibs_mingw](https://github.com/brechtsanders/winlibs_mingw)):**
    ```powershell
    winget install BrechtSanders.WinLibs.POSIX.UCRT
    ```

3. **Compile:**
    ```powershell
    gcc wifistr.c -o wifistr.exe -lwlanapi
    ```
    Alternatively, for a smaller exe:
    ```powershell
    gcc wifistr.c -o wifistr.exe -lwlanapi -s -O2 -ffunction-sections -fdata-sections "-Wl,--gc-sections"
    ```

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.