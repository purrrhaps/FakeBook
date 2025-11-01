#ifndef USER_H
#define USER_H
#include <string>
#include <list>

class User {
private:
    std::string userName;
    std::string email;
    std::string password;
    int age;
    char gender;
    std::string location;
    bool isPrivate;
    std::list<User*> friends;
    std::string userId;
public:
    User();
    void createPost();
    void changePrivacySetting();
    void sendRequest();
    void declineRequest();
    void viewOwnProfile();
    void viewOtherProfile(User* other);
    void viewFeed();
};
#endif //USER_H
