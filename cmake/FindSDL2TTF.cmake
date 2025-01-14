set(LOCAL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_ttf/x86_64-w64-mingw32)

if(UNIX)
    find_library(SDL2_TTF_LIBRARY NAME SDL2_ttf
            HINTS
            /usr/lib
            /usr/lib/x86_64-linux-gnu
            REQUIRED)
    add_library(SDL2TTF_ INTERFACE)
    target_link_libraries(SDL2TTF_ INTERFACE ${SDL2_TTF_LIBRARY})
else()
    find_library(SDL2_TTF_LIBRARY NAME SDL2_ttf HINTS ${LOCAL_PREFIX}/lib REQUIRED)
    add_library(SDL2TTF_ INTERFACE)
    target_link_libraries(SDL2TTF_ INTERFACE ${SDL2_TTF_LIBRARY})
    target_include_directories(SDL2TTF_ INTERFACE ${LOCAL_PREFIX}/include/SDL2)
    install(DIRECTORY ${LOCAL_PREFIX}/bin/ DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

add_library(SDL2::TTF ALIAS SDL2TTF_)