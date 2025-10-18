#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <set>
#include <ctime>
#include <unordered_set>
#include <map>
#include <tuple>

using namespace std;

class User {
private:
    int id;
    string username;
    string password;
    //!!!!!!!!!CONTACTS IS NOT THE USER CONTACTS IT IS THE RECEIVED MESSAGES FROM THIS CONTACT !!!!!!!!!!!1
    unordered_map<int, vector<pair<string, time_t>>> contacts; // {contactID, messages}->received messages from contacts
    queue<string> favoriteMessages;    
    unordered_map<int, int> messageCounts;                     // {contactID, count}
    map<int, set<int>> sortedByMessages;                       // {-messageCount, {contactIDs}}

public:
    unordered_map<int, vector<pair<string, time_t>>> deletedIDmsgs;
    unordered_map<int, vector<pair<string, time_t>>> keysentmessages;
    vector<tuple<string, time_t, int>> sentMessages; // {messageText, expirationTime, receiverID}
    set<pair<time_t, pair<int, string>>> expirationTracker;  // {expirationTime, {contactID, message}}

    // Constructors
    User();
    User(int id, const string& username, const string& password);

    // Getters
    int getID() const;
    string getUsername() const;
    string getPassword() const;

    // Message management
    vector<pair<string, time_t>> viewReceivedMessages(int id);
    string sendMessage(const int& id, const string& msg, int expirationHours);
    string undoMessage(); // Removed SystemManager parameter
    string ReceiveMessage(const int SenderId, const string& message, time_t expirationTime);
    void removeExpiredMessages();
    vector<tuple<string, time_t, int>>& viewSentMessages();
    unordered_map<int, vector<pair<string, time_t>>> getContacts() const;
    const unordered_map<int, vector<pair<string, time_t>>>& getAllContacts() const;

    // Contact management
    bool addContact(int SenderId);
    string RemoveContact(int id);
    string searchContact(int contactID)const;
    vector<pair<string, time_t>> viewDeletedMessages(int id);

    // Sorting and display
    string DisplaySortContactsByMessages();
    void SortContacts(int ContactId, string Case);

    // Favorites
    string addMessageToFav(const string& message);
    queue<string> viewFavorites() const;
    string removeOldestFav();
    bool isEmpty();

    // Timestamp
    string getTimestamp();

    // Optional helper to add message manually
    void AddSentMessage(const string& message, time_t expiration, int receiverId) {
        sentMessages.push_back(make_tuple(message, expiration, receiverId));
    }

    // New helper to remove a specific received message
    string removeReceivedMessage(int senderId, const string& message, time_t expirationTime);

    // Accessors for messageCounts and sortedByMessages
    const unordered_map<int, int>& getMessageCounts() const { return messageCounts; }
    const map<int, set<int>>& getSortedByMessages() const { return sortedByMessages; }
    void setMessageCountsAndSorted(const unordered_map<int, int>& counts, const map<int, set<int>>& sorted) {
        messageCounts = counts;
        sortedByMessages = sorted;
    }
};