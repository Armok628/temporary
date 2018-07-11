#include "inventory.h"
bool has_axe=false;
bool has_raft=false;
bool has_canoe=false;
int logs=0;
int rocks=0;
int select_item(islot_t l[])
{
	static char buf[1000];
	char *strs[16];
	int c=0,b=0;
	for (;l[c].count;c++) {
		sprintf(buf+b,"%s (%d)",l[c].type->name,l[c].count);
		strs[c]=buf+b;
		b+=strlen(buf+b)+1;
	}
	return menu(strs,c);
}
void add_item(islot_t p[],itype_t *t)
{
	int c=0;
	for (;p[c].count;c++)
		if (p[c].type==t) {
			p[c].count++;
			return;
		}
	p[c].type=t;
	p[c].count=1;
}
itype_t *remove_item(islot_t p[],int i)
{
	if (i<0)
		return NULL;
	int c=0;
	for (;p[c].count;c++);
	if (!c)
		return 0;
	itype_t *t=p[i].type;
	p[i].count--;
	if (p[i].count==0) {
		p[i]=p[c-1];
		p[c-1].count=0;
	}
	return t;
}
void remove_type(islot_t p[],itype_t *t)
{
	for (int i=0;p[i].count;i++)
		if (p[i].type==t) {
			remove_item(p,i);
			return;
		}
}
void drop_item(entity_t *e,int i)
{
	if (i<0)
		return;
	tile_t *t=&local_area[e->coords];
	add_item(t->pile,remove_item(e->inventory,i));
}
void drop_menu(entity_t *e)
{
	drop_item(e,select_item(e->inventory));
}
void loot_item(entity_t *e,int i)
{
	if (i<0)
		return;
	tile_t *t=&local_area[e->coords];
	add_item(e->inventory,remove_item(t->corpse->inventory,i));
}
void grab_item(entity_t *e,int i)
{
	if (i<0)
		return;
	tile_t *t=&local_area[e->coords];
	add_item(e->inventory,remove_item(t->pile,i));
}
static char *piles[]={"Corpse","Pile"};
void grab_menu(entity_t *e)
{
	tile_t *t=&local_area[e->coords];
	if (t->pile[0].count&&t->corpse) {
		switch (menu(piles,2)) {
		case 0: // Corpse
			loot_item(e,select_item(t->corpse->inventory));
			return;
		case 1: // Pile
			grab_item(e,select_item(t->pile));
			return;
		}
	} else if (t->corpse) {
		loot_item(e,select_item(t->corpse->inventory));
	} else if (t->pile[0].count) { // anything on ground
		grab_item(e,select_item(t->pile));
	}
}
void equip(entity_t *e,int i)
{
	if (i<0)
		return;
	add_item(e->equipped,remove_item(e->inventory,i));
}
void equip_menu(entity_t *e)
{
	equip(e,select_item(e->inventory));
}
void unequip(entity_t *e,int i)
{
	if (i<0)
		return;
	add_item(e->inventory,remove_item(e->equipped,i));
}
void unequip_menu(entity_t *e)
{
	unequip(e,select_item(e->equipped));
}
bool equipped(entity_t *e,itype_t *t)
{
	for (int i=0;e->equipped[i].count;i++)
		if (e->equipped[i].type==t)
			return true;
	return false;
}
void use_menu(entity_t *e)
{
	announce("s","Select an item to use");
	itype_t *selected=e->inventory[select_item(e->inventory)].type;
	if (!selected->use) {
		announce("s","There's nothing to do with it");
	} else
		selected->use(e);
}