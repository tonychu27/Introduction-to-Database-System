#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "hash.h"
#include <bitset>
#include "utils.h"
#include <queue>

using namespace std;

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
};

hash_entry::hash_entry(int key, int value){
    this->key = key;
    this->value = value;
    this->next=nullptr;
}

hash_bucket::hash_bucket(int hash_key, int depth){
    this->local_depth = depth;
    this->num_entries = 0;
    this->hash_key = hash_key;
    this->first = nullptr;
}

void hash_bucket::clear() {
    hash_entry* current = this->first;
    while(current != nullptr){
        hash_entry* toDelete = current;
        current = current->next;
        delete toDelete;
    }

    this->first = nullptr;
    this->num_entries = 0;
}

hash_table::hash_table(int table_size, int bucket_size, int num_rows, vector<int> key, vector<int> value){
    this->table_size = table_size;
    this->bucket_size = bucket_size;
    this->global_depth = log2(table_size);

    for(int i = 0; i < table_size; i++) this->bucket_table.push_back(new hash_bucket(i, this->global_depth));
    for(int i = 0; i < num_rows; i++) this->insert(key[i], value[i]);
}

void hash_table::extend(hash_bucket *bucket) {

    if(bucket->local_depth < this->global_depth){
        bucket->local_depth++;

        int new_hash_key = bucket->hash_key | (1 << (bucket->local_depth - 1));
        hash_bucket* new_bucket = new hash_bucket(new_hash_key, bucket->local_depth);

        hash_entry* current = bucket->first;
        hash_entry* prev = nullptr;

        while(current != nullptr) {
            int hash_index = current->key & ((1 << this->global_depth) - 1);

            if ((hash_index & (1 << (bucket->local_depth - 1))) != 0) {
                if(prev != nullptr) prev->next = current->next;
                else bucket->first = current->next;

                hash_entry* to_move = current;
                current = current->next;

                to_move->next = new_bucket->first;
                new_bucket->first = to_move;
                new_bucket->num_entries++;
                bucket->num_entries--;
            }
            else {
                prev = current;
                current = current->next;
            }
        }

        int step = 1 << bucket->local_depth;
        int base_index = bucket->hash_key & ((1 << this->global_depth) - 1);

        for (int i = base_index; i < this->table_size; i += step) {
            if (this->bucket_table[i] == bucket) {
                int new_index = i | (1 << (bucket->local_depth - 1));
                this->bucket_table[new_index] = new_bucket;
            }
        }
    }
    else {
        this->global_depth++;
        int new_size = this->table_size * 2;
        this->bucket_table.resize(new_size);

        for(int i=0; i < this->table_size; i++) this->bucket_table[i+this->table_size] = this->bucket_table[i];
        
        this->table_size = new_size;
        this->extend(bucket);
        
    }
}

void hash_table::insert(int key, int value){
    int hash_index = key & ((1 << this->global_depth) - 1);
    hash_bucket* target_bucket = this->bucket_table[hash_index];

    hash_entry* current = target_bucket->first;
    while(current != nullptr) {
        if(current->key == key) {
            current->value = value;
            return;
        }

        current = current->next;
    }


    if(target_bucket->num_entries < this->bucket_size){
        hash_entry* new_entry = new hash_entry(key, value);
        new_entry->next = target_bucket->first;
        target_bucket->first = new_entry;
        target_bucket->num_entries++;
        return;
    }

    this->extend(target_bucket);
    this->insert(key,value);
}

void hash_table::half_table() {
    int max_local_depth = 0;
    for (auto& bucket : bucket_table)
        if (bucket != nullptr)
            max_local_depth = max(max_local_depth, bucket->local_depth);

    if (global_depth > max_local_depth) {
        
        int diff = global_depth - max_local_depth;
        table_size >>= diff;
        global_depth -= diff;

        for (int i = 0; i < table_size; i++) bucket_table[i] = bucket_table[i + table_size];

        bucket_table.resize(table_size);
    }
    
}

void hash_table::shrink(hash_bucket *bucket) {
    if (bucket->num_entries > 0 || bucket->local_depth == 1) return;
    

    int pair_hash_key = bucket->hash_key ^ (1 << (bucket->local_depth - 1));
    hash_bucket* pair_bucket = nullptr;

    if (pair_hash_key >= 0 && pair_hash_key < this->table_size) pair_bucket = this->bucket_table[pair_hash_key];
    

    if (pair_bucket && pair_bucket->local_depth == bucket->local_depth) {

        pair_bucket->local_depth--;
        pair_bucket->hash_key = pair_bucket->hash_key & ((1 << pair_bucket->local_depth) - 1);

        int step = 1 << bucket->local_depth; 
        int base_index = bucket->hash_key & ((1 << this->global_depth) - 1);

        for (int i = base_index; i < this->table_size; i += step) {
            if (this->bucket_table[i] == bucket) this->bucket_table[i] = pair_bucket;
        }

        bucket->clear();
        delete bucket;

        this->shrink(pair_bucket);
    }
}


/* When executing remove_query you can call remove() in the for loop for each key.
*/
void hash_table::remove(int key) {
    int hash_index = key & ((1 << this->global_depth) - 1);
    hash_bucket* target_bucket = this->bucket_table[hash_index];

    hash_entry* current = target_bucket->first;
    hash_entry* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            if (prev != nullptr) prev->next = current->next;
            else target_bucket->first = current->next;
            
            delete current;
            target_bucket->num_entries--;
            break;
        }

        prev = current;
        current = current->next;
    }

    if (target_bucket->num_entries == 0) this->shrink(target_bucket);
    
}


void hash_table::key_query(vector<int> query_keys, string file_name){
    ofstream output_file(file_name);

    if (!output_file.is_open()) {
        cerr << "Error opening file: " << file_name << endl;
        return;
    }


    stringstream buffer;

    buffer << this->global_depth << endl;

    for (int key : query_keys) {
        int hash_index = key & ((1 << this->global_depth) - 1);
        hash_bucket* bucket = this->bucket_table[hash_index];
        
        hash_entry* current = bucket->first;
        bool found = false;
        while (current != nullptr) {
            if (current->key == key) {
                buffer << current->value << "," << bucket->local_depth << endl;
                found = true;
                break;
            }

            current = current->next;
        }

        if (!found) buffer << -1 << "," << bucket->local_depth << endl;
        
    }


    output_file << buffer.str();
    output_file.close();
}

void hash_table::remove_query(vector<int> query_keys) {
    for (int key : query_keys) this->remove(key);
    

    std::queue<hash_bucket*> to_shrink;
    for (int i = 0; i < this->table_size; i++) {
        if (this->bucket_table[i] != nullptr && this->bucket_table[i]->num_entries == 0) to_shrink.push(this->bucket_table[i]);
        
    }

    while (!to_shrink.empty()) {
        hash_bucket* bucket = to_shrink.front();
        to_shrink.pop();
        this->shrink(bucket);
    }

    while (this->global_depth > 1) {
        bool can_shrink = true;

        for (int i = 0; i < (1 << (this->global_depth - 1)); i++) {
            if (this->bucket_table[i] != this->bucket_table[i + (1 << (this->global_depth - 1))]) {
                can_shrink = false;
                break;
            }
        }
        if (can_shrink) {
            this->global_depth--;
            this->table_size /= 2;
            this->bucket_table.resize(this->table_size);
        } 
        else break;
        
    }
}


/* Free the memory that you have allocated in this program
*/
void hash_table::clear(){
    for(hash_bucket* bucket : this->bucket_table){
        if(bucket != nullptr){
            bucket->clear();
            delete bucket;
        }
    }
    
    this->bucket_table.clear();
}

void hash_table::debug() const {
    cout << "\nHash Table State:\n";
    cout << "Global Depth: " << global_depth << endl;
    cout << "Table Size: " << table_size << endl;

    for (int i = 0; i < table_size; i++) {
        if (bucket_table[i] != nullptr) {
            hash_bucket* bucket = bucket_table[i];
            cout << "Bucket " << i << " | Local Depth: " << bucket->local_depth << " | Global Depth: " << global_depth;;
            cout << " | Hash Index: " << bucket->hash_key;
            cout << " | Entries: " << bucket->num_entries << endl;

            hash_entry* entry = bucket->first;
            while (entry != nullptr) {
                cout << "    Key: " << entry->key << " | Value: " << entry->value << endl;
                entry = entry->next;
            }

            if(bucket->num_entries > bucket_size) exit(1);
        } 
        else cout << "Bucket " << i << " is empty.\n";
    }

    cout << "End of Hash Table State.\n\n";
}