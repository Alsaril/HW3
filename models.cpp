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
    std::map<std::string, std::list<std::pair<int, std::shared_ptr<Person>>>> reverseIndex;

    void addToReverseIndex(int index, std::shared_ptr<Person> person) {
        const std::string& key = person->phone;
        auto rPos = reverseIndex.find(key);
        if (rPos == reverseIndex.end()) {
            reverseIndex[key] = {};
            rPos = reverseIndex.find(key);
        }
        rPos->second.emplace_back(index, person);
    }

    void removeFromReverseIndex(PersonMap::iterator pos) {
        const Person& person = *(pos->second);
        auto rPos = reverseIndex.find(person.phone);
        if (rPos == reverseIndex.end()) {
            return;
        }
        auto& list = rPos->second;
        int size = 0;
        for (auto i = list.begin(); i != list.end(); i++, size++) {
            if (i->first == pos->first) {
                i = list.erase(i);
                if (i != list.end()) {
                    size++;
                }
                break;
            }
        }
        if (size == 0) {
            reverseIndex.erase(rPos);
        }
    }

public:
    void set(int index, std::shared_ptr<Person> person) { // insert or replace
        auto pos = content.find(index);
        if (pos != content.end()) {
            removeFromReverseIndex(pos);
        }
        content[index] = person;
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
        addToReverseIndex(pos1->first, pos1->second);
        addToReverseIndex(pos2->first, pos2->second);
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

            set(index, std::make_shared<Person>(dummy));
        }
        in.close();
    }

    void add(std::shared_ptr<Person> person) {
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
        set(to, pos->second);
        remove(from);
    }

    void find(const std::string& query, std::vector<std::pair<int, std::shared_ptr<Person>>>& results) {
        for (auto i = reverseIndex.lower_bound(query); i != reverseIndex.end() && i->first.rfind(query, 0) != std::string::npos; i++) {
            for (const auto& [pos, person]: i->second) {
                results.emplace_back(pos, person);
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
            set(i + 1, tmp[i]);
        }
    }

    void clear() {
        content.clear();
        reverseIndex.clear();
    }

    void printState() {
        std::cout << "\n##### DEBUG #####\n#\n";
        std::cout << "#  Plain storage:\n";
        for (const auto& [key, person]: content) {
            std::cout << "#  " << key << " -> ";
            person->print(std::cout);
            std::cout << '\n';
        }
        std::cout << '\n';
        std::cout << "#  Reverse index:\n";
        for (const auto& [key, list]: reverseIndex) {
            std::cout << "#  " << key << " -> ";
            for (const auto& [index, person]: list) {
                std::cout << '(' << index << ", ";
                person->print(std::cout);
                std::cout << "), ";
            }
            std::cout << '\n';
        }
        std::cout << "#\n#################\n";
    }
};

} // namespace PhoneBook
