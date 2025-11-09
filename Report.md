# FakeBook C++ Project Report

## 1. Introduction

This report details the implementation of FakeBook, a simplified social networking application built in C++. The program simulates a complete social environment, including user authentication, a mutual friendship system, text-based posts, and a chronologically sorted home feed.

All data is persisted to disk using text files, and all in-memory data is loaded and linked at runtime. The design follows an object-oriented approach, separating responsibilities into five core classes: `User`, `Post`, `FakeBook` (the main manager), `Authenticator`, and `DummyDataGenerator`.

## 2. Data Structures Used

The selection of data structures was critical for balancing project requirements, efficiency, and data integrity.

### `std::list<User*>` (Friends List)

* **Used In:** `User::friends`
* **Justification:** The project document explicitly required that "Friend relationships should be implemented using a linked list data structure." The `std::list` is the C++ standard library's implementation of a doubly linked list.
* **Analysis:** This choice provides highly efficient $O(1)$ insertion and removal of friends, which is ideal for the `addFriend()` and `removeFriend()` operations. Its main drawback is the $O(N)$ traversal time required to find a specific friend, but for this project's scale, it was a perfectly suitable trade-off to meet the requirement.

### `std::vector<User*>` (Master User List)

* **Used In:** `FakeBook::masterUserList`
* **Justification:** The `FakeBook` class acts as the central manager for the application's entire in-memory state. A `std::vector` was chosen to store the pointers to all `User` objects loaded from `Users.txt`.
* **Analysis:** This vector provides a single, canonical source of all user data. It allows us to implement a simple $O(N)$ lookup function (`idToPointer`) to find any user by their ID

### `std::vector<Post*>` (Master Post List & User's Post List)

* **Used In:** `FakeBook::masterPostList` and `User::posts`
* **Justification:** `std::vector` was chosen for its $O(1)$ amortized `push_back` time and its fast, cache-friendly $O(N)$ traversal.
* **Analysis:**
    * In `User::posts`, the vector is ideal for the `viewOwnProfile()` feature, which simply iterates the list from beginning to end, satisfying the requirement to show a "List of posts created by the user."
    * In `FakeBook::masterPostList`, it provides a simple way to store all posts. More importantly, this vector (and the user's post vector) is used as the underlying container for the Heapsort algorithm.

### Pointers (`User*`, `Post*`)

* **Used In:** All class relationships (e.g., `Post::authorId`, `User::friends`, `FakeBook::currentSession`).
* **Justification:** This was the most critical design decision of the project. Instead of storing copies of objects, which would be memory-intensive and lead to data desynchronization (a user updates their profile, but their friend's *copy* of them doesn't), we store pointers.
* **Analysis:** Using pointers ensures that there is only **one** `User` object for "xyz" in memory. Every other object (xyz's friends, xyz's posts) holds a pointer to that single object. This guarantees data integrity, is highly memory-efficient, and allows for $O(1)$ access to a friend's or author's data once the pointer is retrieved.

## 3. Algorithms Implemented

### Heapsort (Home Feed)

* **Requirement:** The project required the user's home feed to be sorted by timestamp (newest first). A heapsort was also specified.
* **Implementation:** The `User::viewFeed()` method implements this using the C++ standard library algorithms in `<algorithm>`.
    1.  **Gather:** Posts from direct friends and public posts from friends-of-friends are collected into a `std::set<Post*>` to automatically handle duplicates.
    2.  **Copy:** The unique posts are copied from the set into a `std::vector<Post*>`.
    3.  **Heapify:** `std::make_heap(vec.begin(), vec.end(), PostComparator())` is called. This uses a custom `PostComparator` struct to build a **max-heap**, organizing the vector so the newest post (largest timestamp) is at the root.
    4.  **Sort:** `std::sort_heap(vec.begin(), vec.end(), PostComparator())` is called. This algorithm iteratively pulls the max element from the heap, resulting in a vector sorted from oldest to newest.
    5.  **Display:** The sorted vector is iterated in **reverse** (`rbegin()` to `rend()`) to display the posts from newest to oldest, satisfying the requirement.

### Fisher-Yates Shuffle (Data Generation)

* **Requirement:** The `DummyDataGenerator` needed to produce "realistic" and "random" data.
* **Implementation:** The `std::shuffle` algorithm was used in the `DummyDataGenerator`'s constructor.
* **Analysis:** Instead of randomly picking an item from a pool (which risks collisions and requires re-checks), we first fill all our data pools (e.g., `userIdPool`) sequentially. We then call `std::shuffle` *once* on each pool. This provides a perfectly randomized list, allowing us to simply iterate through the shuffled vector to get a unique, random assignment in $O(1)$ time per user.

### String Parsing (Data Loading)

* **Requirement:** All `parseAll...` methods in `FakeBook` needed to read and interpret the structured text files.
* **Implementation:** A robust parsing algorithm using `std::stringstream` and `std::getline` was used.
* **Analysis:** For each line read, `std::getline(ss, segment, '#')` was used to split the line by the delimiter, storing the segments in a temporary `std::vector<std::string>`. This vector was then processed, and functions like `std::stoi` and `std::stoll` were used to convert the string data into the required C++ types (int, long long, etc.). This proved to be a flexible and reliable method for handling complex file formats.

## 4. Challenges Faced

### 1. Data Persistence for `friends.txt`

* **Challenge:** The project requires that new friendships (created at runtime) are saved to `friends.txt`.
* **Problem:** The file format (`userId:friendId1,friendId2,...`) is a "current state" representation, not a log. Standard C++ file streams (`std::fstream`) cannot "insert" data into the middle of a file; they can only append or overwrite.
* **Solution:** We were forced to adopt a **"read-modify-rewrite"** strategy. When a friendship is added, the `appendFriend()` method calls a helper, `saveAllFriendsToFile()`. This helper function rewrites the *entire* `friends.txt` file from scratch based on the current in-memory `masterUserList`. While highly inefficient on a large scale, this was the only viable solution to maintain the integrity of the required file format.

### 2. File Paths and IDE Working Directory

* **Challenge:** The `DummyDataGenerator` would run successfully but the data files in the `DataStorage` directory remained empty.
* **Problem:** This was traced to a mismatch in the Current Working Directory (CWD). The CLion IDE was running the executable from the `cmake-build-debug` folder, so the program was writing the files to `cmake-build-debug/DataStorage/Users.txt`, not the `DataStorage` folder in the project root.
* **Solution:** The problem was solved by editing the "Run/Debug Configuration" in CLion to explicitly set the "Working directory" to the project's root folder. This ensured the relative path `DataStorage/Users.txt` resolved correctly.

### 3. Circular Dependencies

* **Challenge:** The initial design of `User.h` and `Post.h` created a compilation-blocking loop.
* **Problem:** `User.h` stored a `std::vector<Post*>` (requiring `#include "Post.h"`), and `Post.h` stored a `User* authorId` (requiring `#include "User.h"`).
* **Solution:** This was solved using C++ **forward declarations**. By replacing the `#include` directives with `class Post;` (in `User.h`) and `class User;` (in `Post.h`), we informed the compiler that these types existed without needing their full definition, successfully breaking the dependency cycle.