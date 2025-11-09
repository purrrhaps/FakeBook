#include "Post.h"
#include "User.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Post::Post(User* author, std::string _content, std::chrono::system_clock::time_point timeStamp, bool _isPublic, std::string _postId)
    : postId(_postId),
      authorId(author),
      content(_content),
      isPublicPost(_isPublic),
      timeUploaded(timeStamp)
{
}

void Post::displayPost() {
    std::time_t postTime_t = std::chrono::system_clock::to_time_t(this->timeUploaded);
    std::tm timeInfo = *std::localtime(&postTime_t);
    std::stringstream timeStream;
    timeStream << std::put_time(&timeInfo, "%Y-%m-%d %H:%M");
    std::string formattedTime = timeStream.str();

    std::cout << "--------------------" << std::endl;
    std::cout << "Post by: " << this->authorId->getUserName() << std::endl;
    std::cout << this->content << std::endl;
    std::cout << "Posted on: " << formattedTime << std::endl;
    std::cout << "Visibility: " << (this->isPublicPost ? "Public" : "Friends Only") << std::endl;
    std::cout << "--------------------" << std::endl;
}