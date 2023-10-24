file(REMOVE_RECURSE
  "libsdl_gpu.dll"
  "libsdl_gpu.dll.a"
  "libsdl_gpu.dll.manifest"
  "libsdl_gpu.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/sdl_gpu.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
