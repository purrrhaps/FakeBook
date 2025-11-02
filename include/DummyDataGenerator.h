#ifndef DUMMYDATAGENERATOR_H
#define DUMMYDATAGENERATOR_H
#include <chrono>
#include <random>
#include <vector>

class DummyDataGenerator {
private:
    /* userId#username#email#password#location#gender#age#isPublic#createdAt  for User.txt
       userId:friendId1,friendId2,... for Friends.txt
       postId#authorId#text#timestamp#visibility  for Posts.txt
       fromUserId#toUserId#timestamp#status  for FriendRequests.txt*/

    // age, gender, isPublicProfile, isPublicPost can all be done within the methods and don't need their own pools
    std::mt19937 randomizer;
    std::vector<std::string> userIdPool; // can also work as authorId
    std::vector<std::string> usernamePool;
    std::vector<std::string> emailPool;
    std::vector<std::string> passwordPool;
    std::vector<std::string> locationPool;
    std::vector<std::chrono::system_clock::time_point> userCreatedAtPool;
    std::vector<std::chrono::system_clock::time_point> PostCreatedAtPool;
    std::vector<std::string> postIdPool;
    std::vector<std::string> contentPool;
public:
    DummyDataGenerator();
    void populateUsers();
    void populatePosts();
    void populateFriendsAndRequests();
};
#endif //DUMMYDATAGENERATOR_H
