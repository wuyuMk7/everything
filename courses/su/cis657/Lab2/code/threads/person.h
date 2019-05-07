// person.h
// Declaration of Person class


#ifndef _PERSON_H_
#define _PERSON_H_


class Person {

private:
  int id;
  int priority;
  int time;

public:
  Person() : id(0), priority(0), time(0) {};
  Person(int id, int priority, int time);
  ~Person() {};

  const int Id() const;

  const int Priority() const;
  Person& Priority(const int);

  const int Time() const;
  Person& Time(const int);

  Person& operator= (const Person& person);
};

bool operator== (const Person& p1, const Person& p2);

#endif
