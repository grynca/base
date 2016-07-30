#ifndef FILELISTER_H
#define FILELISTER_H

#include <dirent.h>
#include <string>
#include <vector>

namespace grynca {
    class Path;

    class FileLister
    {
    public:
        // extensions without dot
        FileLister(const Path& dir_path, const fast_vector<std::string>& extensions, bool dive = false);
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
        std::vector<std::string> _extensions;
        std::vector<Pair_> _dirs;
    };
}

#include "FileLister.inl"
#endif //FILELISTER_H