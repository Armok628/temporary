#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdlib.h>
#include "menu.h"
#include "types.h"
extern int logs;
extern int rocks;
extern bool has_raft;
extern bool has_canoe;

int select_item(islot_t []);
void add_item(islot_t [],itype_t *);
itype_t *remove_item(islot_t [],int);
void remove_type(islot_t [],itype_t *);
void drop_item(entity_t *,int);
void drop_menu(entity_t *);
void loot_item(entity_t *,int);
void grab_item(entity_t *,int);
void grab_menu(entity_t *);
void equip(entity_t *,int);
void equip_menu(entity_t *);
void unequip(entity_t *,int);
void unequip_menu(entity_t *);
bool equipped(entity_t *,itype_t *);
void use_menu(entity_t *);
#endif
