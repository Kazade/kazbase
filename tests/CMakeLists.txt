find_package(PkgConfig)
pkg_check_modules(UNITTEST unittest++)

include_directories(${UNITTEST_INCLUDE_DIRS})

IF(UNITTEST_FOUND)
    SET(
        TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_regex.cpp
    )
    get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    message("inc_dirs = ${inc_dirs}")

    ADD_EXECUTABLE(kazbase_tests ${TEST_SOURCES})

    find_package(Boost COMPONENTS system date_time regex thread REQUIRED)

    TARGET_LINK_LIBRARIES(kazbase_tests
        kazbase
        ${UNITTEST_LIBRARIES}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${Boost_THREAD_LIBRARY}
    )
ENDIF()
