#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <iostream>
#include <vector>

class Request {
public:
    Request(): seats(5, 0) {};
    Request(int, int, int dst = 0, int passengers = 0);
    ~Request(){};

    const int Id() const;
    const int From() const;
    const int To() const;
    const int Passengers() const;

    std::vector<int>& Seat();
    const std::vector<int>& Seat() const;

    friend std::ostream& operator<<(std::ostream& out, const Request& req);
private:
    int id;
    int depart;
    int dst;
    int passengers;
    std::vector<int> seats;
};

bool operator==(const Request& l, const Request& r);

#endif
