#include "pintos_thread.h"

struct lock mux;//global lock
struct condition wait_c;//waiting condition
struct condition board_c;//boarding condition



struct station {
	int n_passengers;//passengers count
	int avail_seat;//seat remaining
};

void
station_init(struct station *station)
{
	station->n_passengers = 0;
	station->avail_seat = 0;
	lock_init(&mux);
	cond_init(&board_c);
	cond_init(&wait_c);
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(&mux);
	station->avail_seat = count;
//	printf("Passenger boarding!\n");
	//when no passenger or no seats, end while loop
	while((station->n_passengers != 0)&&(station->avail_seat != 0)){
		cond_signal(&wait_c,&mux);
		cond_wait(&board_c,&mux);
	}
	lock_release(&mux);//
	return 0;
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&mux);//lock
	station->n_passengers++;//exclusive session
//	printf("new passenger!\n");
	cond_wait(&wait_c, &mux);
	lock_release(&mux);//release
	return 0;
}

void
station_on_board(struct station *station)
{
	lock_acquire(&mux);
	cond_signal(&board_c,&mux);
	station->n_passengers--;
	station->avail_seat--;
//	printf("%d passengers remaining, %d seats available\n",station->n_passengers, station->avail_seat);
	lock_release(&mux);
	return 0;
}
