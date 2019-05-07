#include "kernel.h"
#include "main.h"
#include "thread.h"

#include "bitmap.h"
#include "list.h"
#include "request.h"
    
#define SEATS_NUMBER 20
#define REQUESTS_NUMBER 20

Bitmap *seats = new Bitmap(SEATS_NUMBER);			// Bitmap for seats information
int seatsMap[SEATS_NUMBER] = {0};				// Array for seat-request/passenger relation

List<Thread*> *aboardList = new List<Thread*>();
List<Request> *discardRequest = new List<Request>();
int aboardID[REQUESTS_NUMBER];
int current_city = 0;

const char* city_map[] = { "\0", "One", "Two", "Three", "Four", "Five" };

void printSeatMap() 
{
    printf("%92s\n", "Current Seat Map");
    //printf("%s\n", "Current Seat Map");
    printf("%-8s", "Seat");
    for (int i = 0;i < SEATS_NUMBER; ++i)
	printf("%-8d", i);
    printf("\n");

    printf("%-8s", "Status");
    for (int i = 0;i < SEATS_NUMBER; ++i)
	if (seatsMap[i] == -1)
	    printf("%-8s", "Empty");
	else
	    printf("R%-7d", seatsMap[i] / 5);
    printf("\n");
}

void printDiscarded()
{
    printf("\n======================================================================================================================================================================\n");
    printf("Discarded Requests: \n");
    unsigned int discard_length = discardRequest->NumInList();
    if (discard_length <= 0) {
	printf("No discarded request.\n");
    } else {
        for (unsigned int i = 0;i < discard_length; ++i) {
            Request req = discardRequest->RemoveFront();
        	printf("Request %d: %d seats required, destination city %s.\n", req.Id(), req.Passengers(), city_map[req.To()]);
        }
    }
    printf("======================================================================================================================================================================\n\n");
}

int checkSeats() 
{
    int ret = SEATS_NUMBER;

    for (int i = 0;i < SEATS_NUMBER; ++i) {	
	if (seats->Test(i))	
	    -- ret;
    }

    return ret;
}

void printEmptySeats()
{
    printf("Empty seat number:");
    for (int i = 0;i < SEATS_NUMBER; ++i) 
	if (seatsMap[i] == -1)
	    printf(" %d", i);

    printf(".\n");
}

void ReservationThread(int id) {
    int each_seats[5];
    Request req(id, current_city);

    printf("Generate request %d: %d seats required, destination city %s.\n", req.Id(), req.Passengers(), city_map[req.To()]);
	
    if (req.Passengers() <= checkSeats()) {
        printf("Assigned seat");
        for (int i = 0;i < req.Passengers(); ++i) {
    	    int seat = seats->FindAndSet();
    	    seatsMap[seat] = id * 5 + i;
    	    each_seats[i] = seat;
	    req.Seat()[i] = seat;
	    printf(" %d", seat);
        }
	printf(" to request %d.\n", req.Id());

        aboardID[id] = 1;
        aboardList->Append(kernel->currentThread);

	printf("Total number of available seats: %d. ", checkSeats());
        printEmptySeats();
    
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
        kernel->currentThread->Sleep(FALSE);
        (void) kernel->interrupt->SetLevel(oldLevel);
    } else {
	printf("No avaiable seat. Request %d discarded.\n", req.Id());
        discardRequest->Append(req);
        kernel->currentThread->Finish();
    }

    while (1) {
	if (req.To() == current_city) {
	    printf("Request %d arrived at the destination. ", req.Id());
	    printf("Seats");
	    for (int i = 0;i < req.Passengers(); ++i) {
		printf(" %d", each_seats[i]);
		seats->Clear(each_seats[i]);
		seatsMap[each_seats[i]] = -1;
	    }
	    printf(" returned.\n");

	    aboardID[id] = 0;
	    kernel->currentThread->Finish();
	} else {
	    aboardList->Append(kernel->currentThread);
    	    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    	    kernel->currentThread->Sleep(FALSE);
    	    (void) kernel->interrupt->SetLevel(oldLevel);
	}
    }
}

void FlightThread()
{
    while(current_city < 5) {
    	++ current_city;
	
	printf("\n======================================================================================================================================================================\n");
	printf("Arrive in city: %s.\n", city_map[current_city]);
	printSeatMap();
	printf("======================================================================================================================================================================\n\n");

	// Check Aboard Requests

	if (current_city > 1) {

	    printf("--- Check aboard requests. \n");

	    unsigned int list_length = aboardList->NumInList();
	    int aboard_count = checkSeats();
    	    for (unsigned int i = 0;i < list_length; ++i) {
    	        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    	        kernel->scheduler->ReadyToRun(aboardList->RemoveFront());
    	        (void) kernel->interrupt->SetLevel(oldLevel);
    	        kernel->currentThread->Yield();
    	    }

	    if (aboard_count != checkSeats()) {
	        printf("%d seats returned. \n", checkSeats() - aboard_count);
    	        //printSeatMap();
	    } else {
	        printf("Nobody left the plane. \n");
	    }

    	    printSeatMap();
	    printf("Request currently on the plane:");
	    for (int i = 0;i < REQUESTS_NUMBER; ++i)  
	        if (aboardID[i] != 0)
	            printf(" %d", i);
	    printf(".\nOccupancy rate: %.2f%%\n ", 100 * ((float)(SEATS_NUMBER - checkSeats())) / SEATS_NUMBER);
	}


    	// New Reservation Thread

	if (current_city < 5) {
	    printf("\n--- Generate new requests. \n");

    	    for (int i = 0;i < 5; ++i)  {
    	        int id = (current_city - 1) * 5 + i;

    	        char *name = new char[25]();
    	        char id_str[3] = {'\0'};
    	        sprintf(id_str, "%d", id);
    	        strncpy(name, "Request Thread - ", 17);
    	        strncat(name, (const char*)id_str, sizeof(id_str));

    	        Thread *t = new Thread(name);
    	        t->Fork((VoidFunctionPtr) ReservationThread, (void *) id);
    	        kernel->currentThread->Yield();
    	    }

	    // Departure 

	    printf("\n--- Depart from city %s. \n", city_map[current_city]);
	    //printSeatMap();
	}
    }

    printDiscarded();
}

void
ThreadTest()
{
    for (int i = 0;i < SEATS_NUMBER; ++i) {
	aboardID[i] = 0;
	seatsMap[i] = -1;
    }

    Thread *flight = new Thread("Flight Thread");
    flight->Fork((VoidFunctionPtr) FlightThread, (void *) 1);
}
