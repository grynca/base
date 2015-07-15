#include <iostream>
#include "src/base.h"

using namespace std;
using namespace grynca;


struct MyStuff;

class StuffManager : public Manager<MyStuff>
{
public:
};


class MyStuff : public ManagedItem<StuffManager> {
public:
    MyStuff() {
        std::cout << "Creating my stuff." << std::endl;
        a = rand()%10;
    }
    ~MyStuff() {
        std::cout << "Deleting my stuff." << std::endl;
    }

    void init() {
        a = rand()%10;
    }

    int a;
};

int main(int argc, char** argv) {
    fast_vector<int> fv;
    fv.push_back(1);fv.push_back(2);fv.push_back(3);

    Variant<MyStuff, int > v;

    UnsortedVector<int> uv;
    uv.add(1);
    uv.add(2);
    uv.add(3);

    uv.remove(0);
    uv.remove(2);

    UnsortedVersionedVector<int> uvv;
    uvv.add(1);uvv.add(2);uvv.add(3);


    Manager<MyStuff> mgr;
    mgr.addItem();
    mgr.addItem();
    mgr.addItem();
    mgr.addItem();
    mgr.addItem();

    v.set<int>(10);

    std::cout << string_utils::fromString<int>("10", 5) << std::endl;
    cout << string_utils::trim("   Hello, World!   ") << endl;

    Path p = Path::createDirPath("c:/DEV/gamedev");

    FileLister fl = p.listFiles({"cpp", "h"}, true);
    Path path;
    std::string filename;
    while (fl.nextFile(&path, &filename)) {
        std::cout << "PATH: " << path << ", FILENAME: " << filename << std::endl;
    }

    fast_vector<Path> dirs;
    p.listDirs(dirs, true);
    std::cout << "DIRS:" << std::endl;
    for (size_t i=0; i<dirs.size(); ++i) {
        std::cout << dirs[i] << std::endl;
    }

    Variant<TypesPack<int, bool>> a;
    a.set<int>(1);
    a.set<bool>(false);


    OptionsHandler oh(argc, argv);

    KEY_TO_CONTINUE();
    return 0;
}
