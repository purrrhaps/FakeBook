#include "User.h"
#include "Post.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <algorithm>
#include <limits>
#include <set>

void clearCinUser() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

User::User(std::string uName, std::string uId, std::string _email, std::string _password, int _age,
           char _gender, std::string _location, bool _isPublicProfile, std::chrono::system_clock::time_point _createdAt)
    : userName(uName),
      userId(uId),
      email(_email),
      password(_password),
      age(_age),
      gender(_gender),
      location(_location),
      isPublicProfile(_isPublicProfile),
      createdAt(_createdAt) {
}

void User::viewOwnProfile() {
    std::cout << "\n--- Your Profile ---" << std::endl;
    std::cout << "Username: " << this->userName << " (ID: " << this->userId << ")" << std::endl;
    std::cout << "Email: " << this->email << std::endl;
    std::cout << "Location: " << this->location << std::endl;
    std::cout << "Age: " << this->age << "  Gender: " << this->gender << std::endl;
    std::cout << "Profile Status: " << (this->isPublicProfile ? "Public" : "Private") << std::endl;

    std::cout << "\n--- Your Friends (" << this->friends.size() << ") ---" << std::endl;
    for (User* friendUser : this->friends) {
        std::cout << "- " << friendUser->getUserName() << std::endl;
    }

    std::cout << "\n--- Your Posts (" << this->posts.size() << ") ---" << std::endl;
    for (Post* post : this->posts) {
        // post->displayPost();
        std::cout << "  [" << post->getPostId() << "] " << post->getContent() << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}

void User::viewOtherProfile(User* otherUser) {
    if (otherUser == nullptr) return;

    auto isFriend = [this](User* other) {
        for (User* f : this->friends) {
            if (f == other) return true;
        }
        return false;
    };

    bool canViewFullProfile = otherUser->isPublic() || isFriend(otherUser);

    std::cout << "\n--- " << otherUser->getUserName() << "'s Profile ---" << std::endl;
    std::cout << "Location: " << otherUser->location << std::endl;

    if (canViewFullProfile) {
        std::cout << "Age: " << otherUser->age << "  Gender: " << otherUser->gender << std::endl;
        std::cout << "\n--- " << otherUser->getUserName() << "'s Posts ---" << std::endl;

        for (Post* post : otherUser->getPosts()) {
            if (post->isPublic() || isFriend(otherUser)) {
                // post->displayPost();
                std::cout << "  [" << post->getPostId() << "] " << post->getContent() << std::endl;
            }
        }
    } else {
        std::cout << "\nThis profile is private and you are not friends." << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}

void User::changePrivacySetting() {
    char choice = ' ';
    while (choice != 'P' && choice != 'V') {
        std::cout << "Set your profile to (P)ublic or (V)rivate? ";
        std::cin >> choice;
        choice = toupper(choice);
        clearCinUser();
    }
    this->isPublicProfile = (choice == 'P');
    std::cout << "Your profile is now " << (this->isPublicProfile ? "Public." : "Private.") << std::endl;
}

Post* User::createPost() {
    std::string content;
    char privacyChoice = ' ';

    std::cout << "What's on your mind? (Enter your post content):" << std::endl;
    std::getline(std::cin, content);

    while (privacyChoice != 'P' && privacyChoice != 'F') {
        std::cout << "Set post visibility to (P)ublic or (F)riends Only? ";
        std::cin >> privacyChoice;
        privacyChoice = toupper(privacyChoice);
        clearCinUser();
    }
    bool isPostPublic = (privacyChoice == 'P');

    auto now = std::chrono::system_clock::now();
    long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string postId = "p" + std::to_string(timestamp);

    Post* newPost = new Post(this, content, now, isPostPublic, postId);
    this->addPost(newPost);

    std::cout << "Post created successfully!" << std::endl;
    return newPost;
}


struct PostComparator {
    bool operator()(Post* a, Post* b) {
        return a->getTimestamp() < b->getTimestamp();
    }
};

void User::viewFeed() {
    std::cout << "Building your home feed..." << std::endl;
    std::set<Post*> uniquePosts;
    for (User* friendUser : this->friends) {
        for (Post* post : friendUser->getPosts()) {
            uniquePosts.insert(post);
        }
    }

    for (User* friendUser : this->friends) {
        for (User* friendOfFriend : friendUser->getFriends()) {
            if (friendOfFriend == this) continue;
            for (Post* post : friendOfFriend->getPosts()) {
                if (post->isPublic()) {
                    uniquePosts.insert(post);
                }
            }
        }
    }

    if (uniquePosts.empty()) {
        std::cout << "Your feed is empty." << std::endl;
        return;
    }

    std::vector<Post*> feedPosts(uniquePosts.begin(), uniquePosts.end());
    std::make_heap(feedPosts.begin(), feedPosts.end(), PostComparator());
    std::sort_heap(feedPosts.begin(), feedPosts.end(), PostComparator());

    std::cout << "\n--- Your Home Feed (Newest First) ---" << std::endl;
    for (auto it = feedPosts.rbegin(); it != feedPosts.rend(); ++it) {
        Post* post = *it;
        // post->displayPost();
        std::cout << "--------------------" << std::endl;
        std::cout << "Post by: " << post->getAuthor()->getUserName() << std::endl;
        std::cout << post->getContent() << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}
