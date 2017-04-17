#include "FileLister.h"
#include "Path.h"

namespace grynca {
    inline FileLister::FileLister(const DirPath& dir_path, const fast_vector<std::string>& extensions, bool dive /*= false*/)
        : _recursive(dive)
    {
        for (u32 i=0; i<extensions.size(); ++i) {
            extensions_.push_back(extensions[i]);
        }

        // convert extensions to lower case
        for (unsigned int i=0; i<extensions_.size(); i++)
            std::transform(extensions_[i].begin(), extensions_[i].end(), extensions_[i].begin(), ::tolower);

        // open top lvl dir
        DIR* dir = opendir(dir_path.getPath().c_str());
        if (dir)
            dirs_.push_back(Pair_{dir, dir_path.getPath()});
    }

    inline FileLister::~FileLister()
    {
        // close all opened dirs
        for (unsigned int i=0; i<dirs_.size(); i++)
            closedir(dirs_[i].dir);
    }

    inline bool FileLister::nextFile(Path& path_out)
    {
        struct dirent *dirp;
        while (!dirs_.empty())
        {
            while ((dirp = readdir(dirs_.back().dir)) != NULL)
                // read one item from last dir in _dirs
            {
                std::string curr_item_name = dirp->d_name;

                // skip . and ..
                if (curr_item_name == "." || curr_item_name == "..")
                    continue;

                Path p = DirPath(dirs_.back().path) + curr_item_name;

                DIR* dir = opendir(p.getPath().c_str());
                if (dir)
                    // it is a directory
                {
                    if (_recursive)
                        // i care about subdirs
                    {
                        dirs_.push_back(Pair_{dir, p.getPath()});
                    }
                    continue;
                }

                // else-> item is a file
                // test if its extension is among those we are looking for
                std::string extension = p.getExtension();
                fast_vector<std::string>::iterator it;
                it = std::find(extensions_.begin(), extensions_.end(), extension);
                if (it == extensions_.end())
                    // not a file we are looking for
                    continue;

                // it can be read -> return file
                path_out = p;

                return true;
            }
            // close and pop last directory
            closedir(dirs_.back().dir);
            dirs_.pop_back();
        }
        // no more files to check
        return false;
    }
}