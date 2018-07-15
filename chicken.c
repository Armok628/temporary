#include "src/creatures.h"
// Add-on example
void eat_chicken(entity_t *e)
{
	announce("e s",e,"eats a piece of chicken");
	e->hp+=10;
	if (e->hp>e->maxhp)
		e->hp=e->maxhp;
	remove_item(e->inventory,&chicken_meat,1);
}
itype_t chicken_meat={
	.name="Chicken Meat",
	.category=UTILITY,
	.effect=0,
	.value=5,
	.symbol='c',
	.color=LRED,
	.use=&eat_chicken,
	.spawn_flags=NONE,
};
etype_t chicken_etype={
	.name="Chicken",
	.symbol='c',
	.color=YELLOW,
	.maxhp=25,.minhp=10,
	.maxres=5,.minres=1,
	.maxagi=15,.minagi=10,
	.maxwis=5,.minwis=1,
	.maxstr=10,.minstr=5,
	.flags=SOLID|MOBILE,
	.loot_table={
		.items={&chicken_meat},
		.amounts={2},
		.chances={100},
	},
	.friends={
		&chicken_etype,
		&human_etype,
	},
	.spawn_flags=TOWN|OUTSIDE,
	.min_elev=1,.max_elev=65,
	.min_sp=5,.max_sp=10,
};
