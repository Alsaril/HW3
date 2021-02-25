#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <vector>
#include <memory>

namespace PhoneBook {

struct Person {
    std::string firstName, lastName, middleName, phone;

    void read(std::istream& in) {
        in >> lastName >> firstName >> middleName >> phone;
    }

    void print(std::ostream& out) const {
        out << lastName << ' ' << firstName << ' ' << middleName << ' ' << phone;
    }
};

class Book {
private:
    std::map<int, std::shared_ptr<Person>> content;
    std::map<std::string, std::vector<int>> reverseIndex;

    void updateReverseIndex(std::shared_ptr<Person> person, int before, int after) {

    }

public:
    void set(int index, const Person& person) {
        content[index] = std::make_shared<Person>(person);
    }

    void swap(int i, int j) {
        std::swap(content[i], content[j]);
    }

    void remove(int index) {
        content.erase(index);
    }

    void forEach(std::function<void(int, const Person&)> action) {
        for (auto const& [index, person]: content) {
            action(index, *person);
        }
    }

    void save(const std::string& path) {
        std::ofstream out(path);
        forEach([&out](int index, const Person& person) {
            out << index << '\t';
            person.print(out);
            out << '\n';
        });
        out.close();
    }

    void load(const std::string& path) {
        int index;
        Person dummy;

        std::ifstream in(path);
        while (in.is_open() && !in.eof()) {
            in >> index;
            dummy.read(in);

            set(index, dummy);
        }
        in.close();
    }

    void add(const Person& person) {
        if (content.size() == 0) {
            set(0, person);
        } else {
            set(content.rbegin()->first + 1, person); // max + 1
        }
    }

    void move(int from, int to) {
        auto pos = content.find(from);
        if (pos == content.end()) {
            std::cout << "No entry with index " << from << "is found\n";
            return;
        }
        set(to, *(pos->second));
        remove(from);
    }

    void find(const std::string& query, std::vector<std::shared_ptr<Person>> results) {
        
    }

    void sort() {
        std::vector<std::shared_ptr<Person>> tmp;
        tmp.reserve(content.size());
        for (auto const& [index, personPtr]: content) {
            tmp.push_back(personPtr);
        }
        content.clear();
        std::sort(tmp.begin(), tmp.end(), [](const auto& p1, const auto& p2) { return p1->lastName < p2->lastName; });
        
        for (int i = 0; i < tmp.size(); i++) {
            set(i + 1, *tmp[i]);
        }
    }
};

} // namespace PhoneBook
