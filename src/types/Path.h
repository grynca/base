//
// Created by grynca on 27. 6. 2015.
//

#ifndef PATH_H
#define PATH_H

#include "../functions/string_utils.h"
#include <iostream>

namespace grynca {

    // fw
    class FileLister;

    class Path {
        friend Path operator+(const Path& p1, const std::string& s);
        friend std::ostream& operator << (std::ostream& os, Path& p);
    public:
        Path();
        static Path createDirPath(const std::string& s);
        static Path createFilePath(const std::string& s);

        // create needed dirs, returns same as mkdir() (0 if ok, -1 on error + ERRNO set)
        bool createPathDirs();

        // "c:/a/b/c" + "c:/a/x/file.txt" => "../../x/file.txt"
        // http://mrpmorris.blogspot.com/2007/05/convert-absolute-path-to-relative-path.html
        bool convertToRelative(const std::string& relative_to, bool is_dir);

// file manipulation
        bool exists();
        bool loadDataFromFile(fast_vector<uint8_t>& data_out);
        bool saveDataToFile(const fast_vector<uint8_t>& data);
        bool loadStringFromFile(std::string& path_out);
        bool saveStringToFile(const std::string& str);
        bool clearFile();
        bool deleteFile();
// extension does not contain dot
        std::string getExtension();
        void setExtension(const std::string& ext);
        void removeExtension();
// C:/data/obr.jpg , filename == obr.jpg, dirpath == C:/data/
        const std::string& getPath();
        std::string getFilename();
        std::string getDirpath();

        bool isDir();

        void listDirs(fast_vector<Path>& dirsOut, bool dive = false);
        FileLister listFiles(const fast_vector<std::string>& extensions, bool dive = false);
    protected:

        Path(const std::string& s, bool dir_path);

        std::string normalize_(const std::string& path, bool is_dir);
        void listDirsInner_(const std::string& dir, fast_vector<Path>& dirs_out, bool dive);

        bool is_dir_;
        std::string path_;
    };
}

#include "Path.inl"
#endif //PATH_H
