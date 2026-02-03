cmake_minimum_required(VERSION 3.16)
project(KnouxArtStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    OpenGL
    OpenGLWidgets
)

# Set Qt6 policies
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Source files
set(SOURCES
    src/core/main.cpp
    src/core/MainWindow.cpp
    src/core/StyleManager.cpp
    src/ui/GlassButton.cpp
    src/ui/GlassPanel.cpp
    src/ui/CyberpunkSplash.cpp
    src/ui/HomeScreen.cpp
    src/ui/SettingsPanel.cpp
    src/ui/NotificationManager.cpp
    src/photo_editor/PhotoEditor.cpp
    src/video_editor/VideoEditor.cpp
    src/ai/AIStudio.cpp
    src/ai/AIFaceDetector.cpp
    src/ai/AIBodyDetector.cpp
    src/body_editor/BodyEditor.cpp
    src/face_editor/FaceRetouch.cpp
    src/makeup/MakeupStudio.cpp
    src/utils/ImageProcessor.cpp
    src/utils/ExportManager.cpp
)

# Header files
set(HEADERS
    src/core/MainWindow.h
    src/core/StyleManager.h
    src/ui/GlassButton.h
    src/ui/GlassPanel.h
    src/ui/CyberpunkSplash.h
    src/ui/HomeScreen.h
    src/ui/SettingsPanel.h
    src/ui/NotificationManager.h
    src/photo_editor/PhotoEditor.h
    src/video_editor/VideoEditor.h
    src/ai/AIStudio.h
    src/ai/AIFaceDetector.h
    src/ai/AIBodyDetector.h
    src/body_editor/BodyEditor.h
    src/face_editor/FaceRetouch.h
    src/makeup/MakeupStudio.h
    src/utils/ImageProcessor.h
    src/utils/ExportManager.h
)

# Resource files
set(RESOURCES
    resources/icons.qrc
    resources/styles.qrc
)

# Create executable
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/photo_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/src/body_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/face_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/makeup
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /MP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Define version macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    KNOUX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    KNOUX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    KNOUX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    KNOUX_VERSION_STRING="${PROJECT_VERSION}"
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install resources
install(DIRECTORY resources/
    DESTINATION share/${PROJECT_NAME}/resources
)

# CPack configuration
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Knoux Studio")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Knoux Art Studio - Professional Photo & Video Editor with AI")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

if(WIN32)
    set(CPACK_GENERATOR NSIS ZIP)
    set(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_HELP_LINK "https://knoux.studio")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://knoux.studio")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR DragNDMG)
    set(CPACK_DMG_VOLUME_NAME ${PROJECT_NAME})
else()
    set(CPACK_GENERATOR DEB RPM TGZ)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6gui6, libqt6widgets6, libqt6multimedia6")
endif()

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "Knoux Art Studio Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Qt6 Version: ${Qt6Core_VERSION}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
