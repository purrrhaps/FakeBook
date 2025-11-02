#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H
#include <string>
#include <vector>
class User;

class Authenticator{
private:
    std::string fileName;
public:
    Authenticator(std::string _fileName);
    User* login(std::vector<User*>& userList);
    User* signUp(std::vector<User*>& userList);
};
#endif //AUTHENTICATOR_H
