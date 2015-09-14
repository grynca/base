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
        Path(const char* path);
        Path(const std::string& path);

        // create needed dirs, returns same as mkdir() (0 if ok, -1 on error + ERRNO set)
        bool createPathDirs();

        // "c:/a/b/c" + "c:/a/x/file.txt" => "../../x/file.txt"
        // http://mrpmorris.blogspot.com/2007/05/convert-absolute-path-to-relative-path.html
        bool convertToRelative(const std::string& relative_to);

// file manipulation
        bool exists()const;
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
        const std::string& getPath()const;
        std::string getFilename()const;
        std::string getDirpath()const;

        void listDirs(fast_vector<Path>& dirsOut, bool dive = false);
        FileLister listFiles(const fast_vector<std::string>& extensions, bool dive = false);
    protected:
        std::string normalize_(const std::string& path);
        void listDirsInner_(const Path& dir, fast_vector<Path>& dirs_out, bool dive);

        std::string path_;
    };
}

#include "Path.inl"
#endif //PATH_H
