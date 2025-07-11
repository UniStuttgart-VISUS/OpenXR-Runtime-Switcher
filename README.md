# OpenXR Runtime Switcher
This utility allows for switching between different OpenXR runtimes on Windows without relying on the vendor-specific user interfaces. The tool tries to detect all installed OpenXR runtimes we know of and presents them in a single interface for switching between them.

When installing the utility using the installer in the [setup project](setup), the installer allows for modifying the access control list of the OpenXR registry key such that users without administrative privileges can switch the OpenXR runtime. This is typically not possible as the key can only be modified by administrators. If you do not install this feature, the tool must run with administrative privileges to work. The modification to the registry ACLs can also be performed from within the tool via the "Berechtigungen anpassen" button. This might be necessary when upgrading or reinstalling OpenXR. An indication for the ACLs being changed is if the programme is running as normal user and the current OpenXR runtime is not indicated as the selected item in the dropdown box (there are runtimes in the box, but none is selected).

## Building
The application is a mostly self-contained Visual C++ 2022 project and downloads the [Windows Implementation Library](https://github.com/microsoft/wil) and [JSON for Modern C++](https://github.com/nlohmann/json) via Nuget. The installer requires the [WiX Toolset](https://www.firegiant.com/wixtoolset/) and the Visual Studio integration for it installed on the development machine.

On the target machine, the latest [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist) must be installed.
