#find_package(Doxygen)
#
#if(NOT DOXYGEN_FOUND)
#  return()
#endif()
#
#
#set(DOXYGEN_ALIASES "")
#list(APPEND DOXYGEN_ALIASES "rst=\\verbatim embed:rst:")
#list(APPEND DOXYGEN_ALIASES "endrst=\\endverbatim")
#list(APPEND DOXYGEN_ALIASES "swc{1}= @defgroup \\1_group \\1 ^^ @brief Elements that are part of the implementation of the \\1 @{")
#list(APPEND DOXYGEN_ALIASES "endswc=@}")
#list(APPEND DOXYGEN_ALIASES "implements{1}=Implements : **\\1**")
#
#
## just output XML since this is what Breathe consumes
#set(DOXYGEN_GENERATE_HTML YES)
#set(DOXYGEN_GENERATE_XML YES)
#
## don't generate documentation from implementation details
#set(DOXYGEN_EXCLUDE_PATTERNS "*/detail/*")
#
## print paths relative to include directory
#set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR})
#set(DOXYGEN_EXTRACT_PRIVATE YES)
#set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
#set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
#set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
#set(DOXYGEN_INLINE_SOURCES YES)
#set(DOXYGEN_HAVE_DOT YES)
#set(DOXYGEN_COLLABORATION_GRAPH YES)
#set(DOXYGEN_UML_LOOK YES)
#set(DOXYGEN_DOT_UML_DETAILS YES)
#set(DOXYGEN_DOT_IMAGE_FORMAT svg)
#
## If one adds a struct or class to a group and this option is enabled, then also
## any nested class or struct is added to the same group. By default this option
## is disabled and one has to add nested compounds explicitly via \ingroup.
## The default value is: NO.
#set(DOXYGEN_GROUP_NESTED_COMPOUNDS YES)
#
#set(DOXYGEN_INPUT_FOLDERS ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/doc/dox ${PROJECT_SOURCE_DIR}/tests)
#
#
#
## The PREDEFINED tag can be used to specify one or more macro names that are
## defined before the preprocessor is started (similar to the -D option of e.g.
## gcc). The argument of the tag is a list of macros of the form: name or
## name=definition (no spaces). If the definition and the "=" are omitted, "=1"
## is assumed. To prevent a macro definition from being undefined via #undef or
## recursively expanded use the := operator instead of the = operator.
## This tag requires that the tag ENABLE_PREPROCESSING is set to YES.
#
##set(DOXYGEN_PREDEFINED "CONCAT1(x,y,z)=x ## y ## z \\ \n CONCAT2(x,y,z)=CONCAT1(x,y,z) \\ \n TEST(x)=void CONCAT2(TEST_GROUP, __, x)(void)")
#
#set(DOXYGEN_PREDEFINED "CONCAT1(pref,x,y,z)=pref ## x ## y ## z\" \\ \n \"CONCAT2(pref, x,y,z)=CONCAT1(pref,x,y,z)\" \\ \n \"TEST(x,y)=void CONCAT2(TEST_, x, _, y)(void)")
#set(DOXYGEN_MACRO_EXPANSION YES)
#set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
#
## If the MACRO_EXPANSION and EXPAND_ONLY_PREDEF tags are set to YES then this
## tag can be used to specify a list of macro names that should be expanded. The
## macro definition that is found in the sources will be used. Use the PREDEFINED
## tag if you want to use a different macro definition that overrules the
## definition found in the source code.
## This tag requires that the tag ENABLE_PREPROCESSING is set to YES.
#
#set(DOXYGEN_EXPAND_AS_DEFINED, TEST_GROUP)
#
#
#
## Note: use doxygen_add_docs(doxygen-doc ALL ...) if you want your documentation
## to be created by default each time you build. Without the keyword you need to
## explicitly invoke building of the 'doc' target.
#doxygen_add_docs(doxygen-doc  ${DOXYGEN_INPUT_FOLDERS} COMMENT
#                 "Generating API documentation with Doxygen")
#
##configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxyfile.in
##               ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
##doxygen_add_docs(doxygen-doc CONFIG_FILE  ${CMAKE_CURRENT_BINARY_DIR}/doxyfile  COMMENT
##                 "Generating API documentation with Doxygen")
#

find_package(Doxygen)  



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)


doxygen_add_docs( dnfw-doxygen 
  CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  COMMENT "Generating API documentation with Doxygen"
)


find_package(Sphinx REQUIRED)

sphinx_add_docs(sphinx-doc-html
  BUILDER html
  SOURCE_DIR ${PROJECT_SOURCE_DIR}
  CONFIG_DIR ${PROJECT_SOURCE_DIR}/doc/sphinx
  OUTPUT_DIR sphinx-html
  VERBOSE
)


#sphinx_add_docs(sphinx-doc-pdf
#  BUILDER simplepdf 
#  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
#  CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sphinx
#  OUTPUT_DIR sphinx-pdf
#  #ROOT_DOC 'dartnetfw/sss/dnfw.sss'
#
#  VERBOSE
#)


#sphinx_add_docs(sphinx-doc-pdf
#  BUILDER simplepdf 
#  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
#  CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sphinx
#  OUTPUT_DIR sphinx-pdf
#)

#add_custom_target(
#    sphinx-doc-epub ALL
#    COMMAND ${SPHINX_EXECUTABLE} -b epub -c ${CMAKE_CURRENT_BINARY_DIR}
#            ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#    COMMENT "Generating API documentation with Sphinx (EPUB)"
#    VERBATIM)


add_dependencies(sphinx-doc-html dnfw-doxygen)
#add_dependencies(sphinx-doc-pdf dnfw-doxygen)
#add_dependencies(sphinx-doc-epub doxygen-doc)
