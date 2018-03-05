
#include <time.h>
#include "base.h"
using namespace grynca;
#include "MyDomain.h"
#include "test_event_handlers.h"
#include "test_containers.h"
#include "test_variants.h"
#include "test_type_if.h"
#include "test_types.h"
#include "test_enum.h"

int main(int argc, char** argv) {
    srand(time(NULL));
    TestBench& tests = TestBenchSton::get();

    MyDomain::init();
    tests.addTestWithoutMeasure<test_types::Test>("test types");
    tests.addTestWithoutMeasure<test_event_handlers::Test>("test event handlers");
    tests.addTestWithoutMeasure<test_event_handlers::TestTyped>("test event handlers typed");
    tests.addTestWithoutMeasure<test_variants::Test>("test variants");
    tests.addTestWithoutMeasure<test_type_if::Test>("test type if");
    tests.addTestWithoutMeasure<test_enum::Test>("test enum");


    u32 n = test_containers::n();
    tests.addTest<test_containers::TestVector>(ssu::format("fast_vector %u", n));
    tests.addTest<test_containers::TestChunkedBuffer>(ssu::format("ChunkedBuffer %u", n));
    tests.addTest<test_containers::TestItems>(ssu::format("Items %u", n));
    tests.addTest<test_containers::TestItems2>(ssu::format("Items2 %u", n));
    tests.addTest<test_containers::TestTightManager>(ssu::format("TightManager %u", n));
    tests.addTest<test_containers::TestHashMap>(ssu::format("HashMap %u", n));
    tests.addTest<test_containers::TestArray>(ssu::format("Array %u", n));
    tests.addTest<test_containers::TestTightArray>(ssu::format("TightArray %u", n));
    tests.addTest<test_containers::TestCompsPool>(ssu::format("CompsPool %u", n));
    tests.addTest<test_containers::TestPolyPool>(ssu::format("PolyPool %u", n));
    tests.addTest<test_containers::TestMask>(ssu::format("Mask %u", n));
    tests.addTest<test_containers::TestVArray>(ssu::format("VArray %u", n));
    // slooow
    tests.addTest<test_containers::TestSortedArray>(ssu::format("SortedArray %u", n));

    tests.runAllTests();

    WAIT_FOR_KEY_ON_WIN();
    return 0;
}
