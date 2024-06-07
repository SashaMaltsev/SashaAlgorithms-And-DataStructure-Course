#include "directory.hpp"

namespace filesystem {

std::string Directory::GetName() const {

    if (this->parent_ != nullptr) {
        for (auto i = this->parent_->childs_.Begin(); i != this->parent_->childs_.End(); ++i) {
            if ((*i).second == this) {
                return (*i).first;
            }
        }
    }

    return std::string();
}

Directory::Directory() : parent_(nullptr), childs_(), files_() {
}
Directory::Directory(Directory* parent) : parent_(parent), childs_(), files_() {
}

Directory::~Directory() {
    for (auto i = this->childs_.Begin(); i != this->childs_.End(); ++i) {
        delete i->second;
    }

    childs_.Clear();
    files_.Clear();
}

}  // end namespace filesystem
