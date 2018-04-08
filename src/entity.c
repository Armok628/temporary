#include "entity.h"
etype_t player_etype={
	.name="Player",
	.symbol='@',
	.color=LCYAN,
	.maxhp=100,.minhp=50,
	.maxwis=15,.minwis=5,
	.maxstr=15,.minstr=5,
	.flags=PERSISTS|SOLID
};
etype_t human_etype={ // Temporary
	.name="Human",
	.symbol='U',
	.color=CYAN,
	.maxhp=100,.minhp=50,
	.maxwis=15,.minwis=5,
	.maxstr=15,.minstr=5,
	.flags=PERSISTS|SOLID
};
etype_t monster_etype={ // Temporary
	.name="Monster",
	.symbol='&',
	.color=DGRAY,
	.maxhp=100,.minhp=50,
	.maxwis=15,.minwis=5,
	.maxstr=15,.minstr=5,
	.flags=SOLID
};
entity_t *player;
void draw_entity(entity_t *c)
{
	set_color(c->color,c->hp?BG BLACK:BG RED);
	putchar(c->symbol);
}
entity_t *spawn(etype_t *type)
{
	entity_t *e=malloc(sizeof(entity_t));
	e->name=type->name;
	e->symbol=type->symbol;
	e->color=type->color;
	e->maxhp=rand()%(type->maxhp-type->minhp)+type->minhp;
	e->hp=e->maxhp;
	e->str=rand()%(type->maxstr-type->minstr)+type->minstr;
	e->wis=rand()%(type->maxwis-type->minwis)+type->minwis;
	e->flags=type->flags;
	// Temporary
	e->spells[0]=&heal_self_spell;
	e->spells[1]=&magic_missile_spell;
	e->spellc=2;
	e->type=type;
	return e;
}
entity_t *spawn_randomly(tile_t *area,etype_t *type)
{
	entity_t *e=spawn(type);
	e->coords=empty_coords(area);
	area[e->coords].e=e;
	return e;
}
entity_t *spawn_inside(tile_t *area,etype_t *type)
{
	entity_t *e=spawn(type);
	e->coords=inside_coords(area);
	area[e->coords].e=e;
	return e;
}
entity_t *spawn_outside(tile_t *area,etype_t *type)
{
	entity_t *e=spawn(type);
	e->coords=outside_coords(area);
	area[e->coords].e=e;
	return e;
}
