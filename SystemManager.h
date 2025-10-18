#pragma once

using namespace std;
#include <string>
#include <unordered_map>
#include "User.h"
#include "json.hpp"
#include<tuple>

using json = nlohmann::json;

class SystemManager {
private:
    unordered_map<string, User> users;//key username
    unordered_map<int, User>users_id;
    int nextId = 1;

public:
    SystemManager();
    bool signup(const string& username, const string& password);
    bool login(const string& username, const string& password) const;
    User* getUser(const string& username);
    User* getUserByID(int& targetID);
    void saveAllDataToJson(const string& filename = "users.json");
    void loadFromJson(const string& filename = "users.json");
};
