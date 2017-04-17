#ifndef FILELISTER_H
#define FILELISTER_H

#include <dirent.h>
#include "types/containers/fast_vector.h"

namespace grynca {
    class Path;

    class FileLister
    {
    public:
        // extensions without dot
        FileLister(const DirPath& dir_path, const fast_vector<std::string>& extensions, bool dive = false);
        ~FileLister();

        // called iteratively
        // both args can be NULL if not needed
        bool nextFile(Path& path_out);

    protected:
        struct Pair_ {
            DIR* dir;
            std::string path;
        };

        bool _recursive;
        fast_vector<std::string> extensions_;
        fast_vector<Pair_> dirs_;
    };
}

#include "FileLister.inl"
#endif //FILELISTER_H