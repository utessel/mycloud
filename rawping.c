/*
    iCMP ECHO Packet sender with WD ICMP "Magic" Payload
    Triggers WD MyCloud to load a 'startup.sh' file via tftp.
*/

/*
    Copyright (C) 2014 Uli Tessel (utessel@gmx.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/if_packet.h>

struct MagicPacket
{
  struct ether_header eth;
  struct iphdr ip;
  struct icmphdr icmp;
  char data[56];
} __attribute__ ((packed));

char Magic[16] = "WD-ICMP-BEACON\0";


int main(int argc, char ** argv)
{
  int i;
  int fd;
  int result;
  int mac[6];
  struct ifreq ifDataIdx, ifDataMac;
  struct sockaddr_ll address;
  struct MagicPacket packet;

  memset( &mac, 0, sizeof(mac));
  memset( &ifDataIdx, 0, sizeof(ifDataIdx));
  memset( &ifDataMac, 0, sizeof(ifDataMac));
  memset( &address, 0, sizeof(address));
  memset( &packet, 0, sizeof(packet) );

  if (argc != 3)
  {
    printf("Usage: %s <interfacename> <mac-address>\n", argv[0] );
    return -1;
  }

  result = sscanf( argv[2], "%02x:%02x:%02x:%02x:%02x:%02x",
    &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] );

  if (result != 6)
  {
    printf("invalid (%d) MAC Address %s\n", result, argv[2] );
    return -1;
  }

  fd = socket( AF_PACKET, SOCK_RAW, IPPROTO_RAW);
  if (fd == -1)
  {
    perror("Creating raw socket failed. (sudo?)");
    return -1;
  }

  strncpy( ifDataIdx.ifr_name, argv[1], IFNAMSIZ-1 );

  result = ioctl( fd, SIOCGIFINDEX, &ifDataIdx );
  if (result < 0)
  {
    perror("Get Interface index failed.");
    close(fd);
    return -1;
  }

  strncpy( ifDataMac.ifr_name, argv[1], IFNAMSIZ-1 );

  result = ioctl( fd, SIOCGIFHWADDR, &ifDataMac );
  if (result < 0)
  {
    perror("Get Interface (own) MAC Address failed.");
    close(fd);
    return -1;
  }

  printf("Own MAC for %s (idx %d): %02x:%02x:%02x:%02x:%02x:%02x\n",
    ifDataIdx.ifr_name,
    ifDataIdx.ifr_ifindex,
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[0],
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[1],
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[2],
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[3],
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[4],
    (unsigned char)ifDataMac.ifr_hwaddr.sa_data[5]
  ); 

  address.sll_family = PF_PACKET;

  address.sll_ifindex = ifDataIdx.ifr_ifindex;
  address.sll_halen = ETH_ALEN;
  for (i=0; i<6; i++) 
    address.sll_addr[i] = mac[i];

  /* ether */
  for (i=0; i<6; i++) 
    packet.eth.ether_dhost[i] = mac[i];
  memcpy( packet.eth.ether_shost, ifDataMac.ifr_hwaddr.sa_data, 6 );
  packet.eth.ether_type = htons( ETHERTYPE_IP );

  /* ip */
  packet.ip.ihl = 5;
  packet.ip.version = 4;
  packet.ip.tos = 0;
  packet.ip.tot_len = htons(0x54);
  packet.ip.id = 0;
  packet.ip.frag_off = htons(0x4000);
  packet.ip.ttl = 0x40;
  packet.ip.protocol =  IPPROTO_ICMP;
  packet.ip.check = htons(0x36a6); /* todo: calculate */
  packet.ip.saddr = 0x01010101; /* todo: better use own address */
  packet.ip.daddr = 0x01010101; /* todo: better use later address of device*/

  /* icmp: */
  packet.icmp.type = ICMP_ECHO;
  packet.icmp.code = 0;
  packet.icmp.checksum = htons( 0x1f5b ); /* not important, but keeps wireshark happy */
  packet.icmp.un.echo.id = 0;
  packet.icmp.un.echo.sequence = 0;

  for (i=0; i<56; i++)
   packet.data[i] = Magic[i % sizeof(Magic)];

  for (;;)
  {
    printf("Sending Packet to %s\n", argv[2]);
    result = sendto(fd, &packet, sizeof(packet), 0, (const struct sockaddr*)&address, sizeof(address));

    if (result < 0)
    {
      perror("sendto failed.");
      close(fd);
      return -1;
    }
    usleep(500000);
  }
}

