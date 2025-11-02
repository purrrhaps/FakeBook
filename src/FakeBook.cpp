#include "Fakebook.h"
#include "User.h"
#include "Post.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include "Authenticator.h"

const std::string USERS_FILE_PATH = "DataStorage/Users.txt";
const std::string FRIENDS_FILE_PATH = "DataStorage/Friends.txt";
const std::string POSTS_FILE_PATH = "DataStorage/Posts.txt";

User* FakeBook::idToPointer(std::string _userId) const {
    for (auto i : masterUserList) {
        if (i->getUserId() == _userId) {
            return i;
        }
    }
    return nullptr;
}

void FakeBook::parseAllUsers() {
    std::ifstream userReader(USERS_FILE_PATH);
    if (!userReader) {
        std::cerr << "Error opening " << USERS_FILE_PATH << " for reading." << std::endl;
        return;
    }
    std::string line;
    while (std::getline(userReader, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> fields;

        while (std::getline(ss, segment, '#')) {
            fields.push_back(segment);
        }
        // userId#username#email#password#location#gender#age#isPublic#createdAt
        if (fields.size() != 9) {
            std::cerr << "Warning: Skipping malformed user line with " << fields.size() << " fields: " << line << std::endl;
            continue;
        }
        std::string uId = fields[0];
        std::string uName = fields[1];
        std::string email = fields[2];
        std::string password = fields[3];
        std::string location = fields[4];

        char gender = fields[5][0];
        int age = std::stoi(fields[6]);
        bool isPublic = (fields[7] == "Public");

        long long timestampSeconds = std::stoll(fields[8]);
        auto createdAt = std::chrono::system_clock::time_point(std::chrono::seconds(timestampSeconds));

        User* newUser = new User(uName, uId, email, password, age, gender, location, isPublic, createdAt);
        masterUserList.push_back(newUser);
    }
    userReader.close();
    std::cout << "Successfully loaded " << masterUserList.size() << " users into memory." << std::endl;
}

void FakeBook::parseAllFriends(){
    if (masterUserList.empty()) {
        std::cerr << "Cannot parse friends. User list is empty." << std::endl;
        return;
    }
    std::ifstream friendReader(FRIENDS_FILE_PATH);
    if (!friendReader) {
        std::cerr << "Error opening " << FRIENDS_FILE_PATH << " for reading." << std::endl;
        return;
    }
    std::string line;
    int links = 0;

    while (std::getline(friendReader, line)) {
        if (line.empty())
            continue;
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;
        std::string ownerId = line.substr(0, colonPos);
        std::string friendListStr = line.substr(colonPos + 1);

        User* owner = idToPointer(ownerId);
        if (owner == nullptr) {
            std::cerr << "Undefined user: " << ownerId << std::endl;
            continue;
        }
        if (friendListStr.empty())
            continue;

        std::stringstream ss(friendListStr);
        std::string friendId;
        while (std::getline(ss, friendId, ',')) {
            if (friendId.empty())
                continue;

            User* friendUser = idToPointer(friendId);
            if (friendUser == nullptr) {
                std::cerr << "Friend ID not found: " << friendId << ". Skipping link." << std::endl;
                continue;
            }
            owner->addFriend(friendUser);
            links++;
        }
    }
    friendReader.close();
    std::cout << "Successfully established " << links << " links." << std::endl;
}

void FakeBook::parseAllPosts() {
    if (masterUserList.empty()) {
        std::cerr << "Cannot parse posts. User list is empty. Run parseAllUsers() first." << std::endl;
        return;
    }
    std::ifstream postReader(POSTS_FILE_PATH);
    if (!postReader) {
        std::cerr << "Error opening " << POSTS_FILE_PATH << " for reading." << std::endl;
        return;
    }
    std::string line;
    while (std::getline(postReader, line)) {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> fields;

        while (std::getline(ss, segment, '#')) {
            fields.push_back(segment);
        }
        // postId#authorId#text#timestamp#visibility
        if (fields.size() != 5) {
            std::cerr << "Warning: Skipping malformed post line: " << line << std::endl;
            continue;
        }
        std::string postId = fields[0];
        std::string authorId = fields[1];
        std::string content = fields[2];
        long long timestampSeconds = std::stoll(fields[3]);
        auto timeStamp = std::chrono::system_clock::time_point(std::chrono::seconds(timestampSeconds));
        bool isPublic = (fields[4] == "Public");

        User* author = idToPointer(authorId);
        if (author == nullptr) {
            std::cerr << "Warning: Skipping post " << postId << ". Author ID not found." << std::endl;
            continue;
        }
        Post* newPost = new Post(author, content, timeStamp, isPublic, postId);
        masterPostList.push_back(newPost);
        author->addPost(newPost);
    }
    postReader.close();
    std::cout << "Successfully loaded " << masterPostList.size() << " posts into memory." << std::endl;
}
FakeBook::FakeBook() {
    parseAllUsers();
    parseAllFriends();
    parseAllPosts();
}

void clearCin() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void FakeBook::runFakeBook() {
    bool isRunning = true;
    int choice = 0;
    Authenticator auth("DataStorage/Users.txt");
    do {
        if (currentSession == nullptr) {
            std::cout << "\n============================= Welcome to FakeBook ==============================" << std::endl;
            std::cout << "1. Login" << std::endl;
            std::cout << "2. Sign Up" << std::endl;
            std::cout << "3. Quit" << std::endl;
            std::cout << "Enter your choice: ";

            if (!(std::cin >> choice)) {
                std::cerr << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear();
                clearCin();
                continue;
            }
            clearCin();
            switch (choice) {
                case 1:
                    currentSession = auth.login(masterUserList);
                    if (currentSession == nullptr)
                        std::cout << "Login failed. Please check your email and password." << std::endl;
                     else
                        std::cout << "Login successful! Welcome." << std::endl;
                    break;
                case 2:
                    currentSession = auth.signUp(masterUserList);
                    if (currentSession != nullptr)
                        std::cout << "Sign up successful! You are now logged in." << std::endl;
                    else
                        std::cout << "Account already exits." << std::endl;
                    break;
                case 3:
                    isRunning = false;
                    std::cout << "Terminating FakeBook.exe" << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
            }
        } else {
            std::cout << "\n============================= FakeBook Home ===================================" << std::endl;
            std::cout << "1. View Home Feed" << std::endl;
            std::cout << "2. View My Profile" << std::endl;
            std::cout << "3. Create Post" << std::endl;
            // TODO: add remaining choices when implementation is complete.
            std::cout << "9. Logout" << std::endl;
            std::cout << "Enter your choice: ";
            if (!(std::cin >> choice)) {
                std::cerr << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear();
                clearCin();
                continue;
            }
            clearCin();

            switch (choice) {
                case 1:
                    std::cout << "[Action] Calling viewFeed()..." << std::endl;
                    // currentSession->viewFeed(); // TODO: Implement this method.
                    break;
                case 2:
                    std::cout << "[Action] Calling viewOwnProfile()..." << std::endl;
                    // currentSession->viewOwnProfile(); // TODO: Implement this method.
                    break;
                case 3:
                    std::cout << "[Action] Calling createPost()..." << std::endl;
                    // currentSession->createPost(); // TODO: Implement this method.
                    break;
                case 9: // Logout
                    currentSession = nullptr;
                    std::cout << "You have been logged out." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
            }
        }
    } while (isRunning);
}

void FakeBook::appendPost(Post* newPost) {
    std::ofstream postWriter(POSTS_FILE_PATH, std::ios::app);
    if (!postWriter.is_open()) {
        std::cerr << "Erroring opening " << POSTS_FILE_PATH << " for appending." << std::endl;
        return;
    }

    std::string postId = newPost->getPostId();
    std::string authorId = newPost->getAuthor()->getUserId();
    std::string content = newPost->getContent();
    bool isPublic = newPost->isPublic();
    auto timestamp = newPost->getTimestamp();
    long long timestampSeconds = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();

    postWriter << postId << "#" << authorId << "#"<< content << "#" <<
        timestampSeconds << "#" << (isPublic ? "Public" : "FriendsOnly") << std::endl;
    postWriter.close();
}

void FakeBook::saveAllFriendsToFile() {
    std::ofstream friendWriter(FRIENDS_FILE_PATH, std::ios::out);
    if (!friendWriter) {
        std::cerr << "Error opening " << FRIENDS_FILE_PATH << " for saving." << std::endl;
        return;
    }
    for (User* user : masterUserList) {
        std::string line = user->getUserId() + ":";
        std::list<User*> friends = user->getFriends();

        for (User* friendUser : friends)
            line += friendUser->getUserId() + ",";

        if (line.back() == ',')
            line.pop_back();
        friendWriter << line << std::endl;
    }
    friendWriter.close();
}

void FakeBook::appendFriend() {
    saveAllFriendsToFile();
    std::cout << "Friendships saved to file." << std::endl;
}