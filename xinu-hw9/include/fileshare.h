/**
 * @file fileshare.h
 *
 */
/* Embedded Xinu, Copyright (C) 2012.  All rights reserved. */

#include <ether.h>
#include <file.h>

/* A Xinu File Sharing (FISH) packet payload begins with a one byte
   operation code. */
#define FISH_ANNOUNCE 0x01
#define FISH_PING     0x02
#define FISH_DIRASK   0x03
#define FISH_DIRLIST  0x04
#define FISH_GETFILE  0x05
#define FISH_HAVEFILE 0x06
#define FISH_NOFILE   0x07

/* FISH_ANNOUNCE */
/* Packets are unicast, and contain a null-terminated node name,
   followed by zeros padding up to ETHER_MINPAYLOAD size.  A
   FISH_ANNOUNCE packet is sent by each listening node that receives a
   FISH_PING packet. The destination is the FISH_PING source node. */

/* Maximum name length allowed in a FISH_ANNOUNCE packet. */
#define FISH_MAXNAME 25   

/* FISH_PING */
/* Packets are broadcast, and contain zeros padding up to
   ETHER_MINPAYLOAD size.  These are broadcast to the network to elict
   FISH_ANNOUNCE replies. */

/* FISH_DIRASK */
/* Packets are unicast, and contain zeros padding up to
   ETHER_MINPAYLOAD size.  These are sent to known nodes in the school
   to request a directory listing, FISH_DIRLIST. */

/* FISH_DIRLIST */
/* Packets are unicast, and contain a payload of DIRENTRIES * FNAMLEN
   bytes, consisting of the name fields of each entry in the node's
   directory block.  Unused entries should have all zeros.  If this
   payload is smaller than ETHER_MINPAYLOAD, it is padded with
   zeros. */

/* FISH_GETFILE */
/* Packets are unicast, and contain a payload of FNAMLEN bytes,
   consisting of the name of a desired file, followed by zeros padding
   up to ETHER_MINPAYLOAD size. */

/* FISH_HAVEFILE */
/* Packets are unicast and contain two fields:
   FNAMLEN bytes of filename, followed by
   DISKBLOCKLEN bytes of file content. */

/* FISH_NOFILE */
/* Packets are unicast and contain FNAMLEN bytes of filename, followed
   by zeros padding up to ETHER_MINPAYLOAD size. */


/* Fishnode and School are used to store information about other nodes
   running the filesharing protocol on this network. */
struct fishnode
{
	char  used;
	uchar mac[ETH_ADDR_LEN];
	char  name[FISH_MAXNAME];
};
	
#define SCHOOLMAX 10

extern struct fishnode school[SCHOOLMAX];

/* Fishlist stores results from a fish list request (DIRASK). */
extern char   fishlist[DIRENTRIES][FNAMLEN];

/* Prototypes */
int fileSharer(int dev);
