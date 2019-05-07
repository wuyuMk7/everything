#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "kernel.h"
#include "main.h"
#include "thread.h"
#include "person.h"
#include "list.h"

int ComparePriority(Person x, Person y)
{
  if (x.Priority() < y.Priority()) return 1;
  else if (x.Priority() > y.Priority()) return -1;
  else return 0;
}

void PrintWaiting(Person waiting) 
{
  printf("Person %d with sequence: %d, priority: %d, time: %ds.\n", waiting.Id(), waiting.Id() + 1, waiting.Priority(), waiting.Time());
}

void
ThreadTest()
{
  SortedList<Person> simulation_1(ComparePriority);
  List<Person> simulation_2;  
  Person in;
  int order = 1, totalTime = 0;

  srand(time(0));
  for(int i = 0;i < 20;++ i) {
    Person p(i, rand() % 10, rand() % 3 + 1);  
    simulation_1.Insert(p);
  }
  
  printf("The highest priority is 9.\n");
  printf("Simulation 1 waiting queue: \n");
  simulation_1.Apply(PrintWaiting);
  for (order = 1;order <= 20;++ order) {
    printf("-----------------------------------------------------------------\n");
    printf("Order: #%d\n", order);

    if (order > 1) {
      printf("Person %d (sequence %d) is leaving now, with laundry time %ds.\n", in.Id(), in.Id() + 1, in.Time());
    }  
    in = simulation_1.RemoveFront();
    printf("Person %d is entering now. Sequence: %d. Priority: %d. Time: %ds.\n", in.Id(), in.Id() + 1, in.Priority(), in.Time());
    printf("Total laundry time: %ds\n", totalTime); 
    totalTime += in.Time();
    sleep(in.Time());
  }
  printf("-----------------------------------------------------------------\n");
  printf("Person %d is leaving now, with laundry time %ds.\n", in.Id(), in.Time());
  printf("Total laundry time: %ds\n", totalTime); 
  printf("-----------------------------------------------------------------\n\n\n");


  for(int i = 0;i < 20;++ i) {
    Person p(i, rand() % 10, rand() % 3 + 1);  
    simulation_2.Append(p);
  }
  totalTime = 0;
  printf("Simulation 2 waiting queue: \n");
  simulation_2.Apply(PrintWaiting);
  for (order = 1;order <= 20;++ order) {
    printf("-----------------------------------------------------------------\n");
    printf("Order: #%d\n", order);

    if (order > 1) {
      printf("Person %d is leaving now, with laundry time %ds.\n", in.Id(), in.Time());
    }  

    in = simulation_2.Front();
    ListIterator<Person> it(&simulation_2);
    for(; !it.IsDone(); it.Next()) {
      if (it.Item().Priority() > in.Priority()) 
	in = it.Item();
    }
    simulation_2.Remove(in);	

    printf("Person %d is entering now. Sequence: %d. Priority: %d. Time: %ds.\n", in.Id(), in.Id() + 1, in.Priority(), in.Time());
    printf("Total laundry time: %ds\n", totalTime); 
    totalTime += in.Time();
    sleep(in.Time());
  }
  printf("-----------------------------------------------------------------\n");
  printf("Person %d (sequence %d) is leaving now, with laundry time %ds.\n", in.Id(), in.Id() + 1, in.Time());
  printf("Total laundry time: %ds\n", totalTime); 
  printf("-----------------------------------------------------------------\n");
}
