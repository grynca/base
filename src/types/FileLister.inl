#include "FileLister.h"
#include "Path.h"

namespace grynca {
    inline FileLister::FileLister(const Path& dir_path, const fast_vector<std::string>& extensions, bool dive /*= false*/)
        : _recursive(dive)
    {
        for (uint32_t i=0; i<extensions.size(); ++i) {
            _extensions.push_back(extensions[i]);
        }

        // convert extensions to lower case
        for (unsigned int i=0; i<_extensions.size(); i++)
            std::transform(_extensions[i].begin(), _extensions[i].end(), _extensions[i].begin(), ::tolower);

        // open top lvl dir
        std::string dirp = dir_path.getDirpath();
        DIR* dir = opendir(dirp.c_str());
        if (dir)
            _dirs.push_back(Pair_{dir, dirp});
    }

    inline FileLister::~FileLister()
    {
        // close all opened dirs
        for (unsigned int i=0; i<_dirs.size(); i++)
            closedir(_dirs[i].dir);
    }

    inline bool FileLister::nextFile(Path& path_out)
    {
        struct dirent *dirp;
        while (!_dirs.empty())
        {
            while ((dirp = readdir(_dirs.back().dir)) != NULL)
                // read one item from last dir in _dirs
            {
                std::string curr_item_name = dirp->d_name;

                // skip . and ..
                if (curr_item_name == "." || curr_item_name == "..")
                    continue;

                Path p = Path(_dirs.back().path) + curr_item_name;

                DIR* dir = opendir(p.getPath().c_str());
                if (dir)
                    // it is a directory
                {
                    if (_recursive)
                        // i care about subdirs
                    {
                        _dirs.push_back(Pair_{dir, p.getPath()});
                    }
                    continue;
                }

                // else-> item is a file
                // test if its extension is among those we are looking for
                std::string extension = p.getExtension();
                std::vector<std::string>::iterator it;
                it = std::find(_extensions.begin(), _extensions.end(), extension);
                if (it == _extensions.end())
                    // not a file we are looking for
                    continue;

                // it can be read -> return file
                path_out = p;

                return true;
            }
            // close and pop last directory
            closedir(_dirs.back().dir);
            _dirs.pop_back();
        }
        // no more files to check
        return false;
    }
}