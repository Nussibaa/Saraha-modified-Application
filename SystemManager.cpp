#include "SystemManager.h"
#include<fstream>
#include<iostream>
#include<tuple>
SystemManager::SystemManager() {

}
bool SystemManager::signup(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        return false; // Reject empty username or password
    }
    if (users.find(username) != users.end()) { //if the username is found, it returns an iterator not equal to end()
        return false; //username already exists
    }
    //emplace() makes pair in place, unlike insert which needs pre-made pairs
    users.emplace(username, User(nextId, username, password));
    nextId++;
    return true;
}

User* SystemManager::getUserByID(int& targetID) {
    for (auto& [username, user] : users) {
        if (user.getID() == targetID) {
            return &user;
        }
    }
    return nullptr;
}

bool SystemManager::login(const string& username, const string& password) const {
    auto it = users.find(username);
    if (it != users.end() && it->second.getPassword() == password) {
        return true; //login successful
    }
    return false; //username not found or password is incorrect
}

User* SystemManager::getUser(const string& username) {
    auto it = users.find(username);
    if (it != users.end()) {
        return &it->second;
    }
    return nullptr;
}

void SystemManager::saveAllDataToJson(const string& filename) {
    try {
        json j;
        json usersArray = json::array();

        for (auto& [username, user] : users) {
            json userJson;
            userJson["id"] = user.getID();
            userJson["username"] = user.getUsername();
            userJson["password"] = user.getPassword();

            // Save sent messages
            json sentMessagesArray = json::array();
            const auto& sentMessages = user.viewSentMessages();
            for (const auto& msgTuple : sentMessages) {
                json msgJson;
                msgJson["message"] = std::get<0>(msgTuple);
                msgJson["expirationTime"] = std::get<1>(msgTuple);
                msgJson["receiverId"] = std::get<2>(msgTuple);
                sentMessagesArray.push_back(msgJson);
            }
            userJson["sentMessages"] = sentMessagesArray;

            // Save contacts (received messages)
            json contactsArray = json::array();
            const auto& AllContacts = user.getAllContacts();
            for (const auto& [senderId, messages] : AllContacts) {
                json contactJson;
                contactJson["senderId"] = senderId;
                json messagesArray = json::array();
                for (const auto& [message, expirationTime] : messages) {
                    json msgJson;
                    msgJson["message"] = message;
                    msgJson["expirationTime"] = expirationTime;
                    messagesArray.push_back(msgJson);
                }
                contactJson["messages"] = messagesArray;
                contactsArray.push_back(contactJson);
            }
            userJson["contacts"] = contactsArray;

            // Save favorite messages
            json favoriteMessagesArray = json::array();
            queue<string> favorites = user.viewFavorites();
            while (!favorites.empty()) {
                favoriteMessagesArray.push_back(favorites.front());
                favorites.pop();
            }
            userJson["favoriteMessages"] = favoriteMessagesArray;

            // Save deleted messages
            json deletedMessagesArray = json::array();
            for (const auto& [deletedId, messages] : user.deletedIDmsgs) {
                json deletedJson;
                deletedJson["deletedId"] = deletedId;
                json messagesArray = json::array();
                for (const auto& [message, expirationTime] : messages) {
                    json msgJson;
                    msgJson["message"] = message;
                    msgJson["expirationTime"] = expirationTime;
                    messagesArray.push_back(msgJson);
                }
                deletedJson["messages"] = messagesArray;
                deletedMessagesArray.push_back(deletedJson);
            }
            userJson["deletedIDmsgs"] = deletedMessagesArray;

            // Save messageCounts
            json messageCountsArray = json::array();
            for (const auto& [contactId, count] : user.getMessageCounts()) {
                json countJson;
                countJson["contactId"] = contactId;
                countJson["count"] = count;
                messageCountsArray.push_back(countJson);
            }
            userJson["messageCounts"] = messageCountsArray;

            // Save sortedByMessages
            json sortedByMessagesArray = json::array();
            for (const auto& [msgCount, contactIds] : user.getSortedByMessages()) {
                json sortedJson;
                sortedJson["messageCount"] = msgCount;
                json idsArray = json::array();
                for (int id : contactIds) {
                    idsArray.push_back(id);
                }
                sortedJson["contactIds"] = idsArray;
                sortedByMessagesArray.push_back(sortedJson);
            }
            userJson["sortedByMessages"] = sortedByMessagesArray;

            usersArray.push_back(userJson);
        }

        j["users"] = usersArray;

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        file << j.dump(4);
        file.close();
    }
    catch (const std::exception& e) {
        cerr << "Error saving JSON: " << e.what() << endl;
    }
}

void SystemManager::loadFromJson(const string& filename) {
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        json j;
        file >> j;
        file.close();

        users.clear();
        users_id.clear();
        nextId = 1;

        for (const auto& userJson : j["users"]) {
            int id = userJson["id"].get<int>();
            string username = userJson["username"].get<string>();
            string password = userJson["password"].get<string>();

            User user(id, username, password);

            // Load sent messages
            if (userJson.contains("sentMessages")) {
                for (const auto& msgJson : userJson["sentMessages"]) {
                    string message = msgJson["message"].get<string>();
                    time_t expirationTime = msgJson["expirationTime"].get<time_t>();
                    int receiverId = msgJson["receiverId"].get<int>();
                    user.AddSentMessage(message, expirationTime, receiverId);
                }
            }

            // Load received messages (contacts)
            unordered_map<int, vector<pair<string, time_t>>> contacts;
            if (userJson.contains("contacts")) {
                for (const auto& contactJson : userJson["contacts"]) {
                    int senderId = contactJson["senderId"].get<int>();
                    vector<pair<string, time_t>> messages;
                    for (const auto& msgJson : contactJson["messages"]) {
                        string message = msgJson["message"].get<string>();
                        time_t expirationTime = msgJson["expirationTime"].get<time_t>();
                        messages.push_back({ message, expirationTime });
                        user.expirationTracker.insert({ expirationTime, {senderId, message} });
                    }
                    contacts[senderId] = messages;
                }
            }
            contacts = user.getAllContacts();
            // Load favorite messages
            if (userJson.contains("favoriteMessages")) {
                for (const auto& favMsg : userJson["favoriteMessages"]) {
                    user.addMessageToFav(favMsg.get<string>());
                }
            }

            // Load deleted messages
            if (userJson.contains("deletedIDmsgs")) {
                for (const auto& deletedJson : userJson["deletedIDmsgs"]) {
                    int deletedId = deletedJson["deletedId"].get<int>();
                    vector<pair<string, time_t>> messages;
                    for (const auto& msgJson : deletedJson["messages"]) {
                        string message = msgJson["message"].get<string>();
                        time_t expirationTime = msgJson["expirationTime"].get<time_t>();
                        messages.push_back({ message, expirationTime });
                    }
                    user.deletedIDmsgs[deletedId] = messages;
                }
            }

            // Load messageCounts
            unordered_map<int, int> messageCounts;
            if (userJson.contains("messageCounts")) {
                for (const auto& countJson : userJson["messageCounts"]) {
                    int contactId = countJson["contactId"].get<int>();
                    int count = countJson["count"].get<int>();
                    messageCounts[contactId] = count;
                }
            }

            // Load sortedByMessages
            map<int, set<int>> sortedByMessages;
            if (userJson.contains("sortedByMessages")) {
                for (const auto& sortedJson : userJson["sortedByMessages"]) {
                    int msgCount = sortedJson["messageCount"].get<int>();
                    set<int> contactIds;
                    for (const auto& id : sortedJson["contactIds"]) {
                        contactIds.insert(id.get<int>());
                    }
                    sortedByMessages[msgCount] = contactIds;
                }
            }

            // Set messageCounts and sortedByMessages
            user.setMessageCountsAndSorted(messageCounts, sortedByMessages);

            // Rebuild keysentmessages from sentMessages
            for (const auto& msgTuple : user.viewSentMessages()) {
                string msg = std::get<0>(msgTuple);
                time_t expirationTime = std::get<1>(msgTuple);
                int recipientId = std::get<2>(msgTuple);
                user.keysentmessages[recipientId].emplace_back(msg, expirationTime);
            }

            users.emplace(username, user);
            users_id.emplace(id, user);
            nextId = max(nextId, id + 1);
        }
    }
    catch (const std::exception& e) {
        cerr << "Error loading JSON: " << e.what() << endl;
    }
} 