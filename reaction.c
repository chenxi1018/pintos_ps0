#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

//*************************************************//
// In this case, we use O atom to be condition     //
// because H2O need only 1 O atom, thus, when an O //
// occurs, we only need to check H		   //
//*************************************************//

//todo:This file has bug, which might caused by dead lock

struct condition element_c;//waiting for 2H+1O elements
struct condition return_ctr;
struct lock mux;
struct lock mux_w;

struct reaction {
	int num_h;//number of h
	int num_o;//number of o
	int num_water;
};

void
reaction_init(struct reaction *reaction)
{
	reaction->num_h = 0;
	reaction->num_o = 0;
	reaction->num_water = 0;
	cond_init(&element_c);
	cond_init(&return_ctr);
	lock_init(&mux);
	lock_init(&mux_w);
	return;
}

void water_create(struct reaction *reaction){
        lock_acquire(&mux_w);
        make_water();
//      printf("%d water!\n",++reaction->num_water);
        reaction->num_o-=1;
        reaction->num_h-=2;
        //reaction->num_water++;
        lock_release(&mux_w);
        return;
}

void
reaction_h(struct reaction *reaction)
{
	lock_acquire(&mux);
	reaction->num_h++;
//	printf("H atom has been added, H total: %d, O total: %d\n",reaction->num_h,reaction->num_o); 
	//repair H atom return bug (100%)
	cond_signal(&return_ctr,&mux);
	cond_wait(&element_c,&mux);

	lock_release(&mux);
	return;
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&mux);
        reaction->num_o++;
//	printf("O atom has been added, H total: %d, O total: %d\n",reaction->num_h,reaction->num_o);
	while((reaction->num_h<2))
		cond_wait(&return_ctr,&mux);
	water_create(&*reaction);
	cond_signal(&element_c,&mux);
	cond_signal(&element_c,&mux);
	lock_release(&mux);
	return;
}
