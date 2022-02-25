cmake_minimum_required(VERSION 3.18.0)

PROJECT(TextureBasedVolumeRendering)

IF (${USE_32BIT})
    ADD_DEFINITIONS("-m32")
    SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-m32")
ENDIF (${USE_32BIT})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(GLUT REQUIRED)
Find_package(OpenGL)

option(BUILD_UTILS off)
add_subdirectory(3rdlibs/glew/build/cmake)
list(APPEND LIBRARIES glew_s)

add_subdirectory(3rdlibs/glm)
list(APPEND LIBRARIES glm::glm)


include_directories(
        ${OPENGL_INCLUDE_DIR}
        3rdlibs/glew/include
        3rdlibs/glm
        3rdlibs/GLlib
)


set(HEADERS
        include/RayCasting.h
        include/TransferFunction.h
        )

set(SRC
        src/main.cpp
        src/RayCasting.cpp
        src/TransferFunction.cpp
        3rdlibs/Jet.cpp
        3rdlibs/GLlib/Shader.cpp
        3rdlibs/GLlib/GLTexture.cpp
        3rdlibs/arcball/ArcBall.cpp
        3rdlibs/GLlib/GL.cpp
        )


add_executable(isorender ${HEADERS} ${SRC})
target_link_libraries(isorender
        ${OPENGL_LIBRARIES}
        GLUT::GLUT
        ${LIBRARIES}
        ${OPENGL_LIBRARIES}
        )


