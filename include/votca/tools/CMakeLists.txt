configure_file(votca_tools_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/votca_tools_config.h)

file(GLOB_RECURSE VOTCA_HEADERS *.h ${CMAKE_CURRENT_BINARY_DIR}/votca_tools_config.h )

install(FILES ${VOTCA_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/votca/tools)
 
