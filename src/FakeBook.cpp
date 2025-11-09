#include "Fakebook.h"
#include "User.h"
#include "Post.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <limits>
#include "Authenticator.h"
#include "DummyDataGenerator.h"

const std::string USERS_FILE_PATH = "DataStorage/Users.txt";
const std::string FRIENDS_FILE_PATH = "DataStorage/Friends.txt";
const std::string POSTS_FILE_PATH = "DataStorage/Posts.txt";
const std::string REQUESTS_FILE_PATH = "DataStorage/FriendRequests.txt";

User* FakeBook::usernameToPointer(const std::string& username) const {
    for (User* user : masterUserList) {
        if (user->getUserName() == username) {
            return user;
        }
    }
    return nullptr;
}

void clearCin() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

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

void FakeBook::appendPost(Post* newPost) {
    std::ofstream postWriter(POSTS_FILE_PATH, std::ios::app);
    if (!postWriter.is_open()) {
        std::cerr << "Error: opening " << POSTS_FILE_PATH << " for appending." << std::endl;
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

void FakeBook::handleSendRequest() {
    std::cout << "Enter username of person to send request to: ";
    std::string username;
    std::getline(std::cin, username);

    User* targetUser = usernameToPointer(username);
    if (targetUser == nullptr) {
        std::cout << "User not found." << std::endl;
        return;
    }
    if (targetUser == currentSession) {
        std::cout << "You can't send a friend request to yourself." << std::endl;
        return;
    }

    std::ofstream reqFile(REQUESTS_FILE_PATH, std::ios::app);
    if (!reqFile) {
        std::cerr << "Error opening requests file for appending." << std::endl;
        return;
    }

    auto now = std::chrono::system_clock::now();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // Format: fromUserId#toUserId#timestamp#status
    reqFile << currentSession->getUserId() << "#"
            << targetUser->getUserId() << "#"
            << timestamp << "#"
            << "PENDING" << std::endl;

    reqFile.close();
    std::cout << "Friend request sent to " << username << "." << std::endl;
}

void FakeBook::handleRespondRequests() {
    std::cout << "Loading your pending friend requests..." << std::endl;
    std::ifstream reqFileIn(REQUESTS_FILE_PATH);
    if (!reqFileIn) {
        std::cout << "No pending requests." << std::endl;
        return;
    }
    std::vector<std::string> remainingRequests;
    std::string line;
    bool foundRequests = false;

    while (std::getline(reqFileIn, line)) {
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> fields;
        while (std::getline(ss, segment, '#')) {
            fields.push_back(segment);
        }
        if (fields.size() != 4 || fields[3] != "PENDING") {
            remainingRequests.push_back(line);
            continue;
        }

        std::string fromId = fields[0];
        std::string toId = fields[1];

        if (toId == currentSession->getUserId()) {
            foundRequests = true;
            User* sender = idToPointer(fromId);
            if (sender == nullptr) {
                std::cout << "A request from an unknown user was found and ignored." << std::endl;
                continue;
            }

            std::cout << "\nFriend request from: " << sender->getUserName() << std::endl;
            std::cout << "Accept (A), Decline (D), or Ignore (I)? ";
            char choice;
            std::cin >> choice;
            clearCin();
            choice = toupper(choice);

            if (choice == 'A') {
                currentSession->addFriend(sender);
                sender->addFriend(currentSession);
                appendFriend();
                std::cout << "You are now friends with " << sender->getUserName() << "." << std::endl;
            } else if (choice == 'D') {
                std::cout << "Request declined." << std::endl;
            } else {
                remainingRequests.push_back(line);
            }
        } else {
            remainingRequests.push_back(line);
        }
    }
    reqFileIn.close();

    if (!foundRequests) {
        std::cout << "You have no pending friend requests." << std::endl;
    }
    std::ofstream reqFileOut(REQUESTS_FILE_PATH, std::ios::out);
    for (const std::string& req : remainingRequests) {
        reqFileOut << req << std::endl;
    }
    reqFileOut.close();
}

void FakeBook::handleRemoveFriend() {
    std::cout << "Your current friends:" << std::endl;
    std::list<User*> friends = currentSession->getFriends();
    if (friends.empty()) {
        std::cout << "You have no friends to remove." << std::endl;
        return;
    }
    for (User* f : friends) {
        std::cout << "- " << f->getUserName() << std::endl;
    }
    std::cout << "\nEnter username of friend to remove: ";
    std::string username;
    std::getline(std::cin, username);

    User* targetUser = usernameToPointer(username);
    if (targetUser == nullptr) {
        std::cout << "User not found." << std::endl;
        return;
    }

    bool isFriend = false;
    for (User* f : friends) {
        if (f == targetUser) {
            isFriend = true;
            break;
        }
    }

    if (!isFriend) {
        std::cout << "That user is not on your friends list." << std::endl;
        return;
    }
    currentSession->removeFriend(targetUser);
    targetUser->removeFriend(currentSession);

    appendFriend();
    std::cout << "Removed " << username << " from your friends list." << std::endl;
}

void FakeBook::runFakeBook() {
    bool isRunning = true;
    int choice = 0;
    Authenticator auth("DataStorage/Users.txt");
    do {
        if (currentSession == nullptr) {
            std::cout << "\n============================= Welcome to FakeBook ==============================" << std::endl;
            std::cout << "0. Generate Dummy Data" << std::endl;
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
                case 0: {
                    std::cout << "Generating dummy data... This may take a moment." << std::endl;
                    DummyDataGenerator generator;
                    generator.populateUsers();
                    generator.populateFriendsAndRequests();
                    generator.populatePosts();
                    std::cout << "Dummy data generation complete!" << std::endl;

                    std::cout << "Reloading all data..." << std::endl;
                    masterUserList.clear();
                    masterPostList.clear();
                    parseAllUsers();
                    parseAllFriends();
                    parseAllPosts();
                    std::cout << "Data reloaded." << std::endl;
                    break;
                }
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
            std::cout << "3. View Another User's Profile" << std::endl;
            std::cout << "4. Create Post" << std::endl;
            std::cout << "5. Send Friend Request" << std::endl;
            std::cout << "6. Respond to Pending Requests" << std::endl;
            std::cout << "7. Remove Friend" << std::endl;
            std::cout << "8. Change Privacy Setting" << std::endl;
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
                    currentSession->viewFeed();
                    break;
                case 2:
                    currentSession->viewOwnProfile();
                    break;
                case 3: {
                    std::cout << "Enter username to view: ";
                    std::string username;
                    std::getline(std::cin, username);
                    User* targetUser = usernameToPointer(username);
                    if (targetUser) {
                        currentSession->viewOtherProfile(targetUser);
                    } else {
                        std::cout << "User not found." << std::endl;
                    }
                    break;
                }
                case 4: {
                    Post* newPost = currentSession->createPost();
                    if (newPost) {
                        masterPostList.push_back(newPost);
                        appendPost(newPost);
                    }
                    break;
                }
                case 5:
                    handleSendRequest();
                    break;
                case 6:
                    handleRespondRequests();
                    break;
                case 7:
                    handleRemoveFriend();
                    break;
                case 8:
                    currentSession->changePrivacySetting();
                    break;
                case 9:
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