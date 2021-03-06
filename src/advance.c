#include "advance.h"
void handle_input(entity_t *e,char input)
{
	static tile_t *up=NULL;
	static int up_c=-1;
	switch (input) {
	case '\0': // ^@
		debug_command();
		return;
	case '?':
		player_target();
		return;
	case 'R':
		clear_screen();
		clear_announcements();
		draw_local_area();
		announce_stats(player);
		handle_input(player,get_input());
		return;
	case 'q':
		if (up)
			free_area(local_area);
		next_line();
		quit();
	}
	if (e->hp<=0)
		return;
	int o=input_offset(input);
	if (o) {
		try_move(e,e->coords,e->coords+o);
		return;
	}
	char c;
	switch (input) {
	case '<':
		if (up&&local_area[e->coords].bg=='<') {
			fog_of_war=false;
			// Remove player from dungeon and destroy
			local_area[player->coords].e=NULL;
			free_area(local_area);
			// Restore area and location
			local_area=up;
			player->coords=up_c;
			local_area[player->coords].e=player;
			// Clear area and location
			up=NULL;
			up_c=-1;
			draw_local_area();
		}
		return;
	case '>':
		if (local_area[e->coords].bg=='>') {
			fog_of_war=true;
			// Save old area and player location
			up=local_area;
			up_c=player->coords;
			local_area[player->coords].e=NULL; // Remove player
			// Generate dungeon and place player
			local_area=dungeon_gen();
			player->coords=empty_coords(local_area);
			local_area[player->coords].e=player;
			local_area[player->coords].bg='<';
			local_area[player->coords].bg_c=BROWN;
			// Spawn all dungeon-dwellers
			populate(NULL,local_area,-1);
			draw_local_area();
		}
		return;
	case 'a':
		action_menu();
		return;
	case 'c':
		o=input_offset(get_input());
		if (local_area[e->coords+o].bg=='-') {
			local_area[e->coords+o].fg='+';
			draw_posl(e->coords+o);
		}
		return;
	case 'd':
		drop_menu(e);
		return;
	case 'g':
		if (e==player)
			grab_menu(e);
		else
			grab_rand(e);
		return;
	case 'i':
		if (e==player)
			use_menu(e);
		else
			use_rand(e);
		return;
	case 'm':
		if (!(e->flags&CASTS_SPELLS))
			return;
		if (e==player&&player->hp>0)
			spell_menu(e);
		else
			random_spell(e);
		return;
	case 'w':
		if (e!=player||player->hp<=0)
			return;
		c=local_area[e->coords].bg;
		if (c!='#'&&c!='<'&&c!='>'&&c!='-') {
			open_map();
		} else
			announce("s","You must be outside to travel");
		return;
	}
}
void take_turn(entity_t *e)
{
	char key;
	handle_entity_effects(e);
	if (!e->hp)
		return;
	if (e==player) {
		move_cursor(0,HEIGHT);
		print_stats(player);
		next_line();
		key=get_input();
		if (key=='q')
			quit();
		clear_announcements();
		next_line();
	} else
		key=think(e);//generate_input();
	handle_input(e,key);
}
void advance()
{ // Non-player creatures take their turns
	if (fog_of_war)
		draw_local_area();
	tile_t *start_area=local_area;
	static entity_t *e[AREA];
	for (int i=0;i<AREA;i++) {
		handle_tile_effects(i);
		e[i]=local_area[i].e;
	}
	for (int i=0;i<AREA;i++) {
		if (e[i]==player||!e[i]||!e[i]->hp)
			continue;
		else
			take_turn(e[i]);
		if (local_area!=start_area)
			return;
	}
}
