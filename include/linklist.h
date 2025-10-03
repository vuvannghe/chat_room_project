/* Copyright (C) 2001-2011 IP Infusion, Inc. All Rights Reserved. */
#ifndef _ZEBOS_LINKLIST_H
#define _ZEBOS_LINKLIST_H

#include <inttypes.h>
#include <sys/types.h>

typedef int32_t (*list_del_cb_t) (void *val);
typedef int32_t (*list_cmp_cb_t) (void *val1, void *val2);

struct listnode 
{
  struct listnode *next;
  struct listnode *prev;
  void *data;
};

struct list 
{
  struct listnode *head;
  struct listnode *tail;
  u_int32_t count;
  list_cmp_cb_t cmp;
  list_del_cb_t del;
};


/* Prototypes. */
struct list *list_new(); //
struct list *list_create(list_cmp_cb_t cmp_cb, list_del_cb_t del_cb); //

void list_free (struct list *); //

struct listnode *listnode_sort_add (struct list *_list, void *data);
struct listnode *listnode_add (struct list *, void *);  // them vao cuoi
struct listnode *listnode_add_sort (struct list *, void *); //
// u_int16_t listnode_add_sort_index (struct list *,u_int32_t *);

struct listnode *listnode_add_before (struct list *, struct listnode *,void *); // Them mot node truoc mot node bat ki
int listnode_add_sort_nodup (struct list *, void *);
struct listnode *listnode_add_after (struct list *, struct listnode *, void *); // Them mot node sau mot node bat ki
void listnode_delete (struct list *, void *); // Xoa mot node khoi list
void listnode_delete_data (struct list *, void *); // Xoa data cua mot node neu co nod ton tai data do
struct listnode *listnode_lookup (struct list *, void *); // Tim mot node theo data truyen vao
void * list_lookup_data (struct list *list, void *data); 
void *listnode_head (struct list *); //

void list_delete (struct list *); 


/* For ospfd and ospf6d. */
void list_delete_node (struct list *, struct listnode *); //

#endif

