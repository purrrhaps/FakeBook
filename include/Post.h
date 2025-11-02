#ifndef POST_H
#define POST_H
#include <chrono>
class User;

class Post {
private:
    std::string postId;
    User* authorId;
    std::string content;
    bool isPublicPost;
    std::chrono::system_clock::time_point timeUploaded;
public:
    Post(User* author, std::string _content, std::chrono::system_clock::time_point timeStamp, bool _isPublic, std::string postId);
    void displayPost();
    std::string getPostId() const { return postId; }
    User* getAuthor() const { return authorId; }
    std::string getContent() const { return content; }
    bool isPublic() const { return isPublicPost; }
    std::chrono::system_clock::time_point getTimestamp() const { return timeUploaded; }
};
#endif //POST_H
