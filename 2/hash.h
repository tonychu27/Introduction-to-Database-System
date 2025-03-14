#include <vector>
#include <string>

using namespace std;

class hash_entry {
public:
    int key;
    int value;
    hash_entry *next;
    hash_entry(int key, int value);
};

class hash_bucket {
public:
    int local_depth;
    int num_entries;
    int hash_key;
    hash_entry* first;
    hash_bucket(int hash_key, int depth);

    void clear();
};

class hash_table {
public:
    int table_size;
    int bucket_size; 
    int global_depth;
    vector<hash_bucket*> bucket_table;
    hash_table(int table_size, int bucket_size, int num_rows, vector<int> key, vector<int> value);

    void extend(hash_bucket *bucket);
    void half_table();
    void shrink(hash_bucket *bucket);
    void insert(int key, int value);
    void remove(int key);

    void key_query(vector<int> query_keys, string file_name);
    void remove_query(vector<int> query_remove_keys);

    void debug() const;
    void clear();
};