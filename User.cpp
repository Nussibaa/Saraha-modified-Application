#include "User.h"
#include<string>
#include<stack>
#include <iostream>
#include <fstream>
#include<ctime>
#include <algorithm>
#include <sstream>
#include <tuple> 
#include "SystemManager.h"
using namespace std;

User::User(int id, const string& username, const string& password)
	: id(id), username(username), password(password) {
}

int User::getID() const {
	return id;
}

string User::getUsername() const {
	return username;
}

string User::getPassword() const {
	return password;
}


vector<pair<string, time_t>> User::viewReceivedMessages(int id) {
	vector<pair<string, time_t>> result;
	auto it1 = contacts.find(id);
	result = it1->second;
	reverse(result.begin(), result.end());
	return result;
}




string User::DisplaySortContactsByMessages() {//nussiba
	if (messageCounts.empty()) {
		return "No Contacts Found!";
	}
	stringstream ss;
	for (const auto& pair : sortedByMessages) {
		int messageCount = -pair.first;
		for (int contactID : pair.second) {
			ss << "Id: " << contactID << "  Number Of Messages: " << messageCount << "\n";
		}
	}
	return ss.str();
}

string User::ReceiveMessage(const int SenderId, const string& message, time_t expirationTime) {//nussiba
	if (message.empty() || SenderId <= 0) {
		return "Invalid sender ID or empty message.";
	}
	// Store message even if sender is not an added contact
	contacts[SenderId].push_back({ message, expirationTime });
	expirationTracker.insert({ expirationTime, {SenderId, message} });

	// Only sort if sender is an explicitly added contact
	auto it = messageCounts.find(SenderId);
	if (it != messageCounts.end()) {
		SortContacts(SenderId, "Add");
	}
	return "Message received from ID " + to_string(SenderId) + " and stored.";
}


string User::searchContact(int contactID) const {//maria
	auto it = messageCounts.find(contactID);
	if (it == messageCounts.end()) { //contact id not found
		return "CONTACT NOT FOUND!";
	}
	else {
		return "Contact exists.";
	}
}

string User::getTimestamp() {//mariam
	time_t now = time(0);
	struct tm timeinfo;
	char buf[80];

	localtime_s(&timeinfo, &now); // secure version for Windows
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);

	return buf;
}

string User::sendMessage(const int& id, const string& msg, int expirationHours) {//mariam
	if (msg.empty() || id <= 0) {
		return "ID and message can't be empty.\n";
	}
	time_t expirationTime = time(nullptr) + expirationHours * 3600;
	sentMessages.push_back(make_tuple(msg, expirationTime, id));
	keysentmessages[id].push_back(make_pair(msg, expirationTime));
	auto it = messageCounts.find(id);
	if (it != messageCounts.end()) {
		SortContacts(id, "Add");
	}
	return "Message is sent and will expire in " + to_string(expirationHours) + " hours.\n";
}

string User::undoMessage() {//mariam
	if (sentMessages.empty()) {
		return "No message to undo.\n";
	}
	// Get the last sent message
	auto lastMessage = sentMessages.back();
	int receiverId = get<2>(lastMessage);
	string messageText = get<0>(lastMessage);
	time_t expirationTime = get<1>(lastMessage);

	// Remove from sentMessages
	sentMessages.pop_back();

	// Remove from keysentmessages
	auto it = keysentmessages.find(receiverId);
	if (it != keysentmessages.end()) {
		auto& messages = it->second;
		for (auto msgIt = messages.begin(); msgIt != messages.end(); ++msgIt) {
			if (msgIt->first == messageText && msgIt->second == expirationTime) {
				messages.erase(msgIt);
				break;
			}
		}
		if (messages.empty()) {
			keysentmessages.erase(it);
		}
		// Update sorting if receiver is an added contact
		auto countIt = messageCounts.find(receiverId);
		if (countIt != messageCounts.end()) {
			SortContacts(receiverId, "Delete");
		}
	}

	return "Last message undone.\n";
}

unordered_map<int, vector<pair<string, time_t>>> User::getContacts() const {
	return contacts;
}

vector<tuple<string, time_t, int>>& User::viewSentMessages() {
	return sentMessages;
}

const unordered_map<int, vector<pair<string, time_t>>>& User::getAllContacts() const {
	return contacts;
}

string User::addMessageToFav(const string& message) {//menna
	if (message.empty()) {
		return "Error: Cannot add an empty message to favorites.";
	}
	favoriteMessages.push(message);
	return "Message added to favorites successfully!";
}

queue<string> User::viewFavorites() const {
	return favoriteMessages;
}

string User::removeOldestFav() {//menna
	if (favoriteMessages.empty()) {
		return "No favorite messages to remove. Your favorites list is empty.";
	}
	string oldestMessage = favoriteMessages.front();
	favoriteMessages.pop();
	return "Oldest favorite message removed successfully!";
}

// Check if the queue is empty
bool User::isEmpty() {
	return favoriteMessages.empty();
}

void User::removeExpiredMessages() {//rawan
	time_t now = time(0);
	while (!expirationTracker.empty() && expirationTracker.begin()->first <= now) {
		time_t expTime = expirationTracker.begin()->first;
		int userId = expirationTracker.begin()->second.first;
		string message = expirationTracker.begin()->second.second;

		for (auto it = sentMessages.begin(); it != sentMessages.end();) {
			if (get<0>(*it) == message && get<1>(*it) == expTime) {
				it = sentMessages.erase(it);
			}
			else {
				++it;
			}
		}

		auto contactIt = contacts.find(userId);
		if (contactIt != contacts.end()) {
			auto& messages = contactIt->second;
			for (auto it = messages.begin(); it != messages.end();) {
				if (it->first == message && it->second == expTime) {
					it = messages.erase(it);
					SortContacts(userId, "Delete");
				}
				else {
					++it;
				}
			}
			if (messages.empty()) {
				contacts.erase(contactIt);
			}
		}

		expirationTracker.erase(expirationTracker.begin());
	}
}


string User::RemoveContact(int id) {//rawan
	auto it = contacts.find(id);
	if (it != contacts.end()) {
		deletedIDmsgs[id] = move(it->second);
		int count = messageCounts[id];
		if (count > 0) {
			sortedByMessages[count].erase(id);
			if (sortedByMessages[count].empty()) {
				sortedByMessages.erase(count);
			}
		}
		messageCounts.erase(id);//will remove it from contacts
		return "The Contact has been deleted Successfully =) ";
	}
	else {
		return "The Contact isn't found !!!";
	}
}


bool User::addContact(int ContactId) {//habiba
	auto it = contacts.find(ContactId);
	if (it == contacts.end()) {
		return false; // Contact doesn’t exist (no messages received)
	}
	auto it2 = messageCounts.find(ContactId);
	if (it2 == messageCounts.end()) {
		int counts = contacts[ContactId].size() + keysentmessages[ContactId].size();
		messageCounts[ContactId] = counts;
		sortedByMessages[counts].insert(ContactId);
		return true;
	}
	return false; // Contact already added
}

vector<pair<string, time_t>> User::viewDeletedMessages(int id) {//rawan
	auto it = deletedIDmsgs.find(id);
	if (it != deletedIDmsgs.end()) {
		return it->second;
	}
	return {}; // Return empty vector if no messages exist
}


void User::SortContacts(int ContactId, string Case) {//nussiba
	int oldCount = messageCounts[ContactId];//Find Old Count first
	if (sortedByMessages.find(oldCount) != sortedByMessages.end()) {
		sortedByMessages[oldCount].erase(ContactId);
		if (sortedByMessages[oldCount].empty()) {
			//ensure that the map entry for -oldCount is only erased if its associated set<int> is empty
			sortedByMessages.erase(-oldCount);
		}
	}
	if (Case == "Delete") {
		int newCount = (--messageCounts[ContactId]);
		if (messageCounts[ContactId] > 0) {
			sortedByMessages[newCount].insert(ContactId);
		}
		else {
			messageCounts.erase(ContactId);
		}
	}
	else {
		int newCount = (++messageCounts[ContactId]);
		sortedByMessages[newCount].insert(ContactId);
	}
}

string User::removeReceivedMessage(int senderId, const string& message, time_t expirationTime) {//mariam
	auto it = contacts.find(senderId);
	if (it == contacts.end()) {
		return "No messages from sender ID " + to_string(senderId) + ".\n";
	}
	auto& messages = it->second;
	for (auto msgIt = messages.begin(); msgIt != messages.end(); ++msgIt) {
		if (msgIt->first == message && msgIt->second == expirationTime) {
			messages.erase(msgIt);
			// Remove from expirationTracker
			expirationTracker.erase({ expirationTime, {senderId, message} });
			// Update sorting if sender is an added contact
			auto countIt = messageCounts.find(senderId);
			if (countIt != messageCounts.end()) {
				SortContacts(senderId, "Delete");
			}
			if (messages.empty()) {
				contacts.erase(it);
			}
			return "Message removed from sender ID " + to_string(senderId) + ".\n";
		}
	}
	return "Message not found from sender ID " + to_string(senderId) + ".\n";
}