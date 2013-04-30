/**
 * @file     xsh_fish.c
 * @provides xsh_fish
 *
 * Modified by
 * Jason Laqua
 * and 
 * Kaleb Breault
 */
/* Embedded XINU, Copyright (C) 2013.  All rights reserved. */

#include <xinu.h>

/**
 * Local function for sending simply FiSh packets with empty payloads.
 * @param dst MAC address of destination
 * @param type FiSh protocal type (see fileshare.h)
 * @return OK for success, SYSERR otherwise.
 */
static int fishSend(uchar *dst, char fishtype)
{
	uchar packet[PKTSZ];
	uchar *ppkt = packet;
	int i = 0;

	// Zero out the packet buffer.
	bzero(packet, PKTSZ);

	for (i = 0; i < ETH_ADDR_LEN; i++)
	{
		*ppkt++ = dst[i];
	}

	// Source: Get my MAC address from the Ethernet device
	control(ETH0, ETH_CTRL_GET_MAC, (long)ppkt, 0);
	ppkt += ETH_ADDR_LEN;
		
	// Type: Special "3250" packet protocol.
	*ppkt++ = ETYPE_FISH >> 8;
	*ppkt++ = ETYPE_FISH & 0xFF;
		
	*ppkt++ = fishtype;
		
	for (i = 1; i < ETHER_MINPAYLOAD; i++)
	{
		*ppkt++ = i;
	}
		
	write(ETH0, packet, ppkt - packet);
	
	return OK;
}

/**
 * Local function for sending simply FiSh packets with empty payloads.
 * @param dst MAC address of destination
 * @param type FiSh protocal type (see fileshare.h)
 * @return OK for success, SYSERR otherwise.
 */
static int fishSendPayload(uchar *dst, char fishtype, char *payload)
{
	uchar packet[PKTSZ];
	uchar *ppkt = packet;
	int i = 0;

	// Zero out the packet buffer.
	bzero(packet, PKTSZ);

	for (i = 0; i < ETH_ADDR_LEN; i++)
	{
		*ppkt++ = dst[i];
	}

	// Source: Get my MAC address from the Ethernet device
	control(ETH0, ETH_CTRL_GET_MAC, (long)ppkt, 0);
	ppkt += ETH_ADDR_LEN;
		
	// Type: Special "3250" packet protocol.
	*ppkt++ = ETYPE_FISH >> 8;
	*ppkt++ = ETYPE_FISH & 0xFF;
		
	*ppkt++ = fishtype;
	
	memcpy(ppkt, payload, FNAMLEN);
	ppkt += FNAMLEN;
		
	for (i = 1; i < ETHER_MINPAYLOAD; i++)
	{
		*ppkt++ = 0;
	}
	
/*	printf("packet contents:");
	for (i = 0; i < ppkt - packet; i++)
	{
		printf("outgoingPacket[%d]: 0x%02X %c\n", i, packet[i], packet[i]);
	}
*/		
	write(ETH0, packet, ppkt - packet);
	
	return OK;
}

static int fishSendFilePayload(uchar *dst, char fishtype, char *payload)
{
	uchar packet[PKTSZ];
	uchar *ppkt = packet;
	struct ethergram *eg = (struct ethergram *)packet;
	int i = 0;

	// Zero out the packet buffer.
	bzero(packet, PKTSZ);

	for (i = 0; i < ETH_ADDR_LEN; i++)
	{
		*ppkt++ = dst[i];
	}

	// Source: Get my MAC address from the Ethernet device
	control(ETH0, ETH_CTRL_GET_MAC, (long)ppkt, 0);
	ppkt += ETH_ADDR_LEN;
		
	// Type: Special "3250" packet protocol.
	*ppkt++ = ETYPE_FISH >> 8;
	*ppkt++ = ETYPE_FISH & 0xFF;
		
	*ppkt++ = fishtype;
	
	memcpy(ppkt, payload, FNAMLEN);
	ppkt += FNAMLEN;
			
	/* FISH type becomes HAVEFILE */
	eg->data[0] = FISH_HAVEFILE;
	
	int payloadSize = DISKBLOCKLEN + FNAMLEN + 1;
	int fd = fileOpen(payload);
	i = 1+FNAMLEN;
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
	
/*	printf("packet contents:");
	for (i = 0; i < ppkt - packet; i++)
	{
		printf("outgoingPacket[%d]: 0x%02X %c\n", i, packet[i], packet[i]);
	}
*/		
	write(ETH0, packet, ETHER_SIZE + FNAMLEN + DISKBLOCKLEN + 1);
	
	return OK;
}
		
/**
 * Shell command (fish) is file sharing client.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_fish(ushort nargs, char *args[])
{
	uchar bcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	int i = 0;

	if (nargs == 2 && strncmp(args[1], "ping", 4) == 0)
    {
		fishSend(bcast, FISH_PING);
		
		sleep(1000);

		printf("Known FISH nodes in school:\n");
		for (i = 0; i < SCHOOLMAX; i++)
		{
			if (school[i].used)
			{
				printf("\t%02X:%02X:%02X:%02X:%02X:%02X",
					   school[i].mac[0],
					   school[i].mac[1],
					   school[i].mac[2],
					   school[i].mac[3],
					   school[i].mac[4],
					   school[i].mac[5]);
				printf("\t%s\n", school[i].name);
			}
		}
		printf("\n");
		return OK;
	}
	else if (nargs == 3 && strncmp(args[1], "list", 4) == 0)
	{
		//   Locate named node in school,
		//   and send a FISH_DIRASK packet to it.
		//   Wait one second for reply to come in, and
		//   then print contents of fishlist table.
		int i;
		for (i = 0; i < SCHOOLMAX; i++)
		{
			if (strncmp(school[i].name, args[2], FISH_MAXNAME) == 0)
				break;
		}
		if (i == SCHOOLMAX)
		{
			printf("No FiSh \"%s\" found in school.\n", args[2]);
			return OK;
		}
		
		bzero(fishlist, sizeof(fishlist));
		int filesFound = 0;
		
		fishSend(school[i].mac, FISH_DIRASK);
		
		sleep(1000);
		
		char temp[FNAMLEN+1];
		bzero(temp, FNAMLEN+1);
		
		printf("Files on %s:\n", args[2]);
		for (i = 0; i < DIRENTRIES; i++)
		{
			//printf("i = %d fishlist[i] length: %d\n", i, strlen(fishlist[i]));
			if ((fishlist[i][0] != '\0') && (strnlen(fishlist[i], FNAMLEN) != 0))
			{
				strncpy(temp,fishlist[i],FNAMLEN);
				printf("\t%s\n",temp);
				filesFound++;
			}
		}
		printf("%d files found\n", filesFound);
				
		return OK;
	}
	else if (nargs == 4 && strncmp(args[1], "get", 4) == 0)
	{
		//   Locate named node in school,
		//   and send a FISH_GETFILE packet to it.
		//   FileSharer puts file in system when it arrives.
		
		int i;
		for (i = 0; i < SCHOOLMAX; i++)
		{
			if (strncmp(school[i].name, args[2], FISH_MAXNAME) == 0)
				break;
		}
		if (i == SCHOOLMAX)
		{
			printf("No FiSh \"%s\" found in school.\n", args[2]);
			return OK;
		}
		
		fishSendPayload(school[i].mac, FISH_GETFILE, args[3]);
		
		return OK;
	}
	else if (nargs == 4 && strncmp(args[1], "send", 5) == 0)
	{
		//	Send file to target node in school
		int i;
		for (i = 0; i < SCHOOLMAX; i++)
		{
			if (strncmp(school[i].name, args[2], FISH_MAXNAME) == 0)
				break;
		}
		if (i == SCHOOLMAX)
		{
			printf("No FiSh \"%s\" found in school.\n", args[2]);
			return OK;
		}
		int fd;
		if ((fd = fileOpen(args[3])) == SYSERR)
		{
			printf("File \"%s\" does not exist.\n", args[3]);
			return OK;
		}
		fishSendFilePayload(school[i].mac, FISH_GETFILE, args[3]);
		
		return OK;
		
	}
	else
    {
        fprintf(stdout, "Usage: fish [ping | list <node> | get <node> <filename>]\n");
        fprintf(stdout, "Simple file sharing protocol.\n");
        fprintf(stdout, "ping - lists available FISH nodes in school.\n");
        fprintf(stdout, "list <node> - lists directory contents at node.\n");
        fprintf(stdout, "get <node> <file> - requests a remote file.\n");
        fprintf(stdout, "\t--help\t display this help and exit\n");

        return 0;
    }



    return OK;
}
