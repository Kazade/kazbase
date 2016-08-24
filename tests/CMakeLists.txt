find_package(PkgConfig)
pkg_check_modules(UNITTEST unittest++)

include_directories(${UNITTEST_INCLUDE_DIRS})

IF(UNITTEST_FOUND)
    SET(
        TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_regex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unicode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_signals.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ring_buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_file_utils.cpp
    )
    get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    message("inc_dirs = ${inc_dirs}")

    ADD_EXECUTABLE(kazbase_tests ${TEST_SOURCES})

    TARGET_LINK_LIBRARIES(kazbase_tests
        kazbase
        ${UNITTEST_LIBRARIES}
    )
ENDIF()
