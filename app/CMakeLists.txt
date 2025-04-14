



find_package(Doxygen)  



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)


doxygen_add_docs( dnfw-doxygen 
  CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  COMMENT "Generating API documentation with Doxygen"
)


find_package(Sphinx REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/conf.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

sphinx_add_docs(sphinx-doc-html
  BUILDER html
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sphinx
  OUTPUT_DIR sphinx-html
  VERBOSE
)
 