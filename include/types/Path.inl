#include "Path.h"
#include "FileLister.h"
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <fstream>

namespace grynca {

    inline Path::Path()
    {
    }

    inline Path::Path(const char* path)
     : path_(path)
    {
    }

    inline Path::Path(const ustring& path)
     : path_(path)
    {
    }

    inline bool Path::createPathDirs() {
        size_t pre=0,
               pos;
        ustring dir;
        int mdret;

        while((pos=path_.find('/',pre))!=ustring::npos){
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


    inline bool Path::convertToRelative(const ustring& relative_to) {
        fast_vector<ustring> absolute_dirs;
        fast_vector<ustring> relative_dirs;
        ustring rel = normalize_(relative_to);
        ssu::tokenize(path_, absolute_dirs, U"/");
        ssu::tokenize(rel, relative_dirs, U"/");

        // Get the shortest of the two paths
        u32 length = absolute_dirs.size() < relative_dirs.size() ? absolute_dirs.size() : relative_dirs.size();

        // Use to determine where in the loop we exited
        int last_common_root = -1;
        u32 index;
        ustring relative;

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
            ASSERT_M(false, "Paths do not have a common base");
            relative = "";
        }

        // Add on the ..
        for (index = u32(last_common_root + 1); index < relative_dirs.size(); index++)
        {
            if (relative_dirs[index].size() > 0)
                relative += "../";
        }

        // Add on the folders
        for (index = u32(last_common_root + 1); index < absolute_dirs.size() - 1; index++)
        {
            relative += absolute_dirs[index];
            relative += "/";
        }

        relative += absolute_dirs[absolute_dirs.size() - 1];
        if (path_[path_.size()-1]=='/' && relative[relative.size()-1]!='/')
            relative += "/";

        path_ = relative;
        return true;
    }

    inline bool Path::exists()const {
        struct stat buffer;
        return (stat (path_.c_str(), &buffer) == 0);
    }
    inline bool Path::loadDataFromFile(fast_vector<u8>& data_out) {
        std::ifstream f(path_.c_str(), std::ios::binary);
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

    inline bool Path::saveDataToFile(const fast_vector<u8>& data) {
        std::ofstream f(path_.c_str(), std::ios::binary);
        if (!f.is_open())
            return false;

        f.write((const char*)&data[0], data.size());
        f.close();
        return true;
    }

    inline bool Path::loadTextContent(ustring& str_out) {
        std::ifstream fin = std::ifstream(path_.c_str(), std::ios::binary);
        if (!fin.is_open())
            return false;
        fin >> str_out;
        fin.close();
        return true;
    }

    inline bool Path::saveTextContent(const ustring& str) {
        std::ofstream fout( path_.c_str() , std::ios::binary | std::ofstream::trunc );
        if (!fout.is_open())
            return false;

        // prepend byte order mark
        fout << ustring().cpp_str(true);
        fout << str;
        fout.close();
        return true;
    }

    inline bool Path::appendTextContent(const ustring& str) {
        // TODO: check if file is already utf8 ?
        std::ofstream fout( path_.c_str() , std::ios::binary | std::ofstream::app );
        if (!fout.is_open())
            return false;
        fout << str;
        fout.close();
        return true;
    }

    inline bool Path::clearFile() {
        std::ofstream f(path_.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!f.is_open())
            return false;
        f.close();
        return true;
    }

    inline bool Path::deleteFile() {
        return remove(path_.c_str()) == 0;
    }

    inline ustring Path::getExtension() {
        size_t dot_pos = path_.find(U'.');
        if (dot_pos == ustring::npos)
            return "";

        // dot must be after last slash
        size_t slash_pos = path_.find(U'/');
        if (slash_pos != ustring::npos && slash_pos>dot_pos)
            return "";

        // get extension
        ustring ext = path_.substr(dot_pos+1);
        //change to lower case
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    inline void Path::setExtension(const ustring& ext) {
        size_t dot_pos = path_.find(U'.');
        if (dot_pos != ustring::npos)
        {
            size_t slash_pos = path_.find(U'/');
            if (slash_pos != ustring::npos)
            {
                // dot must be after last slash
                if ( dot_pos>slash_pos )
                    // remove everything from dot to end
                    path_.erase(dot_pos, path_.size()-dot_pos);
            }
            else
                path_.erase(dot_pos);
        }
        // append new extension
        if (!ext.empty()) {
            path_.push_back(U'.');
            path_.append(ext);
        }
    }

    inline void Path::removeExtension() {
        setExtension("");
    }

    inline const ustring& Path::getPath()const {
        return path_;
    }

    inline ustring Path::getFilename()const {
        ustring p = path_;
        if (p[p.size()-1] == '/' || p[p.size()-1] == '\\')
            p.pop_back();
        size_t found = p.find_last_of(U"/\\");
        if (found == ustring::npos)
            return p;
        return p.substr(found+1);
    }

    inline ustring Path::getFilenameWOExtension()const {
        ustring p = path_;
        if (p[p.size()-1] == '/' || p[p.size()-1] == '\\')
            p.pop_back();
        size_t slash_pos = p.find_last_of(U"/\\");
        size_t dot_pos = path_.find(U'.');

        if (dot_pos == ustring::npos) {
            if (slash_pos == ustring::npos)
                return p;
            return p.substr(slash_pos+1);
        }
        else if (slash_pos == ustring::npos) {
            return p.substr(0, dot_pos);
        }
        else if (slash_pos>dot_pos) {
        // dot must be after last slash
            return p.substr(slash_pos + 1);
        }

        return p.substr(slash_pos+1, dot_pos-slash_pos-1);
    }

    inline DirPath Path::getDirpath()const {
        ustring p = path_;
        if (p[p.size()-1] == '/' || p[p.size()-1] == '\\')
            p.pop_back();
        size_t found = p.find_last_of(U"/\\");
        if (found == ustring::npos)
            return p;
        return DirPath(p.substr(0,found));
    }

    inline ustring Path::normalize_(const ustring& path) {
        // replace backslashes with forward slashes
        ustring p = path;
        std::replace(p.begin(), p.end(), '\\', '/');
        p = ssu::trim(p);
        // remove trailing slash
        if (p.back() == '/')
            p.pop_back();
        return p;
    }

    inline DirPath::DirPath(const char* path)
     : Path(path)
    {
        if (path_.back() != '/')
            path_.push_back(U'/');
    }

    inline DirPath::DirPath(const ustring& path)
     : Path(path)
    {
        if (path_.back() != '/')
            path_.push_back(U'/');
    }

    inline void DirPath::listDirs(fast_vector<Path>& dirsOut, bool dive /*= false*/) {
        listDirsInner_(path_, dirsOut, dive);
    }

    inline FileLister DirPath::listFiles(const fast_vector<ustring>& extensions, bool dive /*= false*/) {
        return FileLister(*this, extensions, dive);
    }

    inline void DirPath::listDirsInner_(const Path& dir, fast_vector<Path>& dirs_out, bool dive) {
        DIR *d = opendir(dir.getPath().c_str());
        DIR *d_inner;
        struct dirent *dirp;
        if(d == NULL)
            return;
        while ( (dirp = readdir(d)) )
        {
            // ignore . and ..
            ustring name(dirp->d_name);
            ustring path = (dir+name).getPath();
            if (name == "." || name == "..")
                continue;
            d_inner = opendir( path.c_str() );
            if (d_inner)
                // is directory
            {
                if (dive)
                    // recurse into it and find leafs
                {
                    fast_vector<Path> lower_level_dirs;
                    listDirsInner_(path+"/", lower_level_dirs, dive);
                    if (!lower_level_dirs.size())
                        // no subdirs -> this is leaf dir
                    {
                        dirs_out.push_back(path);
                    }
                    else
                        // append leaf dirs to output vector
                        dirs_out.insert(dirs_out.end(), lower_level_dirs.begin(), lower_level_dirs.end());
                }
                else
                    // dont look for leafs, just add dir
                    dirs_out.push_back(path);
                closedir(d_inner);
            }
        }
        closedir(d);
    }


    inline Path operator+(const Path& p1, const ustring& s) {
        return Path(p1.path_ + s);
    }

    inline std::ostream& operator << (std::ostream& os, Path& p) {
        os << p.path_;
        return os;
    }

    inline Path operator+(const DirPath& p1, const ustring& s) {
        return Path(p1.path_ + s);
    }

    inline DirPath operator+(const DirPath& p1, const DirPath& p2) {
        DirPath dp;
        dp.path_ = p1.path_ + p2.path_;
        return dp;
    }
}