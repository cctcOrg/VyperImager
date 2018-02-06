link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(vyper 
    main.cc
    appwin.cc 
    appwin.h
    appbook.cc
    appbook.h
    )

target_link_libraries(vyper
    ${GTKMM_LIBRARIES}  )
