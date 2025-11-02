#include "DummyDataGenerator.h"
int main() {
    DummyDataGenerator gen;
    gen.populateUsers();
    gen.populateFriendsAndRequests();
    gen.populatePosts();
    return 0;
}