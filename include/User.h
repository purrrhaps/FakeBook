#ifndef USER_H
#define USER_H
#include <string>
#include <list>
#include <vector>
class Post;

class User {
private:
    std::string userName;
    std::string email;
    std::string password;
    int age;
    char gender;
    std::string location;
    bool isPublic;
    std::list<User*> friends;
    std::string userId;
    std::vector<Post*> posts;
public:
    User();
    void createPost();
    void changePrivacySetting();
    void sendRequest();
    void declineRequest();
    void viewOwnProfile();
    void viewOtherProfile(User* other);
    void viewFeed(); // TODO: Min-heap sort.
};
#endif //USER_H
