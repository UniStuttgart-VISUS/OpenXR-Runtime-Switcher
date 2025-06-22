# OpenXR Runtime Switcher
This utility allows for switching between different OpenXR runtimes on Windows without relying on the vendor-specific user interfaces. The tool tries to detect all installed OpenXR runtimes we know of and presents them in a single interface for switching between them.

When installing the utility using the installer in the [setup project](setup), the installer will create a scheduled task running with highest privileges that allows users to switch runtimes without elevating.

