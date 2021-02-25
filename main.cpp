#include <string>
#include <unordered_map>
#include "models.cpp"
#include "command.cpp"

void printHelp(const std::unordered_map<std::string, std::unique_ptr<PhoneBook::Command>>& commands) {
    std::cout << "Unknown command, ignoring\n" << "List of possible commands:\n";
    for (const auto& [key, command]: commands) {
        std::cout << key << "\t" << command->description() << '\n';
    }
}

int main() {
    std::unordered_map<std::string, std::unique_ptr<PhoneBook::Command>> commands;
    commands["edit"] = std::make_unique<PhoneBook::EditCommand>();
    commands["swap"] = std::make_unique<PhoneBook::SwapCommand>();
    commands["print"] = std::make_unique<PhoneBook::PrintCommand>();
    commands["add"] = std::make_unique<PhoneBook::AddCommand>();
    commands["remove"] = std::make_unique<PhoneBook::RemoveCommand>();
    commands["move"] = std::make_unique<PhoneBook::MoveCommand>();
    commands["find"] = std::make_unique<PhoneBook::FindCommand>();
    commands["sort"] = std::make_unique<PhoneBook::SortCommand>();

    std::string snapshotPath = "snapshot";
	PhoneBook::Book book;
    book.load(snapshotPath);

    std::string command;

    while (true) {
        std::cin >> command;
        std::cout << command << '\n';
        auto pos = commands.find(command);
        if (pos == commands.end()) {
            printHelp(commands);
            continue;
        }
        auto& realCommand = pos->second;
        realCommand->execute(book);
        if (realCommand->mutate()) {
            book.save(snapshotPath);
        }
        if (realCommand->terminate()) {
            return 0;
        }
    }
}
