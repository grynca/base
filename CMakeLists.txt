cmake_minimum_required(VERSION 3.2)
project(Base)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror")

IF (WIN32)
    # for profiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -gdwarf-2" )
ENDIF()

include_directories(include/)
set(SOURCE_FILES
        include/base.h
        include/functions/debug.h
        include/functions/meta.h
        include/types/Exception.h
        include/types/Type.h
        include/types/Type.inl
        include/types/Type_internal.h
        include/types/Variant.h
        include/types/Variant.inl
        include/types/variant_helper.h
        include/types/Index.h
        include/types/containers/fast_vector.h
        include/types/containers/VVector.h
        include/types/containers/VVector.inl
        include/types/containers/VVector2.h
        include/types/containers/VVector2.inl
        include/types/containers/RoundBuffer.h
        include/types/containers/TightPool.h
        include/types/containers/TightPool.inl
        include/types/containers/Pool.h
        include/types/containers/Pool.inl
        include/types/containers/MultiPool.h
        include/types/containers/MultiPool.inl
        include/types/containers/Array.h
        include/types/containers/Array.inl
        include/types/containers/SharedBuffer.h
        include/types/containers/SharedBuffer.inl
        include/types/containers/HashMap.h
        include/types/containers/HashMap.inl
        include/types/containers/HashSet.h
        include/types/containers/HashSet.inl
        include/types/containers/Items.h
        include/types/BlockMeasure.h
        include/types/RefCount.h
        include/types/CommonPtr.h
        include/types/RefCommonPtr.h
        include/types/RefPtr.h
        include/types/Manager.h
        include/types/Manager.inl
        include/functions/string_utils.h
        include/types/Path.h
        include/types/Path.inl
        include/types/FileLister.h
        include/types/FileLister.inl
        include/types/Config.h
        include/types/Config.inl
        include/types/OptionsHandler.h
        include/types/Singleton.h
        include/types/Singleton.inl
        include/types/Timer.h
        include/types/Timer.inl
        include/functions/random.h
        include/functions/sort_utils.h
        include/functions/sort_utils.inl
        include/functions/string_utils.h
        include/functions/string_utils.inl
        include/types/VariantCaller.h
        include/types/Call.h
        include/functions/common.h
        include/functions/defs.h
        include/functions/undefs.h
        include/types/Mask.h)

set(TEST_FILES
        test/main.cpp
        test/MyDomain.h
        test/test_containers.h
        test/test_variants.h
        test/test_type_if.h
        test/test_enum.h
        test/test_types.h )

add_executable(Base ${SOURCE_FILES} ${TEST_FILES})