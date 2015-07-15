#include "FileLister.h"
#include "Path.h"

namespace grynca {
    inline FileLister::FileLister(const std::string& dir_path, const fast_vector<std::string>& extensions, bool dive /*= false*/)
        : _recursive(dive), _extensions(extensions)
    {
        // convert extensions to lower case
        for (unsigned int i=0; i<_extensions.size(); i++)
            std::transform(_extensions[i].begin(), _extensions[i].end(), _extensions[i].begin(), ::tolower);

        // open top lvl dir
        DIR* dir = opendir(dir_path.c_str());
        if (dir)
            _dirs.push_back(std::make_pair(dir, dir_path) );
    }

    inline FileLister::~FileLister()
    {
        // close all opened dirs
        for (unsigned int i=0; i<_dirs.size(); i++)
            closedir(_dirs[i].first);
    }

    inline bool FileLister::nextFile(Path* path_out, std::string* fname_out /*= NULL*/)
    {
        struct dirent *dirp;
        while (_dirs.size())
        {
            while ((dirp = readdir(_dirs.back().first)) != NULL)
                // read one item from last dir in _dirs
            {
                std::string curr_item_name = dirp->d_name;

                // skip . and ..
                if (curr_item_name == "." || curr_item_name == "..")
                    continue;

                Path p = Path::createDirPath(_dirs.back().second) + curr_item_name;

                DIR* dir = opendir(p.getPath().c_str());
                if (dir)
                    // it is a directory
                {
                    if (_recursive)
                        // i care about subdirs
                    {
                        _dirs.push_back(std::make_pair(dir, p.getPath()) );
                    }
                    continue;
                }

                // else-> item is a file
                // test if its extension is among those we are looking for
                std::string extension = p.getExtension();
                fast_vector<std::string>::iterator it;
                it = std::find(_extensions.begin(), _extensions.end(), extension);
                if (it == _extensions.end())
                    // not a file we are looking for
                    continue;

                // it can be read -> return file
                if (path_out)
                    *path_out = p;
                if (fname_out)
                    *fname_out = curr_item_name;

                return true;
            }
            // close and pop last directory
            closedir(_dirs.back().first);
            _dirs.pop_back();
        }
        // no more files to check
        return false;
    }
}