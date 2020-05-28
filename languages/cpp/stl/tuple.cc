#include <iostream>
#include <tuple>
#include <list>
#include <unordered_map>

using namespace std;

using Nested=std::tuple<int, int, unordered_map<int, int>>;

int main(int argc, char* argv[])
{
  list<Nested> test_list;

  Nested class_1{1, 3, unordered_map<int, int>{{1, 10623}, {2, 10623}, {3, 10624}}};
  test_list.push_back(class_1);

  test_list.push_back(make_tuple(2, 1, unordered_map<int, int>{{1, 20632}}));

  //int id, size;
  //unordered_map<int, int> details;
  for (auto &ele: test_list) {
    //tie(id, size, details) = ele;
    // Following lines can be used after C++ 17 (Structured binding)
    auto [id, size, details] = ele;
    cout << "Class #" << id
	 << ", " << size
	 << " students, details: " << endl;
    for (auto &item: details)
      cout << "Student #" << item.first << ", course No." << item.second << endl;

    cout << endl;
  }

  return 0;
}
