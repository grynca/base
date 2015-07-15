cmake_minimum_required(VERSION 3.2)
project(Base)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
    src/base.h
    src/functions/debug.h
    src/functions/meta.h
    src/types/Exception.h
    src/types/Type.h
    src/types/Type.inl
    src/types/Variant.h
    src/types/Variant.inl
    src/types/variant_helper.h
    src/types/VersionedIndex.h
    src/types/containers/fast_vector.h
    src/types/containers/UnsortedVector.h
    src/types/containers/UnsortedVector.inl
    src/types/containers/UnsortedVersionedVector.h
    src/types/containers/UnsortedVersionedVector.inl
    src/types/BlockMeasure.h
    src/types/RefCount.h
    src/types/CommonPtr.h
    src/types/RefCommonPtr.h
    src/types/RefPtr.h
    src/types/Manager.h
    src/functions/string_utils.h
    src/types/Path.h
    src/functions/random.h
    src/types/FileLister.h
    src/types/Config.h
    src/types/OptionsHandler.h
    src/functions/sort_utils.h
    )
add_executable(Base ${SOURCE_FILES})