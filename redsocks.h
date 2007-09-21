#ifndef REDSOCKS_H_WED_JAN_24_22_17_11_2007
#define REDSOCKS_H_WED_JAN_24_22_17_11_2007
/* $Id$ */

#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <event.h>
#include "list.h"

#if defined __GNUC__
#define PACKED __attribute__((packed))
#else
#error Unknown compiler, modify types.h for it
#endif


struct redsocks_client_t;

typedef struct relay_subsys_t {
	char   *name;
	size_t  payload_len; // size of relay-specific data in client section
	evbuffercb readcb;
	evbuffercb writecb;
	void       (*init)(struct redsocks_client_t *client);
	void       (*fini)(struct redsocks_client_t *client);
	// connect_relay (if any) is called instead of redsocks_connect_relay after client connection acceptance
	void       (*connect_relay)(struct redsocks_client_t *client);
} relay_subsys;

typedef struct redsocks_config_t {
	struct sockaddr_in bindaddr;
	struct sockaddr_in relayaddr;
	char *type;
	char *login;
	char *password;
} redsocks_config;

typedef struct redsocks_instance_t {
	redsocks_config config;
	struct event    listener;
	list_head       clients;
	relay_subsys   *relay_ss;
} redsocks_instance;

typedef struct redsocks_client_t {
	list_head           list;
	redsocks_instance  *instance;
	struct bufferevent *client;
	struct bufferevent *relay;
	struct sockaddr_in  clientaddr;
	struct sockaddr_in  destaddr;
	int                 state;
} redsocks_client;


void redsocks_drop_client(redsocks_client *client);
void redsocks_connect_relay(redsocks_client *client);
void redsocks_start_relay(redsocks_client *client);

typedef int (*size_comparator)(size_t a, size_t b);
int sizes_equal(size_t a, size_t b);
int sizes_greater_equal(size_t a, size_t b);
/** helper for functions when we expect ONLY reply of some size and anything else is error 
 */
int redsocks_read_expected(redsocks_client *client, struct evbuffer *input, void *data, size_comparator comparator, size_t expected);

typedef struct evbuffer* (*redsocks_message_maker)(redsocks_client *client);
struct evbuffer *mkevbuffer(void *data, size_t len);
void redsocks_write_helper_ex(
	struct bufferevent *buffev, redsocks_client *client,
	redsocks_message_maker mkmessage, int state, size_t wm_low, size_t wm_high);
void redsocks_write_helper(
	struct bufferevent *buffev, redsocks_client *client,
	redsocks_message_maker mkmessage, int state, size_t wm_only);

/* vim:set tabstop=4 softtabstop=4 shiftwidth=4: */
/* vim:set foldmethod=marker foldlevel=32 foldmarker={,}: */
#endif /* REDSOCKS_H_WED_JAN_24_22_17_11_2007 */
