# Overlay triplet: same as vcpkg's built-in x64-windows-static, but pins the
# MSVC toolset used to build dependencies to v143 (14.44). The default vcpkg
# behavior is to build ports with the NEWEST installed toolset, which here is
# 14.51 (VS2026) -- whose STL removed stdext::checked_array_iterator and so
# cannot compile the pinned fmt 9.1.0. Pinning to 14.44 keeps deps on v143.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_PLATFORM_TOOLSET_VERSION "14.44")
