#include <iostream>
#include <vector>
#include <string>

struct Record {
    std::string name;
    int* age;  // age is dynamically allocated

    Record(const std::string& name_, int age_) {
        name = name_;
        age = new int(age_);
    }

    void print() const {
        std::cout << name << ": " << *age << std::endl;
    }

    ~Record() {
    }
};

class Database {
private:
    std::vector<Record*> records;

public:
    void add_record(const std::string& name, int age) {
        Record* r = new Record(name, age);
        records.push_back(r);
    }

    void print_all() const {
        for (auto r : records) {
            r->print();
        }
    }

    ~Database() {
    }
};

int main() {
    Database db;
    db.add_record("Alice", 30);
    db.add_record("Bob", 25);
    db.add_record("Charlie", 40);
    db.print_all();

    return 0;
}
