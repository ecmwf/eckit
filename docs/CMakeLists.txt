include(ProcessorCount)
ProcessorCount(nproc)
find_package(Doxygen 1.9 REQUIRED)
find_package(Sphinx REQUIRED)

file(GLOB_RECURSE ECKIT_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../src/eckit/**/*.h)
file(GLOB_RECURSE SPHINX_INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)
set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/index.html)
set(SPHINX_INDEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/sphinx/index.html)
set(DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

add_custom_command(
    DEPENDS 
        ${ECKIT_PUBLIC_HEADERS}
        ${SPHINX_INPUT_FILES}
        conf.py
    COMMAND
    sphinx-build -j ${nproc} -W --keep-going ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/docs/eckit
    MAIN_DEPENDENCY ${DOXYFILE}
    COMMENT "Generating documentation"
    OUTPUT ${SPHINX_INDEX_FILE} ${DOXYGEN_INDEX_FILE}
)

add_custom_target(eckit-doc ALL 
    DEPENDS 
        ${SPHINX_INDEX_FILE} 
        ${DOXYGEN_INDEX_FILE}
)
