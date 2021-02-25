#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <list>
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

typedef std::map<int, std::shared_ptr<Person>> PersonMap;

// all modifications are made through set, swap and remove
class Book {
private:
    PersonMap content;
    std::map<std::string, std::list<int>, std::less<std::string>> reverseIndex;

    void addToReverseIndex(int index, const Person& person) {
        auto rPos = reverseIndex.find(person.phone);
        if (rPos == reverseIndex.end()) {
            reverseIndex[person.phone] = {};
            rPos = reverseIndex.find(person.phone);
        }
        rPos->second.push_back(index);
    }

    void removeFromReverseIndex(PersonMap::iterator pos) {
        const Person& person = *(pos->second);
        auto rPos = reverseIndex.find(person.phone);
        if (rPos == reverseIndex.end()) {
            return;
        }
        auto& list = rPos->second;
        for (auto i = list.begin(); i != list.end(); i++) {
            if (*i == pos->first) {
                list.erase(i);
                break;
            }
        }
    }

public:
    void set(int index, const Person& person) { // insert or replace
        auto pos = content.find(index);
        if (pos != content.end()) {
            removeFromReverseIndex(pos);
        }
        content[index] = std::make_shared<Person>(person);
        addToReverseIndex(index, person);
    }

    void swap(int i, int j) { // swap entries
        auto pos1 = content.find(i);
        auto pos2 = content.find(j);
        if (pos1 == content.end() || pos2 == content.end()) {
            std::cout << "Either first or second entry is missing\n";
            return;
        }
        removeFromReverseIndex(pos1);
        removeFromReverseIndex(pos2);
        std::swap(pos1->second, pos2->second);
        addToReverseIndex(pos1->first, *(pos1->second));
        addToReverseIndex(pos2->first, *(pos2->second));
    }

    void remove(int index) { // remove entry
        auto pos = content.find(index);
        if (pos == content.end()) {
            return;
        }
        removeFromReverseIndex(pos);
        content.erase(pos);
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
            set(1, person);
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

    void find(const std::string& query, std::vector<std::pair<int, std::shared_ptr<Person>>>& results) {
        for (auto i = reverseIndex.lower_bound(query); i != reverseIndex.end() && i->first.rfind(query, 0) != std::string::npos; i++) {
            for (auto pos: i->second) {
                results.emplace_back(pos, content[pos]);
            }
        }
    }

    void sort() {
        std::vector<std::shared_ptr<Person>> tmp;
        tmp.reserve(content.size());
        for (auto const& [index, personPtr]: content) {
            tmp.push_back(personPtr);
        }
        clear();
        std::sort(tmp.begin(), tmp.end(), [](const auto& p1, const auto& p2) { return p1->lastName < p2->lastName; });
        
        for (int i = 0; i < tmp.size(); i++) {
            set(i + 1, *tmp[i]);
        }
    }

    void clear() {
        content.clear();
        reverseIndex.clear();
    }
};

} // namespace PhoneBook
