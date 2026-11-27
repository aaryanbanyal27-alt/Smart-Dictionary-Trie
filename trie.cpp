#include "trie.h"

#include <fstream>

TrieNode::TrieNode() : isEndOfWord(false) {
    for (TrieNode*& child : children) {
        child = nullptr;
    }
}

Trie::Trie() : root(new TrieNode()) {}

Trie::~Trie() {
    destroySubtree(root);
}

bool Trie::insert(const std::string& word, const std::string& meaning) {
    if (word.empty()) {
        return false;
    }

    TrieNode* current = root;
    for (char character : word) {
        if (character < 'a' || character > 'z') {
            return false;
        }
        const std::size_t index = static_cast<std::size_t>(character - 'a');
        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode();
        }
        current = current->children[index];
    }

    if (current->isEndOfWord) {
        return false;
    }
    current->isEndOfWord = true;
    current->meaning = meaning;
    return true;
}

bool Trie::search(const std::string& word, std::string& meaning) const {
    const TrieNode* node = findNode(root, word);
    if (node == nullptr || !node->isEndOfWord) {
        return false;
    }
    meaning = node->meaning;
    return true;
}

bool Trie::contains(const std::string& word) const {
    const TrieNode* node = findNode(root, word);
    return node != nullptr && node->isEndOfWord;
}

std::vector<std::pair<std::string, std::string>> Trie::autocomplete(
    const std::string& prefix) const {
    std::vector<std::pair<std::string, std::string>> words;
    const TrieNode* node = findNode(root, prefix);
    if (node == nullptr) {
        return words;
    }

    std::string currentWord = prefix;
    collectWords(node, currentWord, words);
    return words;
}

bool Trie::remove(const std::string& word) {
    if (word.empty()) {
        return false;
    }
    for (char character : word) {
        if (character < 'a' || character > 'z') {
            return false;
        }
    }

    bool removed = false;
    removeRecursive(root, word, 0, removed);
    return removed;
}

std::vector<std::pair<std::string, std::string>> Trie::getAllWords() const {
    std::vector<std::pair<std::string, std::string>> words;
    std::string currentWord;
    collectWords(root, currentWord, words);
    return words;
}

bool Trie::saveToFile(const std::string& fileName) const {
    std::ofstream output(fileName);
    if (!output) {
        return false;
    }

    for (const auto& entry : getAllWords()) {
        output << entry.first << '|' << entry.second << '\n';
    }
    return output.good();
}

bool Trie::loadFromFile(const std::string& fileName, std::size_t& loadedCount) {
    std::ifstream input(fileName);
    if (!input) {
        loadedCount = 0;
        return false;
    }

    clear();
    loadedCount = 0;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        const std::size_t separator = line.find('|');
        if (separator == std::string::npos || separator == 0 || separator + 1 >= line.size()) {
            continue;
        }

        const std::string word = line.substr(0, separator);
        const std::string meaning = line.substr(separator + 1);
        if (insert(word, meaning)) {
            ++loadedCount;
        }
    }
    return input.eof();
}

void Trie::destroySubtree(TrieNode* node) {
    if (node == nullptr) {
        return;
    }
    for (TrieNode* child : node->children) {
        destroySubtree(child);
    }
    delete node;
}

TrieNode* Trie::findNode(TrieNode* node, const std::string& word) {
    for (char character : word) {
        if (character < 'a' || character > 'z') {
            return nullptr;
        }
        const std::size_t index = static_cast<std::size_t>(character - 'a');
        if (node->children[index] == nullptr) {
            return nullptr;
        }
        node = node->children[index];
    }
    return node;
}

const TrieNode* Trie::findNode(const TrieNode* node, const std::string& word) {
    for (char character : word) {
        if (character < 'a' || character > 'z') {
            return nullptr;
        }
        const std::size_t index = static_cast<std::size_t>(character - 'a');
        if (node->children[index] == nullptr) {
            return nullptr;
        }
        node = node->children[index];
    }
    return node;
}

bool Trie::hasChildren(const TrieNode* node) {
    for (const TrieNode* child : node->children) {
        if (child != nullptr) {
            return true;
        }
    }
    return false;
}

bool Trie::removeRecursive(TrieNode* node, const std::string& word,
                           std::size_t index, bool& removed) {
    if (index == word.size()) {
        if (!node->isEndOfWord) {
            return false;
        }
        node->isEndOfWord = false;
        node->meaning.clear();
        removed = true;
    } else {
        const std::size_t childIndex = static_cast<std::size_t>(word[index] - 'a');
        TrieNode* child = node->children[childIndex];
        if (child == nullptr) {
            return false;
        }
        if (removeRecursive(child, word, index + 1, removed)) {
            delete child;
            node->children[childIndex] = nullptr;
        }
    }
    return !node->isEndOfWord && !hasChildren(node);
}

void Trie::collectWords(
    const TrieNode* node, std::string& currentWord,
    std::vector<std::pair<std::string, std::string>>& words) {
    if (node->isEndOfWord) {
        words.emplace_back(currentWord, node->meaning);
    }
    for (std::size_t index = 0; index < 26; ++index) {
        if (node->children[index] != nullptr) {
            currentWord.push_back(static_cast<char>('a' + index));
            collectWords(node->children[index], currentWord, words);
            currentWord.pop_back();
        }
    }
}

void Trie::clear() {
    destroySubtree(root);
    root = new TrieNode();
}
