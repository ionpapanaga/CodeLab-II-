#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

class Book {
private:
    std::string title;
    std::string author;
    int pages;
    std::string id;
    bool isBorrowed;

public:
    Book(std::string t, std::string a, int p, std::string idNum, bool borrowed)
        : title(t), author(a), pages(p), id(idNum), isBorrowed(borrowed) {}

    std::string getID() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    int getPages() const { return pages; }
    bool getBorrowedStatus() const { return isBorrowed; }

    void setBorrowedStatus(bool status) { isBorrowed = status; }

    void displaySummary() const {
        std::cout << std::left << std::setw(6) << id 
                  << std::setw(28) << (title.length() > 25 ? title.substr(0, 22) + "..." : title)
                  << std::setw(22) << (author.length() > 19 ? author.substr(0, 16) + "..." : author)
                  << std::setw(8) << pages 
                  << (isBorrowed ? "Borrowed" : "Available") << "\n";
    }

    void displayDetails() const {
        std::cout << "\n--- Book Details ---\n"
                  << "ID:        " << id << "\n"
                  << "Title:     " << title << "\n"
                  << "Author:    " << author << "\n"
                  << "Pages:     " << pages << "\n"
                  << "Status:    " << (isBorrowed ? "Borrowed" : "Available") << "\n";
    }
};

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<Book> loadBooks(const std::string& filename) {
    std::vector<Book> books;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return books;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        std::stringstream ss(line);
        std::string title, author, pagesStr, id, statusStr;

        if (std::getline(ss, title, ',') &&
            std::getline(ss, author, ',') &&
            std::getline(ss, pagesStr, ',') &&
            std::getline(ss, id, ',') &&
            std::getline(ss, statusStr)) {
            
            int pages = 0;
            try { pages = std::stoi(trim(pagesStr)); } catch (...) { pages = 0; }
            bool borrowed = (trim(statusStr) == "true");
            books.emplace_back(trim(title), trim(author), pages, trim(id), borrowed);
        }
    }
    return books;
}

void displayAll(const std::vector<Book>& books) {
    std::cout << "\n" << std::string(72, '=') << "\n";
    std::cout << std::left << std::setw(6) << "ID" 
              << std::setw(28) << "Title" 
              << std::setw(22) << "Author" 
              << std::setw(8) << "Pages" 
              << "Status\n";
    std::cout << std::string(72, '-') << "\n";
    for (const auto& b : books) {
        b.displaySummary();
    }
    std::cout << std::string(72, '=') << "\n";
}

int findBookIndex(const std::vector<Book>& books, const std::string& id) {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].getID() == id) return static_cast<int>(i);
    }
    return -1;
}

int main() {
    std::vector<Book> library = loadBooks("bookData.txt");
    if (library.empty()) {
        std::cerr << "No book data available to run the library manager.\n";
        return 1;
    }

    int choice = 0;
    while (true) {
        std::cout << "\n=== Library Management System ===\n"
                  << "1. View All Books\n"
                  << "2. View Individual Book\n"
                  << "3. Borrow a Book\n"
                  << "4. Return a Book\n"
                  << "5. Exit\n"
                  << "Select an option: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string discard;
            std::getline(std::cin, discard);
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }

        if (choice == 5) {
            std::cout << "Exiting system. Goodbye!\n";
            break;
        }

        std::string bookID;
        switch (choice) {
            case 1:
                displayAll(library);
                break;
            case 2: {
                std::cout << "Enter Book ID (e.g. 001): ";
                std::cin >> bookID;
                int idx = findBookIndex(library, bookID);
                if (idx != -1) library[idx].displayDetails();
                else std::cout << "Book with ID " << bookID << " not found.\n";
                break;
            }
            case 3: {
                std::cout << "Enter Book ID to borrow: ";
                std::cin >> bookID;
                int idx = findBookIndex(library, bookID);
                if (idx == -1) {
                    std::cout << "Book not found.\n";
                } else if (library[idx].getBorrowedStatus()) {
                    std::cout << "Notice: '" << library[idx].getTitle() << "' is already borrowed.\n";
                } else {
                    library[idx].setBorrowedStatus(true);
                    std::cout << "Success: You borrowed '" << library[idx].getTitle() << "'.\n";
                }
                break;
            }
            case 4: {
                std::cout << "Enter Book ID to return: ";
                std::cin >> bookID;
                int idx = findBookIndex(library, bookID);
                if (idx == -1) {
                    std::cout << "Book not found.\n";
                } else if (!library[idx].getBorrowedStatus()) {
                    std::cout << "Notice: '" << library[idx].getTitle() << "' was not borrowed.\n";
                } else {
                    library[idx].setBorrowedStatus(false);
                    std::cout << "Success: You returned '" << library[idx].getTitle() << "'.\n";
                }
                break;
            }
            default:
                std::cout << "Invalid option. Choose between 1 and 5.\n";
        }
    }
    return 0;
}