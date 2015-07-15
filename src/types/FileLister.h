#ifndef FILELISTER_H
#define FILELISTER_H

#include "containers/fast_vector.h"
#include <dirent.h>
#include <string>

namespace grynca {
    class Path;

    class FileLister
    {
    public:
        // extensions without dot
        FileLister(const std::string& dir_path, const fast_vector<std::string>& extensions, bool dive = false);
        ~FileLister();

        // called iteratively
        // both args can be NULL if not needed
        bool nextFile(Path* path_out, std::string* fname_out = NULL);

    protected:

        bool _recursive;
        fast_vector<std::string> _extensions;
        fast_vector< std::pair<DIR *, std::string/*path*/> > _dirs;
    };
}

#include "FileLister.inl"
#endif //FILELISTER_H