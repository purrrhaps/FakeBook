#ifndef FAKEBOOK_H
#define FAKEBOOK_H
#include <fstream>
#include <vector>
class User;
class Post;

class FakeBook {
private:
    User* currentSession = nullptr;
    std::vector<User*> masterUserList;
    std::vector<Post*> masterPostList;
    User* idToPointer(std::string userId) const;
public:
    FakeBook();
    void runFakeBook();
    void parseAllUsers(); // this must happen before parseAllFriends() or parseAllPosts() are called.
    void parseAllFriends();
    void parseAllPosts();
    void appendFriend();
    void appendPost(Post *newPost);
    void saveAllFriendsToFile();
};
#endif //FAKEBOOK_H
