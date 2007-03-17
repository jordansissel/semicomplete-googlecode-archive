/*
 * structures.c
 *
 * Storage types!
 *
 * $Id$
 */

#include "hash.h"
#include "structures.h"

#include "transport.h" /* For things like sockaddr_in */

/* Should this be a runtime tweakable? */
#define HASHSIZE 512

static hash_t *xboxen;
static hash_t *proxies;

void init_storage() {
	xboxen = hash_create(HASHSIZE, comparemac, hashmac);
	proxies = hash_create(HASHSIZE, compareip, haship);
}

proxy_t *addproxy(struct sockaddr_in *addr, int sock) {
	proxy_t *newproxy = NULL;
	hnode_t *proxy = NULL;

	//debuglog(5, "Data from proxy: %s", inet_ntoa(addr->sin_addr));

	proxy = hash_lookup(proxies, &(addr->sin_addr.s_addr));
	if (proxy == NULL) {
		hscan_t hs;
		hnode_t *node;

		newproxy = malloc(sizeof(proxy_t));
		newproxy->ip = (int) addr->sin_addr.s_addr;
		newproxy->addr = addr->sin_addr;
		newproxy->port = addr->sin_port;
		newproxy->fd = sock;
		newproxy->xboxen = hash_create(HASHSIZE, comparemac, hashmac);

		debuglog(1, "NEW PROXY FOUND: %s", inet_ntoa(addr->sin_addr));
		hash_alloc_insert(proxies, &(newproxy->ip), newproxy);

		/* List known proxies */
		hash_scan_begin(&hs, proxies);
		fprintf(stderr, "KNOWN PROXIES:\n");
		while ((node = hash_scan_next(&hs))) {
			proxy_t *p = (proxy_t *)(node->hash_data);
			struct sockaddr_in i;
			i.sin_addr.s_addr = *(int *)(node->hash_key);
			fprintf(stderr, "PROXY: %s\n", inet_ntoa(p->addr));
		}
		fprintf(stderr, "END\n");

		return newproxy;
	} else {
		proxy_t *p = proxy->hash_data;
		debuglog(15, "Packet from known proxy...");
		if (p->port != addr->sin_port) {
			debuglog(0, "Port changed on client %s (%d -> %d)", inet_ntoa(addr->sin_addr),
						p->port, addr->sin_port);
			p->port = addr->sin_port;
		}
		return (proxy->hash_data);
	}

}

void remove_proxy(proxy_t *ppt) {
	hscan_t hs;
	hnode_t *node;

	debuglog(1, "Removing proxy %s", inet_ntoa(ppt->addr));

	hash_scan_begin(&hs, proxies);
	while ((node = hash_scan_next(&hs))) {
		proxy_t *p = (proxy_t *)(node->hash_data);
		if (ppt->ip == p->ip) {
			hash_delete(proxies, node);
			break;
		}
	}
}

void addxbox(MACTYPE *macaddr, proxy_t *ppt) {
	xbox_t *newbox = NULL;
	hnode_t *box = NULL;

	box = hash_lookup(xboxen, macaddr);
	if (box == NULL) {
		hscan_t hs;
		hnode_t *node;

		debuglog(1, "NEW XBOX FOUND: %s", ether_ntoa((struct ether_addr *)macaddr));
		debuglog(1, "\tLocation: %s", (ppt == NULL ? "Local" : inet_ntoa(ppt->addr)));
		newbox = malloc(sizeof(xbox_t));
		memcpy(newbox->macaddr,macaddr,ETHER_ADDR_LEN);
		//fprintf(stderr, "Stored macaddr: %s\n\n", ether_ntoa((struct ether_addr *)(newbox->macaddr)));
		newbox->lastseen = time(NULL);
		newbox->proxy = ppt;
		//hash_alloc_insert(xboxen, macaddr, newbox);
		hash_alloc_insert(xboxen, newbox->macaddr, newbox);
		if (ppt != NULL)
			hash_alloc_insert(ppt->xboxen, newbox->macaddr, newbox);

		/* List Known Xboxes */
		hash_scan_begin(&hs, xboxen);
		fprintf(stderr, "KNOWN XBOXES:\n");
		while ((node = hash_scan_next(&hs))) {
			xbox_t *x = (xbox_t *)(node->hash_data);
			fprintf(stderr, "XBOX: %s\n", ether_ntoa((struct ether_addr *)x->macaddr));
		}
		fprintf(stderr, "END\n");
	}
}

int comparemac(const void *k1, const void *k2) {
	extern loglevel;
	if (loglevel >= 30) {
		char foo[25];
		strcpy(foo,ether_ntoa((struct ether_addr *)k1));
		debuglog(30, "comparemac %s vs %s", foo, ether_ntoa((struct ether_addr *)k2));
	}

	return memcmp(k1, k2, ETHER_ADDR_LEN);
}

int compareip(const void *k1, const void *k2) {
	int a, b;
	a = *(unsigned int *)k1;
	b = *(unsigned int *)k2;

	debuglog(30, "compareip: %08x vs %08x = %d", a, b, (a < b ? -1 : (a > b ? 1 : 0)) );

	return (a < b ? -1 : (a > b ? 1 : 0));
}

hash_val_t haship(const void *key) {
	static unsigned long randbox[] = {
		0x49848f1bU, 0xe6255dbaU, 0x36da5bdcU, 0x47bf94e9U,
		0x8cbcce22U, 0x559fc06aU, 0xd268f536U, 0xe10af79aU,
		0xc1af4d69U, 0x1d2917b5U, 0xec4c304dU, 0x9ee5016cU,
		0x69232f74U, 0xfead7bb3U, 0xe9089ab6U, 0xf012f6aeU,
	};

	const unsigned char *str = key;
	int c = 0;
	hash_val_t acc = 0;

	while (c < sizeof(int)) {
		acc ^= randbox[(*str + acc) & 0xf];
		acc = (acc << 1) | (acc >> 31);
		acc &= 0xffffffffU;
		acc ^= randbox[((*str++ >> 4) + acc) & 0xf];
		acc = (acc << 2) | (acc >> 30);
		acc &= 0xffffffffU;
		c++;
	}
	return acc;
}



hash_val_t hashmac(const void *key) {
	static unsigned long randbox[] = {
		0x49848f1bU, 0xe6255dbaU, 0x36da5bdcU, 0x47bf94e9U,
		0x8cbcce22U, 0x559fc06aU, 0xd268f536U, 0xe10af79aU,
		0xc1af4d69U, 0x1d2917b5U, 0xec4c304dU, 0x9ee5016cU,
		0x69232f74U, 0xfead7bb3U, 0xe9089ab6U, 0xf012f6aeU,
	};

	const unsigned char *str = key;
	int c = 0;
	hash_val_t acc = 0;

	while (c < ETHER_ADDR_LEN) {
		acc ^= randbox[(*str + acc) & 0xf];
		acc = (acc << 1) | (acc >> 31);
		acc &= 0xffffffffU;
		acc ^= randbox[((*str++ >> 4) + acc) & 0xf];
		acc = (acc << 2) | (acc >> 30);
		acc &= 0xffffffffU;
		c++;
	}
	return acc;
}

