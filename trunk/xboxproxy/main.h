#include <pcap.h>

void packet_handler(u_char *args, const struct pcap_pkthdr *head,
						  const u_char *packet);
void addxbox(u_char macaddr, int proxyip);
