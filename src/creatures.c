#include "creatures.h"
etype_t player_etype={
	.name="Player",
	.symbol='@',
	.color=L_CYAN,
	.hp={50,100},
	.res={5,15},
	.agi={5,15},
	.wis={5,15},
	.str={5,15},
	.flags=PERSISTS|SOLID|MOBILE|USES_ITEMS|CASTS_SPELLS,
	.spells={
		&heal_self_spell,
		&magic_missile_spell,
	},
	.spawn_flags=NONE,
};
etype_t human_etype={
	.name="Human",
	.symbol='U',
	.color=CYAN,
	.hp={50,100},
	.res={5,15},
	.agi={5,15},
	.wis={5,15},
	.str={5,15},
	.flags=PERSISTS|SOLID|MOBILE|USES_ITEMS|CASTS_SPELLS,
	.loot_table={
		{&sword,{50,{1,1}}},
		{&gold,{100,{2,10}}},
	},
	.spells={
		&heal_self_spell,
	},
	.enemies={
		&monster_etype,
	},
	.spawn_flags=TOWN|INSIDE|OUTSIDE,
	.elev={1,100},
	.freq={100,{5,AREA/128}},
};
etype_t monster_etype={
	.name="Monster",
	.symbol='&',
	.color=D_GRAY,
	.hp={50,100},
	.res={5,15},
	.agi={5,15},
	.wis={5,10},
	.str={10,15},
	.flags=SOLID|MOBILE|USES_ITEMS|CASTS_SPELLS,
	.loot_table={
		{&gold,{50,{5,25}}},
	},
	.spells={
		&magic_missile_spell,
	},
	.friends={
		&monster_etype,
	},
	.enemies={
		&player_etype,
		&human_etype,
		&mage_etype,
	},
	.spawn_flags=TOWN|WILDERNESS|DUNGEON|OUTSIDE,
	.elev={1,100},
	.freq={100,{8,AREA/128}},
};
etype_t toad_etype={
	.name="Toad",
	.symbol='<',
	.color=GREEN,
	.hp={5,10},
	.res={1,5},
	.agi={15,20},
	.wis={1,2},
	.str={1,5},
	.flags=SOLID|MOBILE,
	.friends={
		&toad_etype,
	},
	.spawn_flags=WILDERNESS|OUTSIDE,
	.elev={51,60},
	.freq={100,{8,AREA/128}},
};
etype_t mage_etype={
	.name="Mage",
	.symbol='M',
	.color=PURPLE,
	.hp={50,75},
	.res={5,10},
	.agi={5,15},
	.wis={15,20},
	.str={5,10},
	.flags=PERSISTS|SOLID|MOBILE|USES_ITEMS|CASTS_SPELLS,
	.loot_table={
		{&gold,{100,{10,20}}},
	},
	.spells={
		&heal_self_spell,
		&magic_missile_spell,
		&raise_dead_spell,
	},
	.friends={
		&mage_etype,
		&player_etype,
	},
	.enemies={
		&monster_etype,
	},
	.spawn_flags=TOWN|INSIDE,
	.elev={1,100},
	.freq={100,{2,AREA/128}},
};
// Dragons and firebreathing
// Entity effect: Burning
effect_t burning;
void burn_start(int c)
{
	entity_t *e=local_area[c].e;
	announce("e s",e,"bursts into flames!");
	e->color=L_RED;
	draw_posl(e->coords);
}
void burn_turn(int c)
{
	entity_t *e=local_area[c].e;
	int d=1+rand()%5;
	if (e->color==L_RED)
		e->color=YELLOW;
	else
		e->color=L_RED;
	announce("e s d s",e,"burns for",d,"damage");
	e->hp-=d;
	if (e->hp<=0) {
		printf(", killing it!");
		end_effect(e->effects,&burning,c);
		kill_entity(e);
		draw_posl(e->coords);
	}
}
void burn_end(int c)
{
	entity_t *e=local_area[c].e;
	announce("e s",e,"stops burning");
	e->color=e->type->color;
	draw_posl(e->coords);
}
effect_t burning={
	.start=&burn_start,
	.turn=&burn_turn,
	.end=&burn_end,
};
// Tile effect: Growing grass
bool has_grass(tile_t *t)
{
	return t->bg_c==GREEN||t->bg_c==L_GREEN;
}
bool grass_adjacent(int c)
{
	for (int dx=-1;dx<=1;dx++)
		for (int dy=-1;dy<=1;dy++) {
			int t=c+lin(dx,dy);
			if (!legal_move(c,t))
				continue;
			if (has_grass(&local_area[t]))
				return true;
		}
	return false;
}
void try_grow(int);
effect_t growing_grass={
	.start=NULL,
	.turn=&try_grow,
	.end=NULL,
};
void try_grow(int c)
{
	if (grass_adjacent(c)&&!(rand()%50)) {
		local_area[c].bg_c=rand()%2?GREEN:L_GREEN;
		end_effect(local_area[c].effects,&growing_grass,c);
		draw_posl(c);
	}
}
// Tile effect: Burning
void tile_burning_start(int c)
{
	local_area[c].bg_c=rand()%2?L_RED:YELLOW;
}
void tile_burning_turn(int c)
{
	tile_t *t=&local_area[c];
	if (t->e&&t->e->type!=&dragon_etype)
		add_effect(t->e->effects,&burning,5+rand()%5,c);
	if (t->bg_c==L_RED)
		t->bg_c=YELLOW;
	else
		t->bg_c=L_RED;
	draw_posl(c);
}
void tile_burning_end(int c)
{
	local_area[c].bg_c=L_GRAY;
	draw_posl(c);
	//add_effect(local_area[c].effects,&growing_grass,1000,c);
}
effect_t tile_burning={
	.start=&tile_burning_start,
	.turn=&tile_burning_turn,
	.end=&tile_burning_end,
};
// Spell: Breathe fire
bool set_fire(int c)
{
	add_effect(local_area[c].effects,&tile_burning,5+rand()%5,c);
	draw_posl(c);
	return true;
}
SPELL_START(breathe_fire,Breathe Fire,OFFENSE)
	ON(target_enemy(caster))
	if (!target)
		return;
	int effect=10+rand()%caster->wis;
	announce("e s es d s",caster,"breathes fire at",target,", doing",effect,"damage");
	target->hp-=effect;
	in_line(caster->coords,target->coords,&set_fire);
	end_effect(target->effects,&burning,target->coords);
	if (target->hp<=0) {
		target->hp=0;
		kill_entity(target);
		draw_posl(target->coords);
		return;
	}
	add_effect(target->effects,&burning,5+rand()%5,target->coords);
	draw_posl(target->coords);
SPELL_END
etype_t dragon_etype={
	.name="Dragon",
	.symbol='D',
	.color=GREEN,
	.hp={150,200},
	.res={15,20},
	.agi={15,20},
	.wis={15,20},
	.str={15,20},
	.flags=PERSISTS|SOLID|MOBILE|CASTS_SPELLS,
	.loot_table={
		{&gold,{100,{100,200}}},
	},
	.spells={
		&breathe_fire_spell,
	},
	.enemies={
		&player_etype,
		&human_etype,
		&mage_etype,
		&monster_etype,
	},
	.spawn_flags=TOWN|WILDERNESS|OUTSIDE,
	.elev={66,100},
	.freq={50,{1,1}},
};
