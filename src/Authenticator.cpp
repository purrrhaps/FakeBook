#include "Authenticator.h"
#include "User.h"
#include <iostream>
#include <fstream>
#include <chrono>

void clearCinAuth() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

Authenticator::Authenticator(std::string _fileName) : fileName(_fileName) {}

User* Authenticator::login(std::vector<User*>& userList) {
    std::string email, password;

    std::cout << "Enter email: ";
    std::getline(std::cin, email);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    for (User* user : userList) {
        if (user->getEmail() == email && user->getPassword() == password) {
            return user;
        }
    }
    return nullptr;
}

User* Authenticator::signUp(std::vector<User*>& userList) {
    std::string uName, email, password, location;
    int age;
    char gender = ' ';
    char privacyChoice = ' ';
    bool isPublic;

    std::cout << "Enter new username: ";
    std::getline(std::cin, uName);
    std::cout << "Enter new email: ";
    std::getline(std::cin, email);
    std::cout << "Enter new password: ";
    std::getline(std::cin, password);
    std::cout << "Enter location: ";
    std::getline(std::cin, location);
    std::cout << "Enter age: ";
    while (!(std::cin >> age)) {
        std::cerr << "Invalid age. Please enter a number: ";
        std::cin.clear();
        clearCinAuth();
    }
    clearCinAuth();

    while (gender != 'M' && gender != 'F') {
        std::cout << "Enter gender (M/F): ";
        std::cin >> gender;
        gender = toupper(gender);
        clearCinAuth();
    }
    
    while (privacyChoice != 'P' && privacyChoice != 'V') {
        std::cout << "Profile privacy (P = Public, V = Private): ";
        std::cin >> privacyChoice;
        privacyChoice = toupper(privacyChoice);
        clearCinAuth();
    }
    isPublic = (privacyChoice == 'P');

    for (User* user : userList) {
        if (user->getEmail() == email) {
            std::cout << "This email is already taken." << std::endl;
            return nullptr;
        }
    }
    std::string uId = "u" + std::to_string(userList.size() + 1);
    auto createdAt = std::chrono::system_clock::now();
    long long timestampSeconds = std::chrono::duration_cast<std::chrono::seconds>(createdAt.time_since_epoch()).count();

    std::ofstream userFile(fileName, std::ios::app);
    if (!userFile.is_open()) {
        std::cerr << "Error: Could not open " << fileName << " for appending." << std::endl;
        return nullptr;
    }

    // Format: userId#username#email#password#location#gender#age#isPublic#createdAt
    userFile << uId << "#" << uName << "#" << email << "#" << password << "#" << location << "#" << gender << "#"
             << age << "#" << (isPublic ? "Public" : "Private") << "#"<< timestampSeconds << std::endl;
    userFile.close();

    User* newUser = new User(
        uName, uId, email, password, age, gender, location, isPublic, createdAt
    );
    userList.push_back(newUser);
    return newUser;
}