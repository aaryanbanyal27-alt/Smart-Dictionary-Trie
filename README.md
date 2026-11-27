# Smart Dictionary & Auto-Suggestion System

A console dictionary built in C++17 with a manually implemented Trie. It stores word meanings, suggests completions for a prefix, and saves the dictionary to a text file.

## Features

- Add words, reject duplicates, and search for meanings
- Suggest matching words in alphabetical order
- Delete words while preserving shared Trie prefixes
- Display the full dictionary alphabetically
- Load and save the dictionary using `dictionary.txt`
- Compare repeated Trie lookups with a linear scan
- Include 25 sample words for a ready-to-run demonstration

## Data Structure

Each `TrieNode` has 26 child pointers, an `isEndOfWord` flag, and a meaning string. Each lowercase English letter selects one child. A node can mark a complete word while still having children, so words that share prefixes are stored efficiently.

The application converts uppercase English letters to lowercase before processing input. Words may contain English letters only. Meanings can contain spaces but cannot contain the `|` delimiter used in the data file.

## Project Structure

```text
Smart-Dictionary-Trie/
├── main.cpp
├── trie.h
├── trie.cpp
├── dictionary.txt
├── README.md
├── .gitignore
└── screenshots/
```

## Compile and Run

From this directory, compile with:

```bash
g++ -std=c++17 main.cpp trie.cpp -o dictionary
```

Run the application from the project directory so it can find `dictionary.txt`:

```bash
./dictionary
```

On Windows, run `dictionary.exe` after compiling with a C++17-capable compiler.

## Sample Session

```text
Loaded 25 words from dictionary.txt.

========================================
       SMART DICTIONARY (TRIE)
========================================
  1. Add a word and meaning
  2. Search for a word
  3. Get autocomplete suggestions
  4. Delete a word
  5. Display all words alphabetically
  6. Compare Trie and linear search
  7. Save dictionary
  8. Reload dictionary from file
  0. Save and exit
----------------------------------------
Choose an option: 3
Prefix: algo
Suggestions for 'algo':
  algorithm - A step-by-step procedure for solving a problem.
```

## Time Complexity

Let `L` be the word length, `P` the prefix length, and `V` the number of Trie nodes visited below the prefix.

| Operation | Complexity |
| --- | --- |
| Insert, search, delete | O(L) |
| Find autocomplete prefix | O(P) |
| Generate autocomplete results | O(V + output size) |
| Display all words | O(total Trie nodes + output size) |
| Linear search | O(N × L) worst case for N words |

Each Trie node contains 26 pointers, so memory usage depends on the total number of distinct prefixes.

## Future Improvements

- Support meanings containing the file delimiter through escaping or a structured format
- Add word editing and frequency-ranked suggestions
- Add automated unit tests and larger benchmark datasets
- Capture terminal screenshots for the `screenshots/` folder
