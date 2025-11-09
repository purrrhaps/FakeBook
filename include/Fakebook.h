#ifndef FAKEBOOK_H
#define FAKEBOOK_H

#include <vector>
#include <string>

class User;
class Post;

class FakeBook {
private:
    User* currentSession = nullptr;
    std::vector<User*> masterUserList;
    std::vector<Post*> masterPostList;

    User* idToPointer(std::string userId) const;
    User* usernameToPointer(const std::string& username) const;
    void saveAllFriendsToFile();
    void handleSendRequest();
    void handleRespondRequests();
    void handleRemoveFriend();

public:
    FakeBook();
    void runFakeBook();
    void parseAllUsers();
    void parseAllFriends();
    void parseAllPosts();
    void appendFriend();
    void appendPost(Post *newPost);
};
#endif //FAKEBOOK_H