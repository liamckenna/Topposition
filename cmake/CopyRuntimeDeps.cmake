# CopyRuntimeDeps.cmake
#
# Invoked at POST_BUILD time with:
#   -DEXE_PATH=<path to built .exe>
#   -DMINGW_BIN_DIR=<MinGW bin/ directory>
#   -DRELEASE_DIR=<release output directory>
#
# Uses file(GET_RUNTIME_DEPENDENCIES) to walk the full transitive DLL tree,
# copies every resolved dependency into RELEASE_DIR/libs/, and removes any
# DLLs that SDL3's cmake package may have auto-dropped into RELEASE_DIR root.

# Remove any DLLs that vcpkg's applocal.ps1 or SDL3's cmake package may have
# auto-dropped into the release/ root BEFORE resolving dependencies.
# If these stale copies sit next to the .exe they are found by the dependency
# walker and conflict with the canonical copies in MINGW_BIN_DIR.
file(GLOB _STALE_ROOT_DLLS "${RELEASE_DIR}/*.dll")
foreach(_DLL IN LISTS _STALE_ROOT_DLLS)
    file(REMOVE "${_DLL}")
endforeach()

# Recursively resolve all runtime DLL dependencies of the executable.
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES    "${EXE_PATH}"
    RESOLVED_DEPENDENCIES_VAR   RESOLVED_DEPS
    UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
    DIRECTORIES    "${MINGW_BIN_DIR}"
    # Exclude Windows inbox DLLs that must not be redistributed.
    PRE_EXCLUDE_REGEXES
        "api-ms-win-.*"
        "ext-ms-.*"
        "hvsifiletrust\\.dll"
    POST_EXCLUDE_REGEXES
        ".*[/\\\\][Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss][Yy][Ss][Tt][Ee][Mm]32[/\\\\].*"
)

file(MAKE_DIRECTORY "${RELEASE_DIR}/libs")

foreach(DEP IN LISTS RESOLVED_DEPS)
    file(COPY "${DEP}" DESTINATION "${RELEASE_DIR}/libs")
endforeach()

if(UNRESOLVED_DEPS)
    message(WARNING "Unresolved runtime dependencies (not copied):\n  ${UNRESOLVED_DEPS}")
endif()
