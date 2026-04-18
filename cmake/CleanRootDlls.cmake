# CleanRootDlls.cmake
# Removes any *.dll files from the root release/ folder that were auto-copied
# there by SDL3's cmake package config.  All DLLs must live only in
# release/libs/ so the launcher bat can put that single directory on PATH.

file(GLOB ROOT_DLLS "${RELEASE_DIR}/*.dll")
foreach(DLL IN LISTS ROOT_DLLS)
    file(REMOVE "${DLL}")
endforeach()
