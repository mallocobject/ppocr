cmake_minimum_required(VERSION 3.14)

project(ppocr VERSION 0.1 LANGUAGES CXX C)

# Qt相关设置
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PPOCR相关选项
option(WITH_MKL "Compile with MKL/OpenBlas support, default use MKL." ON)
option(WITH_GPU "Compile with GPU/CPU, default use CPU." OFF)
option(WITH_STATIC_LIB "Compile with static/shared library, default use static." ON)
option(WITH_TENSORRT "Compile with TensorRT." OFF)

set(PADDLE_LIB "" CACHE PATH "Location of Paddle libraries")
set(OPENCV_DIR "" CACHE PATH "Location of OpenCV libraries")
set(CUDA_LIB "" CACHE PATH "Location of CUDA libraries")
set(CUDNN_LIB "" CACHE PATH "Location of CUDNN libraries")
set(TENSORRT_DIR "" CACHE PATH "Location of TensorRT libraries")

# 查找Qt库
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 查找OpenCV库
if(NOT DEFINED OPENCV_DIR)
    message(FATAL_ERROR "Please set OPENCV_DIR with -DOPENCV_DIR=/path/opencv")
endif()
find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR}/build NO_DEFAULT_PATH)

# 查找Paddle库
if(NOT DEFINED PADDLE_LIB)
    message(FATAL_ERROR "Please set PADDLE_LIB with -DPADDLE_LIB=/path/paddle/lib")
endif()
include_directories("${PADDLE_LIB}/paddle/include")
link_directories("${PADDLE_LIB}/paddle/lib")

# 项目源文件（Qt + PPOCR）
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)
aux_source_directory(./src PROJECT_SOURCES)

# 设置编译标志
macro(safe_set_static_flag)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endmacro()

if(WITH_MKL)
    add_definitions(-DUSE_MKL)
endif()

if(WIN32)
    add_definitions("/DGOOGLE_GLOG_DLL_DECL=")
    if(WITH_MKL)
        set(FLAG_OPENMP "/openmp")
    endif()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /bigobj /MTd ${FLAG_OPENMP}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /bigobj /MT ${FLAG_OPENMP}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj /MTd ${FLAG_OPENMP}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj /MT ${FLAG_OPENMP}")
    if(WITH_STATIC_LIB)
        safe_set_static_flag()
        add_definitions(-DSTATIC_LIB)
    endif()
else()
    if(WITH_MKL)
        set(FLAG_OPENMP "-fopenmp")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 ${FLAG_OPENMP} -std=c++11")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
endif()

# GPU相关设置
if(WITH_GPU)
    if(NOT DEFINED CUDA_LIB OR "${CUDA_LIB}" STREQUAL "")
        message(FATAL_ERROR "Please set CUDA_LIB with -DCUDA_LIB=/path/cuda/lib64")
    endif()
    if(NOT WIN32)
        if(NOT DEFINED CUDNN_LIB)
            message(FATAL_ERROR "Please set CUDNN_LIB with -DCUDNN_LIB=/path/cudnn/lib64")
        endif()
    endif()
endif()

# 包含Paddle第三方库
include_directories("${PADDLE_LIB}/third_party/install/protobuf/include")
include_directories("${PADDLE_LIB}/third_party/install/glog/include")
include_directories("${PADDLE_LIB}/third_party/install/gflags/include")
include_directories("${PADDLE_LIB}/third_party/install/xxhash/include")
include_directories("${PADDLE_LIB}/third_party/install/zlib/include")
include_directories("${PADDLE_LIB}/third_party/install/onnxruntime/include")
include_directories("${PADDLE_LIB}/third_party/install/paddle2onnx/include")
include_directories("${PADDLE_LIB}/third_party/boost")
include_directories("${PADDLE_LIB}/third_party/eigen3")
include_directories("${CMAKE_SOURCE_DIR}/")

link_directories("${PADDLE_LIB}/third_party/install/zlib/lib")
link_directories("${PADDLE_LIB}/third_party/install/protobuf/lib")
link_directories("${PADDLE_LIB}/third_party/install/glog/lib")
link_directories("${PADDLE_LIB}/third_party/install/gflags/lib")
link_directories("${PADDLE_LIB}/third_party/install/xxhash/lib")
link_directories("${PADDLE_LIB}/third_party/install/onnxruntime/lib")
link_directories("${PADDLE_LIB}/third_party/install/paddle2onnx/lib")

# MKL/OpenBLAS设置
if(WITH_MKL)
    include_directories("${PADDLE_LIB}/third_party/install/mklml/include")
    set(MATH_LIB 
        ${PADDLE_LIB}/third_party/install/mklml/lib/mklml.lib
        ${PADDLE_LIB}/third_party/install/mklml/lib/libiomp5md.lib)
    set(MKLDNN_PATH "${PADDLE_LIB}/third_party/install/onednn")
    if(EXISTS ${MKLDNN_PATH})
        include_directories("${MKLDNN_PATH}/include")
        set(MKLDNN_LIB ${MKLDNN_PATH}/lib/mkldnn.lib)
    endif()
else()
    set(MATH_LIB ${PADDLE_LIB}/third_party/install/openblas/lib/openblas.lib)
endif()

# Paddle推理库
if(WITH_STATIC_LIB)
    set(PADDLE_LIB_DEPS ${PADDLE_LIB}/paddle/lib/paddle_inference.lib)
else()
    set(PADDLE_LIB_DEPS ${PADDLE_LIB}/paddle/lib/paddle_inference.dll)
endif()

# 其他依赖
set(DEPS 
    ${PADDLE_LIB_DEPS}
    ${MATH_LIB}
    ${MKLDNN_LIB}
    glog gflags_static libprotobuf xxhash
    ${OpenCV_LIBS}
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(WIN32)
    set(DEPS ${DEPS} libcmt shlwapi)
endif()

if(WITH_GPU)
    set(DEPS ${DEPS}
        ${CUDA_LIB}/cudart.lib
        ${CUDA_LIB}/cublas.lib
        ${CUDNN_LIB}/cudnn.lib)
endif()

# 定义可执行目标
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ppocr
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(ppocr
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(ppocr PRIVATE ${DEPS})

# 设置macOS和Windows属性
set_target_properties(ppocr PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装目标
install(TARGETS ppocr
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6需要finalize
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ppocr)
endif()

# Windows下MKL相关DLL复制
if(WIN32 AND WITH_MKL)
    add_custom_command(TARGET ppocr POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${PADDLE_LIB}/third_party/install/mklml/lib/mklml.dll 
            $<TARGET_FILE_DIR:ppocr>/mklml.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${PADDLE_LIB}/third_party/install/mklml/lib/libiomp5md.dll 
            $<TARGET_FILE_DIR:ppocr>/libiomp5md.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${PADDLE_LIB}/third_party/install/onednn/lib/mkldnn.dll 
            $<TARGET_FILE_DIR:ppocr>/mkldnn.dll
    )
endif()