#include <ctime>
#include <cstdlib>

#include "request.h"

Request::Request(int id, int depart, int dst, int passengers)
: id(id), depart(depart), dst(dst), passengers(passengers), seats(5, 0)
{
    //srand(time(0));

    if (dst == 0) {
	int t_r = rand() % 10;

	if (depart == 1) {
	    if (t_r == 0)
		this->dst = 2;
	    else if (t_r <= 2)
		this->dst = 3;
	    else if (t_r <= 5)
		this->dst = 4;
	    else
		this->dst = 5;
	} else if (depart == 2) {
	    if (t_r <= 4)
		this->dst = 3;
	    else if (t_r <= 6)
		this->dst = 4;
	    else
		this->dst = 5;
	} else if (depart == 3) {
	    this->dst = (t_r <= 5 ? 4 : 5);
	} else {
	    this->dst = 5;
	}
    }

    if (passengers == 0) {
	this->passengers = rand() % 5 + 1; 	
    }
}

const int Request::Id() const { return id; }
const int Request::From() const { return depart; }
const int Request::To() const { return dst; }
const int Request::Passengers() const { return passengers; }

const std::vector<int>& Request::Seat() const { return seats; }
std::vector<int>& Request::Seat() { return seats; }

std::ostream& operator<<(std::ostream& out, const Request& req) {
    out << "Request id: " << req.id
	<< ", from city: " << req.depart
	<< ", to city: " << req.dst
	<< ", passengers count: " << req.passengers;

    return out;
}

bool operator==(const Request& l, const Request& r) 
{
    return l.Id() == r.Id();
}

