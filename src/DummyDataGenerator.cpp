#include "DummyDataGenerator.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

const int USER_COUNT = 20;
const int MAX_POSTS_PER_USER = 10;
const int TOTAL_POSTS = USER_COUNT * MAX_POSTS_PER_USER; // 200 posts at max
const std::string USERS_FILE = "DataStorage/Users.txt";

void fillStringPool(std::vector<std::string>& pool, const std::string& prefix, int count) {
    for (int i = 1; i <= count; ++i) {
        pool.push_back(prefix + std::to_string(i));
    }
}

DummyDataGenerator::DummyDataGenerator()
    : randomizer(std::chrono::system_clock::now().time_since_epoch().count())
{
    fillStringPool(userIdPool, "u", USER_COUNT);
    fillStringPool(usernamePool, "User", USER_COUNT);
    fillStringPool(emailPool, "user", USER_COUNT);
    fillStringPool(passwordPool, "Pass", USER_COUNT);
    fillStringPool(locationPool, "Country", USER_COUNT);
    fillStringPool(postIdPool, "p", TOTAL_POSTS);
    fillStringPool(contentPool, "This is post content no.", TOTAL_POSTS);

    // guarantees unique AND random values as all pools are iterated from start to end
    std::shuffle(userIdPool.begin(), userIdPool.end(), randomizer);
    std::shuffle(usernamePool.begin(), usernamePool.end(), randomizer);
    std::shuffle(emailPool.begin(), emailPool.end(), randomizer);
    std::shuffle(passwordPool.begin(), passwordPool.end(), randomizer);
    std::shuffle(locationPool.begin(), locationPool.end(), randomizer);
    std::shuffle(postIdPool.begin(), postIdPool.end(), randomizer);
    std::shuffle(contentPool.begin(), contentPool.end(), randomizer);
}

void DummyDataGenerator::populateUsers() {
    std::uniform_int_distribution ageDistribution(13, 120);
    std::uniform_int_distribution boolDistribution(0, 1);
    std::uniform_int_distribution<> indexDistribution(0, locationPool.size() - 1);

    std::ofstream userWriter(USERS_FILE);
    if (!userWriter) {
        std::cerr << "Error opening " << USERS_FILE << " for writing." << std::endl;
        return;
    }

    for (int i = 0; i < USER_COUNT; ++i) {
        int age = ageDistribution(randomizer);
        char gender = (boolDistribution(randomizer) == 0) ? 'M' : 'F';
        bool isPublic = (boolDistribution(randomizer) == 1);
        std::string location = locationPool[indexDistribution(randomizer)];

        std::uniform_int_distribution<> timeElapsedDistribution(0, 100000); // in seconds
        auto createdAt = std::chrono::system_clock::now() - std::chrono::seconds(timeElapsedDistribution(randomizer));

        // userId#username#email#password#location#gender#age#isPublic#createdAt
        userWriter << userIdPool[i] << "#" << usernamePool[i] << "#" << emailPool[i] << "@fakebook.com" << "#"
                 << passwordPool[i] << "#" << location << "#" << gender << "#" << age << "#"
                    << (isPublic ? "Public" : "Private") << "#"
                 << std::chrono::duration_cast<std::chrono::seconds>(createdAt.time_since_epoch()).count();

        userWriter << std::endl;
    }
    userWriter.close();
    std::cout << "Generated " << USER_COUNT << " users and saved to Users.txt." << std::endl;
}

void DummyDataGenerator::populateFriendsAndRequests() {
    std::ofstream friendWriter("DataStorage/Friends.txt");
    std::ofstream requestWriter("DataStorage/FriendRequests.txt");
    if (!friendWriter || !requestWriter) {
        std::cerr << "Error opening friends/requests files." << std::endl;
        return;
    }
    std::vector<std::vector<std::string>> adjacencyList(USER_COUNT); // to ensure mutuality.
    std::uniform_int_distribution<> friendProbability(1, 10);
    std::uniform_int_distribution<> pendingRequestProbability(1, 10);

    int friendCount = 0;
    int pendingRequestsCount = 0;

    for (int i = 0; i < USER_COUNT; ++i) {
        std::string currentUserId = userIdPool[i];
        for (int j = i + 1; j < USER_COUNT; ++j) {
            std::string potentialFriendId = userIdPool[j];
            if (friendProbability(randomizer) <= 3) {
                if (pendingRequestProbability(randomizer) == 1) {
                    auto timestamp = std::chrono::system_clock::now();
                    requestWriter << currentUserId << "#" << potentialFriendId << "#"
                                 << std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count() << "#"
                                 << "PENDING" << std::endl;
                    pendingRequestsCount++;
                } else {
                    adjacencyList[i].push_back(potentialFriendId);
                    adjacencyList[j].push_back(currentUserId);
                    friendCount++;
                }
            }
        }
    }
    for (int i = 0; i < USER_COUNT; ++i) {
        std::string userId = userIdPool[i];
        std::stringstream friendLine;
        friendLine << userId << ":";

        for (const std::string& friendId : adjacencyList[i])
            friendLine << friendId << ",";

        std::string finalFriendList = friendLine.str();
        if (finalFriendList.back() == ',')
            finalFriendList.pop_back();
        friendWriter << finalFriendList << std::endl;
    }
    friendWriter.close();
    requestWriter.close();
    std::cout << "Generated " << friendCount << " mutual friendships and " << pendingRequestsCount << " pending requests." << std::endl;
}

void DummyDataGenerator::populatePosts() {
    std::ofstream postWriter("DataStorage/Posts.txt");
    if (!postWriter) {
        std::cerr << "Error opening Posts.txt for writing." << std::endl;
        return;
    }
    std::uniform_int_distribution<> postsPerUser(0, MAX_POSTS_PER_USER);
    std::uniform_int_distribution<> privacyDistribution(0, 1);
    std::uniform_int_distribution<> contentIndex(0, contentPool.size() - 1);
    int postIndex = 0;
    int actualTotalPosts = 0;

    for (int i = 0; i < USER_COUNT; ++i) {
        int postCounter = postsPerUser(randomizer);
        std::string authorId = userIdPool[i];

        for (int j = 0; j < postCounter; ++j) {
            if (postIndex >= TOTAL_POSTS)
                break;

            std::string postId = postIdPool[postIndex];
            std::string content = contentPool[contentIndex(randomizer)];
            bool isPublic = (privacyDistribution(randomizer) == 1);

            std::uniform_int_distribution<> timeElapsed(0, 50000);
            auto timestamp = std::chrono::system_clock::now() - std::chrono::seconds(timeElapsed(randomizer));

           // postId#authorId#text#timestamp#visibility  for Posts.txt
            postWriter << postId << "#" << authorId << "#" << content << "#"
                      << std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count() << "#"
                      << (isPublic ? "Public" : "FriendsOnly") << std::endl;
            postIndex++;
            actualTotalPosts++;
        }
    }
    postWriter.close();
    std::cout << "Generated " << actualTotalPosts << " posts and saved to posts.txt." << std::endl;
}