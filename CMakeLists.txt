cmake_minimum_required(VERSION 3.2)
project(Base)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

IF (WIN32)
    # for profiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -gdwarf-2" )
ENDIF()

    set(SOURCE_FILES
        src/base.h
        src/functions/debug.h
        src/functions/meta.h
        src/types/Exception.h
        src/types/Type.h
        src/types/Type.inl
        src/types/Type_internal.h
        src/types/Variant.h
        src/types/Variant.inl
        src/types/variant_helper.h
        src/types/Index.h
        src/types/containers/fast_vector.h
        src/types/containers/Array.h
        src/types/containers/Array.inl
        src/types/containers/SharedBuffer.h
        src/types/containers/SharedBuffer.inl
        src/types/containers/SortedSet.h
        src/types/containers/SortedSet.inl
        src/types/BlockMeasure.h
        src/types/RefCount.h
        src/types/CommonPtr.h
        src/types/RefCommonPtr.h
        src/types/RefPtr.h
        src/types/Manager.h
        src/types/Manager.inl
        src/functions/string_utils.h
        src/types/Path.h
        src/types/Path.inl
        src/types/FileLister.h
        src/types/FileLister.inl
        src/types/Config.h
        src/types/Config.inl
        src/types/OptionsHandler.h
        src/types/Singleton.h
        src/types/Singleton.inl
        src/types/Timer.h
        src/types/Timer.inl
        src/functions/random.h
        src/functions/sort_utils.h
        src/functions/sort_utils.inl
        src/functions/string_utils.h
        src/functions/string_utils.inl
        src/types/VariantCaller.h
        src/types/Call.h
        src/types/containers/VVector.h
        src/types/containers/VVector.inl
        src/types/containers/VVector2.h
        src/types/containers/VVector2.inl
        src/types/containers/RoundBuffer.h
        src/types/containers/TightPool.h
        src/types/containers/TightPool.inl
        src/types/containers/Pool.h
        src/types/containers/Pool.inl
        src/functions/common.h
        )

set(TEST_FILES
        test/main.cpp
        test/MyDomain.h
        test/test_containers.h
        test/test_variants.h
        test/test_type_if.h
        test/test_enum.h test/test_types.h )

add_executable(Base ${SOURCE_FILES} ${TEST_FILES})