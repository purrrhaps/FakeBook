#ifndef POST_H
#define POST_H
#include <chrono>
class User;

class Post {
private:
    std::string postId;
    User* authorId;
    std::string content;
    bool isPublic;
    std::chrono::system_clock::time_point timeUploaded;
public:
    Post(User* author, std::string _content, std::chrono::system_clock::time_point timeStamp, bool _isPublic, std::string postId);
    void displayPost();
};
#endif //POST_H
