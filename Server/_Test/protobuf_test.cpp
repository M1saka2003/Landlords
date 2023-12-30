#include <iostream>
#include "Person.pb.h"

int main() {
    Person p{};
    p.set_id(10);
    p.set_age(20);
    p.set_sex("xyz");
    p.set_name("abc");

    std::string data;
    Person a{};
    p.SerializeToString(&data);
    a.ParseFromString(data);
    std::cout << a.id() << " " << a.age() << " " << a.sex() << " " << a.name() << std::endl;
}
