#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::array;
using std::make_unique;
using std::move;
using std::pair;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

namespace utils {

using std::ifstream;
using std::make_pair;

// The first member in the returned value is the words read from given file.
// The second member is true if file is read successfully, or false otherwise.
pair<vector<string>, bool> words_from_file(const char *filename) {
    assert(filename);

    vector<string> words;
    ifstream file(filename);
    if (!file.is_open()) {
        return make_pair(words, false);
    }

    string word;
    while (getline (file, word)) {
        words.emplace_back(word);
    }
    file.close();
    
    return make_pair(words, true);
}

} // namespace utils

// Util functions: these should be put into T9Trie directly if test() is removed
size_t letter_to_num(const char& c) {
    assert(c >= 'a' && c <= 'z');
    return c - 'a';
}

size_t letter_to_key(const char& c) {
    assert(c >= 'a' && c <= 'z');
    size_t num = -1; // invalid number
    if (c >= 'a' && c <= 'c') {
        num = 2;
    } else if (c >= 'd' && c <= 'f') {
        num = 3;
    } else if (c >= 'g' && c <= 'i') {
        num = 4;
    } else if (c >= 'j' && c <= 'l') {
        num = 5;
    } else if (c >= 'm' && c <= 'o') {
        num = 6;
    } else if (c >= 'p' && c <= 's') {
        num = 7;
    } else if (c >= 't' && c <= 'v') {
        num = 8;
    } else {
        assert(c >= 'w' && c <= 'z');
        num = 9;
    }
    return num;
}

class T9Trie {
public:
    T9Trie(vector<string>&& words) {
        init();
        build(move(words));
    };

    ~T9Trie() = default;

    vector<string> search(const string& keys) const {
        assert(root); // Must be called after build()
        vector<string> words;
        Node* n = root.get();
        for (const char& key: keys) {
            // ignore unused character
            if (key >= '2' && key <= '9') {
                size_t k = key - '0';
                if (!n->children[k]) {
                    return words;
                }
                n = n->children[k].get();
            }
        }
        words = n->words;
        return words;
    } 

private:
    void build(vector<string>&& words) {
        assert(!root); // Can be called once only
        root = make_unique<Node>();
        for (string& word: words) {
            insert(move(word));
        }
    }

    void insert(string&& word) {
        assert(root);
        Node* n = root.get();
        for (const char& c: word) {
            size_t index = letter_to_num(c);
            size_t key = letter_key.at(index);
            if (!n->children[key]) {
                n->children[key] = make_unique<Node>();
            }
            n = n->children[key].get();
        }
        n->words.emplace_back(word);
    }

    void init() {
        for(char c = 'a' ; c <= 'z' ; ++c) {
            size_t index = letter_to_num(c);
            letter_key[index] = letter_to_key(c);
        }
    }

    struct Node {
        array<unique_ptr<Node>, 10> children; // from 0 to 9 
        vector<string> words;
    };
    unique_ptr<Node> root;

    array<size_t, 26> letter_key; // from a to z
};

void test(const T9Trie& trie, const vector<string>&& words) {
    // Create a table mapping letter to key
    array<size_t, 26> letter_key; // from a to z
    for(char c = 'a' ; c <= 'z' ; ++c) {
        size_t index = letter_to_num(c);
        letter_key[index] = letter_to_key(c);
    }

    // Create a table mapping keys to words
    unordered_map<string, vector<string>> keys_to_words;
    for (const string& word: words) {
        string keys;
        for (const char& c: word) {
            size_t index = letter_to_num(c);
            size_t k = letter_key.at(index);
            keys += char(k + '0');
        }
        keys_to_words[keys].emplace_back(move(word));
    }

    // Compare the keys-to-words table with the given trie
    for (const auto& [keys, expected]: keys_to_words) {
        vector<string> actual = trie.search(keys);
        assert(actual == expected);
    }
}

int main() {
    pair<vector<string>, bool> read = utils::words_from_file("popular.txt");
    assert(read.second);

    vector<string> words = read.first;
    T9Trie t9trie(move(words));

    // case -> 2273
    string keys("*02112*#7#3##");
    vector<string> result = t9trie.search(keys);
    assert(std::find(result.begin(), result.end(), "case") != result.end());

    test(t9trie, move(read.first));

    return 0;
}