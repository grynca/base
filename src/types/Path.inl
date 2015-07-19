#include <cstddef>
#include <string>
#include <sys/stat.h>
#include <cassert>
#include <fstream>
#include <stdio.h>
#include <dirent.h>
#include "Path.h"
#include "FileLister.h"

namespace grynca {

    inline Path::Path()
     : is_dir_(false)
    {
    }

    inline Path::Path(const std::string& s, bool dir_path)
     : is_dir_(dir_path), path_(s)
    {
        path_ = normalize_(path_, is_dir_);
    }

    inline Path Path::createDirPath(const std::string& s) {
    // static
        return Path(s, true);
    }

    inline Path Path::createFilePath(const std::string& s) {
    // static
        return Path(s, false);
    }

    inline bool Path::createPathDirs() {
        size_t pre=0,
               pos;
        std::string dir;
        int mdret;

        while((pos=path_.find_first_of('/',pre))!=std::string::npos){
            dir=path_.substr(0,pos++);
            pre=pos;
            if(dir.size()==0)
                continue; // if leading / first time is 0 length
#if defined(_MSC_VER)
// MSVC
            mdret = _mkdir(dir.c_str());
#elif defined(_WIN32)
// MinGW
            mdret = mkdir(dir.c_str());
#else
// GCC
			mdret = mkdir(dir.c_str(), 0777);
#endif
            if(mdret==-1 && errno!=EEXIST) {
                return false;
            }
        }
        return true;
    }


    inline bool Path::convertToRelative(const std::string& relative_to, bool is_dir) {
        fast_vector<std::string> absolute_dirs;
        fast_vector<std::string> relative_dirs;
        std::string rel = normalize_(relative_to, is_dir);
        string_utils::tokenize(path_, absolute_dirs, "/");
        string_utils::tokenize(rel, relative_dirs, "/");

        // Get the shortest of the two paths
        int length = absolute_dirs.size() < relative_dirs.size() ? absolute_dirs.size() : relative_dirs.size();

        // Use to determine where in the loop we exited
        int last_common_root = -1;
        int index;
        std::string relative;

        // Find common root
        for (index = 0; index < length; index++)
        {
            if (absolute_dirs[index] == relative_dirs[index])
                last_common_root = index;
            else
                break;
        }
        // If we didn't find a common prefix then throw
        if (last_common_root == -1)
        {
            assert( 0 ); // "Paths do not have a common base"
            relative = "";
        }

        // Add on the ..
        for (index = last_common_root + 1; index < relative_dirs.size(); index++)
        {
            if (relative_dirs[index].size() > 0)
                relative += "../";
        }

        // Add on the folders
        for (index = last_common_root + 1; index < absolute_dirs.size() - 1; index++)
        {
            relative += absolute_dirs[index];
            relative += "/";
        }

        relative += absolute_dirs[absolute_dirs.size() - 1];
        if (path_[path_.size()-1]=='/' && relative[relative.size()-1]!='/')
            relative += "/";

        path_ = relative;
    }

    inline bool Path::exists() {
        struct stat buffer;
        return (stat (path_.c_str(), &buffer) == 0);
    }
    inline bool Path::loadDataFromFile(fast_vector<uint8_t>& data_out) {
        assert(!is_dir_);

        std::ifstream f(path_, std::ios::binary);
        if (!f.is_open())
            return false;

        // seek out file size
        f.seekg(0, std::ios::end);
        size_t fsize = f.tellg();
        f.seekg(0, std::ios::beg);

        data_out.resize(fsize);
        f.read( (char*)&data_out[0], fsize );

        f.close();
        return true;
    }

    inline bool Path::saveDataToFile(const fast_vector<uint8_t>& data) {
        std::ofstream f(path_, std::ios::binary);
        if (!f.is_open())
            return false;

        f.write((const char*)&data[0], data.size());
        f.close();
        return true;
    }

    inline bool Path::loadStringFromFile(std::string& path_out) {
        std::ifstream f(path_);
        if (!f.is_open())
            return false;
        f.seekg(0, std::ios::end);
        path_out.reserve(f.tellg());
        f.seekg(0, std::ios::beg);
        path_out.assign((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
        f.close();
        return true;

    }

    inline bool Path::saveStringToFile(const std::string& str) {
        std::ofstream lf(path_, std::ofstream::app);
        if (!lf.is_open())
            return false;
        lf << str;
        lf.close();
        return true;
    }

    inline bool Path::clearFile() {
        std::ofstream f(path_, std::ofstream::out | std::ofstream::trunc);
        if (!f.is_open())
            return false;
        f.close();
        return true;
    }

    inline bool Path::deleteFile() {
        return remove(path_.c_str()) == 0;
    }

    inline std::string Path::getExtension() {
        size_t dot_pos = path_.find_last_of('.');
        if (dot_pos == std::string::npos)
            return "";

        // dot must be after last slash
        size_t slash_pos = path_.find_last_of('/');
        if (slash_pos != std::string::npos && slash_pos>dot_pos)
            return "";

        // get extension
        std::string ext = path_.substr(dot_pos+1);
        //change to lower case
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    inline void Path::setExtension(const std::string& ext) {
        size_t dot_pos = path_.find_last_of('.');
        if (dot_pos != std::string::npos)
        {
            size_t slash_pos = path_.find_last_of('/');
            if (slash_pos != std::string::npos)
            {
                // dot must be after last slash
                if ( dot_pos>slash_pos )
                    // remove everything from dot to end
                    path_.erase(dot_pos+1);
            }
            else
                path_.erase(dot_pos+1);
        }
        // append new extension
        path_.append(ext);
    }

    inline void Path::removeExtension() {
        setExtension("");
    }

    inline const std::string& Path::getPath() {
        return path_;
    }

    inline std::string Path::getFilename() {
        std::string p = path_;
        if (p[p.size()-1] == '/' || p[p.size()-1] == '\\')
            p.pop_back();
        unsigned int found = p.find_last_of("/\\");
        return p.substr(found+1);
    }

    inline std::string Path::getDirpath() {
        std::string p = path_;
        if (p[p.size()-1] == '/' || p[p.size()-1] == '\\')
            p.pop_back();
        unsigned int found = p.find_last_of("/\\");
        return p.substr(0,found);

    }

    inline bool Path::isDir() {
        return is_dir_;
    }

    inline void Path::listDirs(fast_vector<Path>& dirsOut, bool dive /*= false*/) {
        assert(is_dir_);
        listDirsInner_(path_, dirsOut, dive);
    }

    inline std::string Path::normalize_(const std::string& path, bool is_dir) {
        // replace backslashes with forward slashes
        std::string p = path;
        std::replace(p.begin(), p.end(), '\\', '/');
        p = string_utils::trim(p);
        if (is_dir)  {
            // enforce trailing slash
            if (p.size())
            {
                char last_char = p[p.size()-1];
                if (last_char != '/')
                    p.push_back('/');
            }
        }
        return p;
    }

    inline FileLister Path::listFiles(const fast_vector<std::string>& extensions, bool dive /*= false*/) {
        assert(is_dir_);
        return FileLister(path_, extensions, dive);
    }

    inline void Path::listDirsInner_(const std::string& dir, fast_vector<Path>& dirs_out, bool dive) {
        DIR *d = opendir(dir.c_str());
        DIR *d_inner;
        struct dirent *dirp;
        if(d == NULL)
            return;
        while ( (dirp = readdir(d)) )
        {
            // ignore . and ..
            std::string name(dirp->d_name);
            if (name == "." || name == "..")
                continue;
            d_inner = opendir( (dir + name).c_str() );
            if (d_inner)
            // is directory
            {
                if (dive)
                    // recurse into it and find leafs
                {
                    fast_vector<Path> lower_level_dirs;
                    listDirsInner_(dir+name+"/", lower_level_dirs, dive);
                    if (!lower_level_dirs.size())
                        // no subdirs -> this is leaf dir
                    {
                        dirs_out.push_back(Path::createDirPath(dir+name));
                    }
                    else
                        // append leaf dirs to output vector
                        dirs_out.insert(dirs_out.end(), lower_level_dirs.begin(), lower_level_dirs.end());
                }
                else
                    // dont look for leafs, just add dir
                    dirs_out.push_back(Path::createDirPath(dir+name));
                closedir(d_inner);
            }
        }
        closedir(d);
    }


    inline Path operator+(const Path& p1, const std::string& s) {
        bool s_is_dir = s.back() == '/' || s.back() == '\\';
        return Path(p1.path_ + s, s_is_dir);
    }

    inline std::ostream& operator << (std::ostream& os, Path& p) {
        std::cout << p.path_;
    }
}