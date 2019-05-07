// person.cc
// Definition of Person class 


#include "person.h"

Person::Person(int id, int priority, int time) : id(id), priority(priority), time(time) {}

const int Person::Id() const {
  return id;
}

const int Person::Priority() const{
  return priority;
}

Person& Person::Priority(const int priority) {
  this->priority = priority;
  return *this;
}

const int Person::Time() const{
  return time;
}

Person& Person::Time(const int time) {
  this->time = time;
  return *this;
}

Person& Person::operator=(const Person& person)
{
  this->id = person.id;
  this->priority = person.priority;
  this->time = person.time;
  return *this;
}

bool operator== (const Person& l, const Person& r)
{
  return l.Id() == r.Id() && l.Priority() == r.Priority() && l.Time() == r.Time();
}
