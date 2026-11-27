#include "trie.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

const std::string dictionaryFile = "dictionary.txt";

std::string trim(const std::string& value) {
    const std::string whitespace = " \t\n\r\f\v";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

bool normalizeWord(std::string& word) {
    word = trim(word);
    if (word.empty()) {
        return false;
    }

    for (char& character : word) {
        const unsigned char value = static_cast<unsigned char>(character);
        if (!std::isalpha(value) || value > 127) {
            return false;
        }
        character = static_cast<char>(std::tolower(value));
    }
    return true;
}

bool readWord(const std::string& prompt, std::string& word) {
    std::cout << prompt;
    if (!std::getline(std::cin, word)) {
        return false;
    }
    if (!normalizeWord(word)) {
        std::cout << "Please enter a word containing English letters only.\n";
        return false;
    }
    return true;
}

void showMenu() {
    std::cout << "\n========================================\n"
              << "       SMART DICTIONARY (TRIE)\n"
              << "========================================\n"
              << "  1. Add a word and meaning\n"
              << "  2. Search for a word\n"
              << "  3. Get autocomplete suggestions\n"
              << "  4. Delete a word\n"
              << "  5. Display all words alphabetically\n"
              << "  6. Compare Trie and linear search\n"
              << "  7. Save dictionary\n"
              << "  8. Reload dictionary from file\n"
              << "  0. Save and exit\n"
              << "----------------------------------------\n"
              << "Choose an option: ";
}

void compareSearch(const Trie& dictionary) {
    std::vector<std::pair<std::string, std::string>> entries = dictionary.getAllWords();
    if (entries.empty()) {
        std::cout << "The dictionary is empty. Add or load words first.\n";
        return;
    }

    std::string word;
    if (!readWord("Enter a word to benchmark: ", word)) {
        return;
    }

    constexpr std::size_t repetitions = 20000;
    std::size_t trieMatches = 0;
    std::size_t linearMatches = 0;

    const auto trieStart = std::chrono::steady_clock::now();
    for (std::size_t iteration = 0; iteration < repetitions; ++iteration) {
        trieMatches += dictionary.contains(word) ? 1U : 0U;
    }
    const auto trieEnd = std::chrono::steady_clock::now();

    const auto linearStart = std::chrono::steady_clock::now();
    for (std::size_t iteration = 0; iteration < repetitions; ++iteration) {
        const auto match = std::find_if(entries.begin(), entries.end(),
                                        [&word](const auto& entry) {
                                            return entry.first == word;
                                        });
        linearMatches += match != entries.end() ? 1U : 0U;
    }
    const auto linearEnd = std::chrono::steady_clock::now();

    const double trieNanoseconds = static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(trieEnd - trieStart).count());
    const double linearNanoseconds = static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(linearEnd - linearStart).count());

    std::cout << "\nSearch benchmark (" << repetitions << " repetitions, " << entries.size()
              << " dictionary words)\n"
              << "  Trie:          " << std::fixed << std::setprecision(2)
              << trieNanoseconds / repetitions << " ns/search\n"
              << "  Linear search: " << linearNanoseconds / repetitions << " ns/search\n"
              << "  Results:       " << (trieMatches == repetitions && linearMatches == repetitions
                                            ? "word found by both methods"
                                            : "word not found by either method")
              << "\nTimings vary by machine and dictionary size.\n";
}

}

int main() {
    Trie dictionary;
    std::size_t loadedWords = 0;
    if (dictionary.loadFromFile(dictionaryFile, loadedWords)) {
        std::cout << "Loaded " << loadedWords << " words from " << dictionaryFile << ".\n";
    } else {
        std::cout << "Could not open " << dictionaryFile << "; starting with an empty dictionary.\n";
    }

    std::string choice;
    while (true) {
        showMenu();
        if (!std::getline(std::cin, choice)) {
            std::cout << "\nInput closed. Saving dictionary...\n";
            if (!dictionary.saveToFile(dictionaryFile)) {
                std::cout << "Error: could not save " << dictionaryFile << ".\n";
            }
            break;
        }

        if (choice == "0") {
            if (dictionary.saveToFile(dictionaryFile)) {
                std::cout << "Dictionary saved. Goodbye!\n";
            } else {
                std::cout << "Error: could not save " << dictionaryFile << ".\n";
            }
            break;
        }

        if (choice == "1") {
            std::string word;
            if (!readWord("Word: ", word)) {
                continue;
            }
            std::cout << "Meaning: ";
            std::string meaning;
            if (!std::getline(std::cin, meaning)) {
                break;
            }
            meaning = trim(meaning);
            if (meaning.empty() || meaning.find('|') != std::string::npos) {
                std::cout << "Please enter a non-empty meaning without the '|' character.\n";
                continue;
            }
            if (!dictionary.insert(word, meaning)) {
                std::cout << "The word '" << word << "' already exists.\n";
            } else if (dictionary.saveToFile(dictionaryFile)) {
                std::cout << "Added '" << word << "' and saved the dictionary.\n";
            } else {
                std::cout << "Added '" << word << "', but saving failed.\n";
            }
        } else if (choice == "2") {
            std::string word;
            if (!readWord("Word to search: ", word)) {
                continue;
            }
            std::string meaning;
            if (dictionary.search(word, meaning)) {
                std::cout << word << ": " << meaning << '\n';
            } else {
                std::cout << "'" << word << "' was not found.\n";
            }
        } else if (choice == "3") {
            std::string prefix;
            if (!readWord("Prefix: ", prefix)) {
                continue;
            }
            const auto suggestions = dictionary.autocomplete(prefix);
            if (suggestions.empty()) {
                std::cout << "No words found for prefix '" << prefix << "'.\n";
            } else {
                std::cout << "Suggestions for '" << prefix << "':\n";
                for (const auto& entry : suggestions) {
                    std::cout << "  " << entry.first << " - " << entry.second << '\n';
                }
            }
        } else if (choice == "4") {
            std::string word;
            if (!readWord("Word to delete: ", word)) {
                continue;
            }
            if (!dictionary.remove(word)) {
                std::cout << "'" << word << "' was not found.\n";
            } else if (dictionary.saveToFile(dictionaryFile)) {
                std::cout << "Deleted '" << word << "' and saved the dictionary.\n";
            } else {
                std::cout << "Deleted '" << word << "', but saving failed.\n";
            }
        } else if (choice == "5") {
            const auto entries = dictionary.getAllWords();
            if (entries.empty()) {
                std::cout << "The dictionary is empty.\n";
            } else {
                std::cout << "\nDictionary (" << entries.size() << " words):\n";
                for (const auto& entry : entries) {
                    std::cout << "  " << entry.first << " - " << entry.second << '\n';
                }
            }
        } else if (choice == "6") {
            compareSearch(dictionary);
        } else if (choice == "7") {
            if (dictionary.saveToFile(dictionaryFile)) {
                std::cout << "Dictionary saved to " << dictionaryFile << ".\n";
            } else {
                std::cout << "Error: could not save " << dictionaryFile << ".\n";
            }
        } else if (choice == "8") {
            std::size_t count = 0;
            if (dictionary.loadFromFile(dictionaryFile, count)) {
                std::cout << "Reloaded " << count << " words from " << dictionaryFile << ".\n";
            } else {
                std::cout << "Error: could not open " << dictionaryFile << ".\n";
            }
        } else {
            std::cout << "Invalid option. Choose a number from 0 to 8.\n";
        }
    }

    return 0;
}
