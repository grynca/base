//
// Created by grynca on 27. 6. 2015.
//

#ifndef PATH_H
#define PATH_H

#include "functions/ssu.h"

namespace grynca {

    // fw
    class FileLister;
    class DirPath;

    class Path {
        friend Path operator+(const Path& p1, const ustring& s);
        friend std::ostream& operator << (std::ostream& os, Path& p);
    public:
        Path();
        Path(const char* path);
        Path(const ustring& path);

        // create needed dirs, returns same as mkdir() (0 if ok, -1 on error + ERRNO set)
        bool createPathDirs();

        // "c:/a/b/c" + "c:/a/x/file.txt" => "../../x/file.txt"
        // http://mrpmorris.blogspot.com/2007/05/convert-absolute-path-to-relative-path.html
        bool convertToRelative(const ustring& relative_to);

// file manipulation
        bool exists()const;
        bool loadDataFromFile(fast_vector<u8>& data_out);
        bool saveDataToFile(const fast_vector<u8>& data);
        bool loadTextContent(ustring& str_out);
        bool saveTextContent(const ustring& str);
        bool appendTextContent(const ustring& str);
        bool clearFile();
        bool deleteFile();
// extension does not contain dot
        ustring getExtension();
        void setExtension(const ustring& ext);
        void removeExtension();
// C:/data_/obr.jpg , filename == obr.jpg, dirpath == C:/data_/
        const ustring& getPath()const;
        ustring getFilename()const;
        ustring getFilenameWOExtension()const;
        DirPath getDirpath()const;
    protected:
        ustring normalize_(const ustring& path);

        ustring path_;
    };

    class DirPath : public Path
    {
        friend Path operator+(const DirPath& p1, const ustring& s);
        friend DirPath operator+(const DirPath& p1, const DirPath& p2);
    public:
        DirPath(const char* path);
        DirPath(const ustring& path);

        void listDirs(fast_vector<Path>& dirsOut, bool dive = false);
        FileLister listFiles(const fast_vector<ustring>& extensions, bool dive = false);
    protected:
        DirPath() {}
        void listDirsInner_(const Path& dir, fast_vector<Path>& dirs_out, bool dive);
    };

}

#include "Path.inl"
#endif //PATH_H