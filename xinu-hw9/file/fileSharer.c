/* fileSharer.c - fileSharer */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */
/* Modified by Jason Laqua and Kaleb Breault */

#include <kernel.h>
#include <xc.h>
#include <file.h>
#include <fileshare.h>
#include <ether.h>
#include <network.h>
#include <nvram.h>

struct fishnode school[SCHOOLMAX];
char   fishlist[DIRENTRIES][FNAMLEN];

static uchar bcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uchar myMAC[ETH_ADDR_LEN];

int fishAnnounce(uchar *packet)
{
	struct ethergram *eg = (struct ethergram *)packet;
	int i = 0;

	for (i = 0; i < SCHOOLMAX; i++)
	{
		/* Check to see if node already known. */
		if (school[i].used && 
			(0 == memcmp(school[i].mac, eg->src, ETH_ADDR_LEN)))
			return OK;
	}
	for (i = 0; i < SCHOOLMAX; i++)
	{
		/* Else find an unused slot and store it. */
		if (!school[i].used)
		{
			school[i].used = 1;
			memcpy(school[i].mac, eg->src, ETH_ADDR_LEN);
			memcpy(school[i].name, eg->data + 1, FISH_MAXNAME);
			//printf("Node %s\n", school[i].name);
			return OK;
		}
	}
	return SYSERR;
}

/*------------------------------------------------------------------------
 * fishPing - Reply to a broadcast FISH request.
 *------------------------------------------------------------------------
 */
void fishPing(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;

	/* Source of request becomes destination of reply. */
	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	/* Source of reply becomes me. */
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	/* Zero out payload. */
	bzero(eg->data, ETHER_MINPAYLOAD);
	/* FISH type becomes ANNOUNCE. */
	eg->data[0] = FISH_ANNOUNCE;
	strncpy(&eg->data[1], nvramGet("hostname\0"), FISH_MAXNAME-1);
	write(ETH0, packet, ETHER_SIZE + ETHER_MINPAYLOAD);
}

void fishDirAsk(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	
	/* Source of request becomes destination of reply. */
	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	/* Source of reply becomes me. */
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	/* Zero out payload. */
	bzero(eg->data, ETHER_MINPAYLOAD);
	/* adjust ppkt to current location */
	ppkt += ETHER_SIZE;
	/* FISH type becomes DIRLIST */
	eg->data[0] = FISH_DIRLIST;
	/* adjust ppkt to current location */
	ppkt++;
	
	int i;
	int j;
    for (i = 0; i < DIRENTRIES; i++)
	{
		ppkt += FNAMLEN;
		if (supertab->sb_dirlst->db_fnodes[i].fn_state)
		{
			//printf("%s\n", supertab->sb_dirlst->db_fnodes[i].fn_name);
			strncpy(&eg->data[i*FNAMLEN+1], supertab->sb_dirlst->db_fnodes[i].fn_name, FNAMLEN);
		}
		else
		{
			for (j = 0; j < FNAMLEN; j++)
				eg->data[i*FNAMLEN+1+j] = 0;
		}
		//printf("eg->data = %s\n", i*FNAMLEN+1, eg->data[i*FNAMLEN+1]);
	}

	int packetSize = 0;
	if ((ppkt - packet) > (ETHER_SIZE + ETHER_MINPAYLOAD))
		packetSize = ppkt - packet;
	else
		packetSize = ETHER_SIZE + ETHER_MINPAYLOAD;

/*	printf("ppkt-packet = %d\n", ppkt-packet);

	printf("Packet (%d bytes):\n", packetSize);
    for (i = 0; i < packetSize; i++)
    {
		printf("packet[%d] = 0x%02X %c\n", i, packet[i], packet[i]);
    }
*/
	write(ETH0, packet, packetSize);
}

int fishDirList(uchar *packet)
{
	struct ethergram *eg = (struct ethergram *)packet;
	
	int i;
	for (i = 0; i < DIRENTRIES; i++)
	{
		strncpy(fishlist[i], &eg->data[i*FNAMLEN+1], FNAMLEN);
	}

	return OK;
}

void fishGetFile(uchar *packet)
{
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	
	/* Source of request becomes destination of reply. */
	memcpy(eg->dst, eg->src, ETH_ADDR_LEN);
	/* Source of reply becomes me. */
	memcpy(eg->src, myMAC, ETH_ADDR_LEN);
	/* Zero out payload. */
	bzero(&eg->data[1+FNAMLEN], ETHER_MINPAYLOAD);
	
	int fileFound = 0;
	int i;
	for (i = 0; i < DIRENTRIES; i++)
	{
		if (supertab->sb_dirlst->db_fnodes[i].fn_state)
		{
			if (strncmp(supertab->sb_dirlst->db_fnodes[i].fn_name, &eg->data[1], FNAMLEN) == 0)
			{
				fileFound = 1;
				break;
			}
		}
	}
	int payloadSize = 0;
	if (fileFound)
	{
		/* FISH type becomes HAVEFILE */
		eg->data[0] = FISH_HAVEFILE;
		payloadSize = DISKBLOCKLEN + FNAMLEN + 1;

		int fd = fileOpen(&eg->data[1]);
		int i = 1+FNAMLEN;
		char temp;
		while (i < payloadSize)
		{
			if ((temp = fileGetChar(fd)) != SYSERR)
			{
				eg->data[i] = temp;
			}
			else 
			{
				eg->data[i] = 0;
			}
			i++;
		}
		fileClose(fd);
	}
	else
	{
		/* FISH type becomes NOFILE */
		eg->data[0] = FISH_NOFILE;
		payloadSize = ETHER_MINPAYLOAD;
		
	}
	write(ETH0, packet, ETHER_SIZE + payloadSize);
}

void fishHaveFile(uchar *packet)
{
	struct ethergram *eg = (struct ethergram *)packet;
	int packetSize = ETHER_SIZE + DISKBLOCKLEN + FNAMLEN + 1;
//	printf("Packet (%d bytes):\n", packetSize);
	int i;
/*    for (i = 0; i < packetSize; i++)
    {
		printf("fileHavePacket[%d] = 0x%02X %c\n", i, packet[i], packet[i]);
    }
*/
    int fd;
    char temp[FNAMLEN+1];
	bzero(temp, FNAMLEN+1);
	strncpy(temp,&eg->data[1],FNAMLEN);
	if ((fd = fileOpen(temp)) != SYSERR)
	{
		if (fileDelete(fd) == SYSERR)
		{
			printf("Unable to overwrite file\n");
			return;
		}
	}
    if ((fd = fileCreate(temp)) != SYSERR)
    {
    	for (i = 1+FNAMLEN; i < DISKBLOCKLEN + FNAMLEN + 1; i++)
    	{
    		filePutChar(fd, eg->data[i]);
    	}
    	fileClose(fd);
    	printf("File Created\n");
    }
    else
    {
    	printf("Unable to make file\n");
    }
}

void fishNoFile(uchar *packet)
{
	printf("File Does Not Exist\n");
}


/*------------------------------------------------------------------------
 * fileSharer - Process that shares files over the network.
 *------------------------------------------------------------------------
 */
int fileSharer(int dev)
{
	uchar packet[PKTSZ];
	int size;
	struct ethergram *eg = (struct ethergram *)packet;

	enable();
	/* Zero out the packet buffer. */
	bzero(packet, PKTSZ);
	bzero(school, sizeof(school));
	bzero(fishlist, sizeof(fishlist));

	/* Lookup canonical MAC in NVRAM, and store in ether struct */
 	colon2mac(nvramGet("et0macaddr"), myMAC);

	while (SYSERR != (size = read(dev, packet, PKTSZ)))
	{
		/* Check packet to see if fileshare type with
		   destination broadcast or me. */
		if ((ntohs(eg->type) == ETYPE_FISH)
			&& ((0 == memcmp(eg->dst, bcast, ETH_ADDR_LEN))
				|| (0 == memcmp(eg->dst, myMAC, ETH_ADDR_LEN))))
		{
			switch (eg->data[0])
			{
			case FISH_ANNOUNCE:
				fishAnnounce(packet);
				break;

			case FISH_PING:
				fishPing(packet);
				break;
			case FISH_DIRASK:
				fishDirAsk(packet);
				break;
			case FISH_DIRLIST:
				fishDirList(packet);
				break;
			case FISH_GETFILE:
				fishGetFile(packet);
				break;
			case FISH_HAVEFILE:
				fishHaveFile(packet);
				break;
			case FISH_NOFILE:
				fishNoFile(packet);
				break;
			default:
				printf("ERROR: Got unhandled FISH type %d\n", eg->data[0]);
			}
		}
	}

	return OK;
}
