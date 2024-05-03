#include "fs.hpp"

namespace filesystem {

Fs::Fs() {
    this->root_ = new Directory();
    this->current_ = this->root_;
}

Fs::~Fs() {
    delete this->root_;
}

void Fs::ChangeDir(const std::string& path) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    if (v_path.size() == 1) {
        it = this->current_;
    }

    for (size_t i = 1; i < v_path.size(); ++i) {

        if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

            it = it->childs_[v_path[i]];

        } else {
            throw filesystem::exceptions::FileNotFoundException("Dir not found");
        }
    }

    this->current_ = it;
}

void Fs::PWD() const {
    std::vector<std::string> path;
    Directory* it = this->current_;

    while (it != nullptr) {
        path.push_back(it->GetName());
        it = it->parent_;
    }

    for (auto i = path.rbegin() + 1; i != path.rend(); ++i) {

        std::cout << "/" << *i;
    }
    std::cout << std::endl;
}

void Fs::RemoveFile(const std::string& path) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    if (path != "/") {

        for (size_t i = 0; i < v_path.size(); ++i) {

            if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

                it = it->childs_[v_path[i]];

            } else {
                if ((i == v_path.size() - 1) and (it->files_.Find(v_path[i]) != it->files_.End())) {
                    it->files_.Erase(v_path[i]);
                    return;
                }
                throw filesystem::exceptions::FileNotFoundException("Dir not found");
            }
        }
    }

    RemoveDir(it);
}

void Fs::ListFiles(const std::string& path = ".") const {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    for (size_t i = 1; i < v_path.size(); ++i) {

        if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

            it = it->childs_[v_path[i]];

        } else {
            throw filesystem::exceptions::FileNotFoundException("Dir not found");
        }
    }

    for (auto i = it->files_.Begin(); i != it->files_.End(); ++i) {
        std::cout << i->first << std::endl;
    }
}

void Fs::MakeDir(const std::string& path, bool is_create_parents) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    for (size_t i = 1; i < v_path.size(); ++i) {

        if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

            it = it->childs_[v_path[i]];

        } else {
            if (!is_create_parents and i != v_path.size() - 1) {
                throw filesystem::exceptions::FileNotFoundException("Dir not found");
            }

            it->childs_.Insert({v_path[i], new Directory(it)});
            it = it->childs_[v_path[i]];
        }
    }
}

void Fs::CreateFile(const std::string& path = ".", bool is_overwrite = false) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    if (v_path.size() == 1) {
        it = this->current_;
    } else {

        for (size_t i = 1; i < v_path.size() - 1; ++i) {

            if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

                it = it->childs_[v_path[i]];

            } else {
                throw filesystem::exceptions::FileNotFoundException("Dir not found");
            }
        }
    }

    std::pair<std::string, File> p{v_path[v_path.size() - 1], File()};

    if (is_overwrite and (it->files_.Find(p.first) != it->files_.End())) {
        it->files_.Erase(p.first);
    }

    it->files_.Insert(p);
}

void Fs::WriteToFile(const std::string& path, bool is_overwrite, std::ostringstream& stream) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    for (size_t i = 1; i < v_path.size() - 1; ++i) {

        if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

            it = it->childs_[v_path[i]];

        } else {
            throw filesystem::exceptions::FileNotFoundException("Dir not found");
        }
    }

    if (is_overwrite) {
        it->files_[v_path[v_path.size() - 1]].Write(stream.str());
    } else {
        it->files_[v_path[v_path.size() - 1]].Append(stream.str());
    }
}

void Fs::ShowFileContent(const std::string& path) {

    std::vector<std::string> v_path = Split(path, "/");
    Directory* it = this->root_;

    for (size_t i = 1; i < v_path.size() - 1; ++i) {

        if (it->childs_.Find(v_path[i]) != it->childs_.End()) {

            it = it->childs_[v_path[i]];

        } else {
            throw filesystem::exceptions::FileNotFoundException("Dir not found");
        }
    }

    it->files_[v_path[v_path.size() - 1]].Read();
}

void Fs::FindFile(const std::string& filename) {

    std::vector<std::string> path;

    if (Find(*(this->root_), filename, path)) {
        for (auto i = path.rbegin(); i != path.rend(); ++i) {
            std::cout << *i << "/";
        }
        std::cout << std::endl;
    } else {
        throw filesystem::exceptions::FileNotFoundException("Dir not found");
    }
}

std::vector<std::string> Fs::Split(const std::string& str, const std::string& splitter) const {

    std::vector<std::string> res;

    size_t it1 = 0;
    size_t it2 = str.find(splitter);

    while (it2 != std::string::npos) {
        res.push_back(str.substr(it1, it2 - it1));
        it1 = it2 + 1;
        it2 = str.find(splitter, it1);
    }

    res.push_back(str.substr(it1, it2));

    return res;
}

bool Fs::Find(Directory& dir, const std::string& filename, std::vector<std::string>& path) {

    if (dir.files_.Find(filename) != dir.files_.End()) {
        path.push_back(filename);
        return true;
    }

    for (auto i = dir.childs_.Begin(); i != dir.childs_.End(); ++i) {
        if (Find(*((*i).second), filename, path)) {
            path.push_back((*i).first);
            return true;
        }
    }

    return false;
}

void Fs::RemoveDir(Directory* it) {
    for (auto i = it->childs_.Begin(); i != it->childs_.End(); ++i) {
        delete i->second;
    }

    if (it->parent_ != nullptr) {
        for (auto i = it->parent_->childs_.Begin(); i != it->parent_->childs_.End(); ++i) {
            if (i->second == it) {
                it->parent_->childs_.Erase(i->first);
            }
        }
    }

    if (it == this->current_) {
        this->current_ = this->root_;
    }

    if (it != this->root_) {
        delete it;
    } else {
        it->files_.Clear();
        it->childs_.Clear();
    }
}
}  // end namespace filesystem
