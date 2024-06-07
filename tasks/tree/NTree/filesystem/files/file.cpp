#include "file.hpp"

namespace filesystem {

File::File() : content_() {
}

File::File(std::string data) : content_(data) {
}

void File::Read(size_t bytes) const {
    if (bytes > this->content_.size()) {
        bytes = this->content_.size();
    }
    for (size_t i = 0; i < bytes; ++i) {
        std::cout << (this->content_[i]);
    }
    std::cout << std::endl;
}

void File::Write(std::string data) {
    this->content_ = data;
}

void File::Append(std::string data) {
    this->content_ += data;
}
}  // end namespace filesystem
