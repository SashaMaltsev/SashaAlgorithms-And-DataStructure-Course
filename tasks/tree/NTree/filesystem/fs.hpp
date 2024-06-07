#pragma once

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

#include "files/directory.hpp"

namespace filesystem {

class Fs {
public:
    Fs();

    ~Fs();

    void ChangeDir(const std::string& path);

    void PWD() const;

    void RemoveFile(const std::string& path);

    void ListFiles(const std::string& path) const;

    void MakeDir(const std::string& path, bool is_create_parents = false);

    void CreateFile(const std::string& path, bool is_overwrite);

    void WriteToFile(const std::string& filename, bool is_overwrite, std::ostringstream& stream);

    void ShowFileContent(const std::string& path);

    void FindFile(const std::string& filename);

private:
    std::vector<std::string> Split(const std::string& str, const std::string& splitter) const;
    bool Find(Directory& dir, const std::string& filename, std::vector<std::string>& path);
    void RemoveDir(Directory* it);

private:
    filesystem::Directory* root_;
    filesystem::Directory* current_;
};

}  // end namespace filesystem
