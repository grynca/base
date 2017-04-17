cmake_minimum_required(VERSION 3.2)
project(Base)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -ffast-math" )

IF (CMAKE_BUILD_TYPE STREQUAL "Release")
    #disable profiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNPROFILE -Wall -Werror -Wno-strict-aliasing" )
ENDIF()

IF (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    IF (WIN32)
        # for profiling
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -gdwarf-2" )
    ENDIF()
ENDIF()

option(USE_SDL2 "Use SDL2" ON)
IF (USE_SDL2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SDL2=1" )
    IF (WIN32)
        # TODO:
    ELSE()
        set(LIBS "-lGLEW -lGL -lGLU -lSDL2")
    ENDIF()
ENDIF()

include_directories(include/)

set(SOURCE_FILES
        include/base_cfg.h
        include/base.h
        include/functions/sdlinclude.h
        include/functions/debug.h
        include/functions/profiling.h
        include/functions/profiling.inl
        include/functions/meta.h
        include/functions/random.h
        include/functions/sort_utils.h
        include/functions/sort_utils.inl
        include/functions/ssu.h
        include/functions/ssu.inl
        include/functions/common.h
        include/functions/defs.h
        include/functions/undefs.h
        include/functions/emsc_utils.h
        include/types/Exception.h
        include/types/Type.h
        include/types/Type.inl
        include/types/Type_internal.h
        include/types/Variant.h
        include/types/Variant.inl
        include/types/variant_helper.h
        include/types/Index.h
        include/types/Mask.h
        include/types/Mask.inl
        include/types/EventsHandler.h
        include/types/EventHandlers.inl
        include/types/containers/fast_vector.h
        include/types/containers/VVector.h
        include/types/containers/VVector.inl
        include/types/containers/VVector2.h
        include/types/containers/VVector2.inl
        include/types/containers/RoundBuffer.h
        include/types/containers/ChunkedBuffer.h
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
        include/types/containers/Items2.h
        include/types/Bits.h
        include/types/Bits.inl
        include/types/TestBench.h
        include/types/TestBench.inl
        include/types/SimpleFont.h
        include/types/SimpleFont.inl
        include/types/BlockMeasure.h
        include/types/RefCount.h
        include/types/CommonPtr.h
        include/types/RefCommonPtr.h
        include/types/RefPtr.h
        include/types/Manager.h
        include/types/Manager.inl
        include/types/ObjFunc.h
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
        include/types/VariantCaller.h
        include/types/Call.h
        include/types/BitScanner.h
        include/types/BitScanner.inl
        include/types/EventsReciever.h
        include/3rdp/ustring.h)

set(TEST_FILES
        test/main.cpp
        test/MyDomain.h
        test/test_containers.h
        test/test_variants.h
        test/test_type_if.h
        test/test_enum.h
        test/test_types.h
        test/test_event_handlers.h
        )

add_executable(Base ${SOURCE_FILES} ${TEST_FILES})
target_link_libraries(Base ${LIBS})