project (bulletmind)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TARGET_SOURCES
    app.h
    app.c
    asset.h
    asset.c
    command.h
    command.c
    entity.h
    entity.c
    main.c
    scene.h
    scene.c
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND TARGET_LINK_OPTS "/SUBSYSTEM:CONSOLE")
endif()

add_executable(${PROJECT_NAME} ${TARGET_SOURCES})
add_dependencies(${PROJECT_NAME} Bulletmind::Mind)
target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Bulletmind::Mind)

# copy assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
