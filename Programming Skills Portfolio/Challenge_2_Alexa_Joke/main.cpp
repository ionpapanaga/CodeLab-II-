#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

// Structure to hold a joke setup and its punchline
struct Joke {
    std::string setup;
    std::string punchline;
};

// Function to load jokes from the specified file
std::vector<Joke> loadJokes(const std::string& filename) {
    std::vector<Joke> jokes;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return jokes;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimPos = line.find('?');
        if (delimPos != std::string::npos) {
            Joke j;
            j.setup = line.substr(0, delimPos + 1); // Keep the '?' with the setup
            j.punchline = line.substr(delimPos + 1);
            jokes.push_back(j);
        }
    }
    return jokes;
}

// Helper to trim leading/trailing whitespace
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

int main() {
    std::vector<Joke> jokes = loadJokes("randomJokes.txt");
    if (jokes.empty()) {
        std::cerr << "No jokes found or file is missing.\n";
        return 1;
    }

    // Set up modern random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, jokes.size() - 1);

    std::cout << "=========================================\n";
    std::cout << "         Alexa Joke Generator            \n";
    std::cout << "=========================================\n";
    std::cout << "Type 'Alexa, tell me a joke' to hear one, or 'quit' to exit.\n\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, input)) break;

        std::string cleaned = trim(input);
        if (cleaned == "quit" || cleaned == "Quit" || cleaned == "q" || cleaned == "Q") {
            std::cout << "Goodbye!\n";
            break;
        }

        if (cleaned == "Alexa, tell me a joke") {
            const Joke& selectedJoke = jokes[dist(gen)];
            std::cout << "\nAlexa: " << selectedJoke.setup << "\n";
            std::cout << "[Press Enter to see punchline...]";
            std::string enterPrompt;
            std::getline(std::cin, enterPrompt);
            std::cout << "Alexa: " << selectedJoke.punchline << "\n\n";
        } else {
            std::cout << "Unrecognized prompt. Please enter \"Alexa, tell me a joke\" or \"quit\".\n";
        }
    }

    return 0;
}