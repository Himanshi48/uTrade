#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>
#include <sstream>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;

struct Entry {
    string value;
    long long expiry;
};

unordered_map<string, Entry> store;
mutex mtx;
int expired_cleaned = 0;

long long now() {
    return chrono::system_clock::to_time_t(
        chrono::system_clock::now());
}

bool isExpired(const Entry &e) {
    if (e.expiry == -1) return false;
    return now() > e.expiry;
}

void SET(string key, string value, int ex) {
    lock_guard<mutex> lock(mtx);

    long long expiry = -1;
    if (ex > 0)
        expiry = now() + ex;

    store[key] = {value, expiry};

    cout << "OK" << endl;
}

void GET(string key) {
    lock_guard<mutex> lock(mtx);

    if (store.find(key) == store.end()) {
        cout << "(nil)" << endl;
        return;
    }

    Entry &e = store[key];

    if (isExpired(e)) {
        store.erase(key);
        expired_cleaned++;
        cout << "(nil)" << endl;
        return;
    }

    cout << e.value << endl;
}

void DEL(string key) {
    lock_guard<mutex> lock(mtx);

    if (store.erase(key))
        cout << "OK" << endl;
    else
        cout << "(nil)" << endl;
}

void TTL(string key) {
    lock_guard<mutex> lock(mtx);

    if (store.find(key) == store.end()) {
        cout << -2 << endl;
        return;
    }

    Entry &e = store[key];

    if (e.expiry == -1) {
        cout << -1 << endl;
        return;
    }

    int remaining = e.expiry - now();

    if (remaining <= 0) {
        store.erase(key);
        expired_cleaned++;
        cout << -2 << endl;
        return;
    }

    cout << remaining << endl;
}

void KEYS(string pattern) {
    lock_guard<mutex> lock(mtx);

    if (pattern.back() != '*') return;

    string prefix = pattern.substr(0, pattern.size() - 1);

    for (auto &p : store) {
        if (!isExpired(p.second) && p.first.find(prefix) == 0)
            cout << p.first << endl;
    }
}

void STATS() {
    lock_guard<mutex> lock(mtx);

    cout << "Total keys: " << store.size() << endl;
    cout << "Expired cleaned: " << expired_cleaned << endl;
    cout << "Memory estimate: "
         << store.size() * sizeof(Entry)
         << " bytes" << endl;
}

void SAVE() {
    lock_guard<mutex> lock(mtx);

    ofstream file("snapshot.json");

    file << "{\n";

    for (auto &p : store) {
        if (!isExpired(p.second)) {
            file << "\"" << p.first << "\":\""
                 << p.second.value << "\",\n";
        }
    }

    file << "}\n";

    file.close();

    cout << "Snapshot saved" << endl;
}

void LOAD() {
    lock_guard<mutex> lock(mtx);

    ifstream file("snapshot.json");

    string line;

    while (getline(file, line)) {

        int k1 = line.find("\"");
        int k2 = line.find("\"", k1 + 1);

        if (k1 == string::npos) continue;

        string key = line.substr(k1 + 1, k2 - k1 - 1);

        int v1 = line.find("\"", k2 + 1);
        int v2 = line.find("\"", v1 + 1);

        string value = line.substr(v1 + 1, v2 - v1 - 1);

        store[key] = {value, -1};
    }

    file.close();

    cout << "Snapshot loaded" << endl;
}

void cleanup() {
    while (true) {

        this_thread::sleep_for(chrono::seconds(1));

        lock_guard<mutex> lock(mtx);

        long long t = now();

        for (auto it = store.begin(); it != store.end();) {

            if (it->second.expiry != -1 &&
                it->second.expiry < t) {

                it = store.erase(it);
                expired_cleaned++;
            }
            else
                ++it;
        }
    }
}

int main() {

    thread cleaner(cleanup);
    cleaner.detach();

    string line;

    while (true) {

        if (!getline(cin, line))
            break;

        if (line.empty())
            continue;

        stringstream ss(line);

        string cmd;
        ss >> cmd;

        if (cmd == "SET") {

            string key, value, exWord;
            int ex = -1;

            ss >> key >> value;

            if (ss >> exWord && exWord == "EX")
                ss >> ex;

            SET(key, value, ex);
        }

        else if (cmd == "GET") {

            string key;
            ss >> key;

            GET(key);
        }

        else if (cmd == "DEL") {

            string key;
            ss >> key;

            DEL(key);
        }

        else if (cmd == "TTL") {

            string key;
            ss >> key;

            TTL(key);
        }

        else if (cmd == "KEYS") {

            string pattern;
            ss >> pattern;

            KEYS(pattern);
        }

        else if (cmd == "SAVE") {

            SAVE();
        }

        else if (cmd == "LOAD") {

            LOAD();
        }

        else if (cmd == "STATS") {

            STATS();
        }

        else if (cmd == "EXIT") {

            break;
        }

        else {

            cout << "Unknown command" << endl;
        }
    }

    return 0;
}