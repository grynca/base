#include <iostream>
#include <time.h>
#include "../src/base.h"
using namespace std;
using namespace grynca;
#include "MyDomain.h"
#include "test_containers.h"
#include "test_variants.h"
#include "test_type_if.h"
#include "test_types.h"
#include "test_enum.h"


int main(int argc, char** argv) {
    srand(time(NULL));

    MyDomain::init();

    test_types::test();

    test_variants::testVariants();
    test_containers::testStdMap();
    test_containers::testArray();
    test_containers::testTightArray();
    test_containers::testVirtualVector();

    test_type_if::test();

    test_enum::test();

    KEY_TO_CONTINUE();
    return 0;
}
