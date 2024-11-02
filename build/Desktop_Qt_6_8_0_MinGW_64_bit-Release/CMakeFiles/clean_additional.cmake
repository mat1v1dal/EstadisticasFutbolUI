# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\EstadisticasFutbol_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\EstadisticasFutbol_autogen.dir\\ParseCache.txt"
  "EstadisticasFutbol_autogen"
  )
endif()
