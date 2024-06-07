#pragma once

#include <cstdint>
#include <iostream>
#include <string>

namespace filesystem {

class File {
    friend class Fs;

public:
    File();

    explicit File(std::string data);

    void Read(size_t bytes = SIZE_MAX) const;

    void Write(std::string data);

    void Append(std::string data);

private:
    std::string content_;
};

}  // end namespace filesystem
