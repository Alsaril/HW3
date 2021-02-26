#pragma once

#include <iostream>
#include "models.cpp"

namespace PhoneBook {

class Command {
private:
    bool isTerminate;
    bool isMutate;
    std::string desc;
public:
    Command(bool isTerminate, bool isMutate, const std::string& desc): isTerminate(isTerminate), isMutate(isMutate), desc(desc) {}
    bool terminate() { return isTerminate; };
    bool mutate() { return isMutate; };
    const std::string& description() const { return desc; }

    virtual void execute(Book& book) {}
    virtual ~Command() {}
};

class EditCommand: public Command {
private:
    Person dummy;
    int index;

public:
    EditCommand(): Command(false, true, "edits person on desired position: <index> <last_name> <first_name> <middle_name> <phone>") {}

    virtual void execute(Book& book) {
        std::cin >> index;
        dummy.read(std::cin);
        book.set(index, std::make_shared<Person>(dummy));
    }
};

class SwapCommand: public Command {
private:
    int i, j;

public:
    SwapCommand(): Command(false, true, "swaps two entries: <first_index> <second_index>") {}

    virtual void execute(Book& book) {
        std::cin >> i >> j;
        book.swap(i, j);
    }
};

class PrintCommand: public Command {
public:
    PrintCommand(): Command(false, false, "prints all entries") {}

    virtual void execute(Book& book) {
        book.forEach([](int index, const Person& person) {
            std::cout << index << '\t';
            person.print(std::cout);
            std::cout << '\n';
        });
    }
};

class ExitCommand: public Command {
public:
    ExitCommand(): Command(true, false, "exits the program") {}
};

class AddCommand: public Command {
private:
    Person dummy;

public:
    AddCommand(): Command(false, true, "inserts new person to the end of the book: <last_name> <first_name> <middle_name> <phone>") {}

    virtual void execute(Book& book) {
        dummy.read(std::cin);
        book.add(std::make_shared<Person>(dummy));
    }
};

class RemoveCommand: public Command {
private:
    int index;

public:
    RemoveCommand(): Command(false, true, "removes entry by index: <index>") {}

    virtual void execute(Book& book) {
        std::cin >> index;
        book.remove(index);
    }
};

class MoveCommand: public Command {
private:
    int from;
    int to;

public:
    MoveCommand(): Command(false, true, "moves entry to new index: <from_index> <to_index>") {}

    virtual void execute(Book& book) {
        std::cin >> from >> to;
        book.move(from, to);
    }
};

class FindCommand: public Command {
private:
    std::string query;

public:
    FindCommand(): Command(false, false, "searches entries by phone prefix") {}

    virtual void execute(Book& book) {
        std::cin >> query;

        std::vector<std::pair<int, std::shared_ptr<Person>>> results;
        book.find(query, results);

        for (auto& pair: results) {
            std::cout << pair.first << '\t';
            pair.second->print(std::cout);
            std::cout << '\n';
        }
    }
};

class SortCommand: public Command {
public:
    SortCommand(): Command(false, true, "sorts book by last_name") {}

    virtual void execute(Book& book) {
        book.sort();
    }
};

class ClearCommand: public Command {
public:
    ClearCommand(): Command(false, true, "clears content") {}

    virtual void execute(Book& book) {
        book.clear();
    }
};

class NopCommand: public Command {
public:
    NopCommand(): Command(false, false, "does nothing") {}
};
} // namespace PhoneBook
