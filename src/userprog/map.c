#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void map_init(struct map* m)
{
	list_init(&(m->content));
	m->next_key = 2;
}


key_t map_insert(struct map* m, value_t* v)
{
	struct association* as = (struct association*)malloc(sizeof(struct association));
	
	value_t* valPtr = (value_t*)malloc(sizeof(value_t));
	valPtr = v;
	
	as->value = valPtr;	
	as->key = m->next_key++;
	list_push_back(&m->content, &as->elem);

	return as->key;
}



struct association* find_association_by_key(struct map* m, key_t key)
{
	struct list_elem* e;
	for(e = list_begin(&m->content); e != list_end(&m->content);)
	{
		struct list_elem* have = e;
		struct association* want = NULL;
		want = list_entry(have, struct association, elem);
		if(want->key == key){
			return want;
		}
		e = list_next(e);
	}
	return NULL;
}

value_t map_find(struct map* m, key_t k)
{
	struct association* s;
	s = find_association_by_key(m, k);
	
	if(s != NULL)
	{	
		// Found
		return s->value;
	}
	// Did not find element
	return NULL;
}



value_t map_remove(struct map* m, key_t k)
{
	struct association* s;
	s = find_association_by_key(m, k);
	if(s != NULL)
	{
		list_remove(&s->elem);
		free(s);
		
		return s->value;
	}

	// Did not find element...
	return NULL;
}


void map_for_each(struct map* m, 
	void (*exec)(key_t k, value_t v, int aux), int aux) {
	

	struct list_elem* e;
	struct association *assoc;
	
	for (e = list_begin(&m->content); e != list_end(&m->content); e = list_next(e))
	{
		assoc = list_entry(e, struct association, elem);
		exec(assoc->key, (value_t)assoc->value, aux);
	}
}

void map_remove_if(struct map* m, bool (*cond)(key_t k, value_t v, int aux), int aux)
{
	struct list_elem* e;
	
	for (e = list_begin(&m->content); e != list_end(&m->content);)
	{
		struct association *assoc = list_entry(e, struct association, elem);
		if(cond(assoc->key, assoc->value, aux)){
			e = list_remove(&assoc->elem);
			free(assoc);
		} 
		else{
 			e = list_next(e);
		}
	}
}

/*

int main(int argc, char* argv[])
{
	struct map m;
	map_init(&m);

	key_t key = map_insert(&m, "900");	
	value_t retval = map_find(&m, (key_t)key);	
	printf("Value found: %s\n", retval);
	map_insert(&m, (value_t)"5");
	map_insert(&m, (value_t)"12");
	map_insert(&m, (value_t)"120");


	value_t retval = map_find(&m, (key_t)key);	
	printf("Value found: %i\n", (int)retval);

	map_remove(&m, (key_t)0);
	map_print_all(&m);

  	map_for_each(&m, print_less, 15);

	map_remove_if(&m, do_free, 0);

  	map_for_each(&m, print_less, 1000);

  	return 0;
}*/
