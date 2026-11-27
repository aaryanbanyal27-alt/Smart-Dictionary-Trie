#ifndef TRIE_H
#define TRIE_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

struct TrieNode {
    TrieNode();

    TrieNode* children[26];
    bool isEndOfWord;
    std::string meaning;
};

class Trie {
public:
    Trie();
    ~Trie();

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    bool insert(const std::string& word, const std::string& meaning);
    bool search(const std::string& word, std::string& meaning) const;
    bool contains(const std::string& word) const;
    std::vector<std::pair<std::string, std::string>> autocomplete(
        const std::string& prefix) const;
    bool remove(const std::string& word);
    std::vector<std::pair<std::string, std::string>> getAllWords() const;
    bool saveToFile(const std::string& fileName) const;
    bool loadFromFile(const std::string& fileName, std::size_t& loadedCount);

private:
    TrieNode* root;

    static void destroySubtree(TrieNode* node);
    static TrieNode* findNode(TrieNode* node, const std::string& word);
    static const TrieNode* findNode(const TrieNode* node, const std::string& word);
    static bool hasChildren(const TrieNode* node);
    static bool removeRecursive(TrieNode* node, const std::string& word,
                                std::size_t index, bool& removed);
    static void collectWords(const TrieNode* node, std::string& currentWord,
                             std::vector<std::pair<std::string, std::string>>& words);
    void clear();
};

#endif
