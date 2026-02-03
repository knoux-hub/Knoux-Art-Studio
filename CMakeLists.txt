cmake_minimum_required(VERSION 3.16)
project(KnouxArtStudio VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia OpenGLWidgets)
add_executable(${PROJECT_NAME} WIN32
    src/core/main.cpp src/core/MainWindow.cpp
    src/photo_editor/PhotoEditor.cpp src/video_editor/VideoEditor.cpp
    src/body_editor/BodyEditor.cpp src/face_editor/FaceRetouch.cpp
    src/utils/ImageProcessor.cpp src/utils/ExportManager.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::OpenGLWidgets)
