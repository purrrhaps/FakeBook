#ifndef USER_H
#define USER_H
#include <string>
#include <list>
#include <vector>
#include <chrono>
class Post;

class User {
private:
    std::string userName;
    std::string email;
    std::string password;
    int age;
    char gender;
    std::string location;
    bool isPublicProfile;
    std::list<User*> friends;
    std::string userId;
    std::vector<Post*> posts;
    std::chrono::system_clock::time_point createdAt;
public:
    User(std::string uName, std::string uId, std::string email, std::string password, int _age,
         char _gender, std::string _location, bool _isPublicProfile, std::chrono::system_clock::time_point _createdAt);

    std::string getUserId() {
        return userId;
    }
    void addPost(Post* _post) {
        posts.push_back(_post);
    }
    void addFriend(User* friendUser) {
        friends.push_back(friendUser);
    }
    std::list<User*> getFriends() const {
        return friends;
    }
    Post* createPost();
    void changePrivacySetting();
    void sendRequest();
    User* acceptRequest();
    void declineRequest();
    void viewOwnProfile();
    void viewOtherProfile(User* other);
    void viewFeed(); // TODO: Min-heap sort.
};
#endif //USER_H
