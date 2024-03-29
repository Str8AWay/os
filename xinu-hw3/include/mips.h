/**
 * @file mips.h
 *
 * Define human readable registers, coprocessor 0 registers,
 * cause and status register masks, memory segment addresses, context record
 * offsets, and cache functions and register offsets.
 *
 * $Id: mips.h 184 2007-07-12 22:52:09Z agember $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#ifndef	_MIPS_H
#define	_MIPS_H

/**
 * Human readable register names
 */

#define zero $0        /**< wired zero                                   */
#define AT   $1        /**< asm temp - uppercase because of ".set at"    */
#define v0   $2        /**< return value                                 */
#define v1   $3
#define a0   $4        /**< argument 0 to 3                              */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8        /**< caller saved 0 to 7                          */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16       /**< callee saved 0 to 7                          */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24       /**< caller saved 8 and 9                         */
#define t9   $25
#define jp   $25       /**< PIC jump register                            */
#define k0   $26       /**< kernel scratch                               */
#define k1   $27
#define gp   $28       /**< global pointer                               */
#define sp   $29       /**< stack pointer                                */
#define fp   $30       /**< frame pointer                                */
#define s8   $30       /**< same like fp!                                */
#define ra   $31       /**< return address                               */

/**
 * Coprocessor 0 registers
 */
#define CP0_INDEX    $0  /**< Index pointer into TLB                     */
#define CP0_RANDOM   $1  /**< Random pointer into TLB                    */
#define CP0_ENTRYLO0 $2  /**< Even page TLB entry                        */
#define CP0_ENTRYLO1 $3  /**< Odd page TLB entry                         */
#define CP0_CONTEXT  $4  /**< Hold PTE base and VPN on TLB exception     */
#define CP0_PGMASK   $5  /**< Mask for virtual address on TLB matching   */
#define CP0_WIRED    $6  /**< Boundary between random and wired entries  */
#define CP0_BADVADDR $8  /**< Bad address generated by TLB exceptions    */
#define CP0_COUNT    $9  /**< Timer counter                              */
#define CP0_ENTRYHI  $10 /**< Hi half of TLB (VPN+ASID)                  */
#define CP0_COMPARE  $11 /**< interrupt when CP0_COUNT == CP0_COMPARE    */
#define CP0_STATUS   $12 /**< Various run time processor information     */
#define CP0_CAUSE    $13 /**< Identifies cause of interrupt/exception    */
#define CP0_EPC      $14 /**< Return address after exception handling    */
#define CP0_PRID     $15 /**< processor identification                   */
#define CP0_CONFIG   $16 /**< configuration register (select 0, 1)       */
#define CP0_DIAG     $22 /**< implementation dependent (diagnostic?)     */
#define CP0_TAGLO    $28 
#define CP0_TAGHI    $29
#define CP0_ERREPC   $30

/**
 * Processor ID masks
 */
#define PRID_REV   0x000000FF   /**< Revision                            */
#define PRID_CPUID 0x0000FF00   /**< CPU ID                              */

/**
 * Platform specific values and macros
 */

/* Processor revisions */
#define PRID_REV_WRT54G     0x29
#define PRID_REV_WRT54GL    0x08
#define PRID_REV_WRT350N    0x1A

/**
 * Cause register interrupt masks
 */
#define CAUSE_EXC 0x0000007C   /**< Exception Code                       */
#define CAUSE_SW0 0x00000100   /**< Software interrupt                   */
#define CAUSE_SW1 0x00000200
#define CAUSE_HW0 0x00000400   /**< Hardware interrupt                   */
#define CAUSE_HW1 0x00000800   /**< UART interrupt                       */
#define CAUSE_HW2 0x00001000
#define CAUSE_HW3 0x00002000
#define CAUSE_HW4 0x00004000
#define CAUSE_HW5 0x00008000   /**< Timer interrupt                      */

/**
 * Status register masks
 */
#define STATUS_IE  0x00000001  /**< Global interrupt enable              */
#define STATUS_EXL 0x00000002  /**< Exception Level                      */
#define STATUS_ERL 0x00000004  /**< Error Level                          */
#define STATUS_SW0 CAUSE_SW0   /**< Software interrupt enable            */
#define STATUS_SW1 CAUSE_SW1
#define STATUS_HW0 CAUSE_HW0   /**< Hardware interrupt enable            */
#define STATUS_HW1 CAUSE_HW1   /**< UART interrupt enable                */
#define STATUS_HW2 CAUSE_HW2
#define STATUS_HW3 CAUSE_HW3
#define STATUS_HW4 CAUSE_HW4
#define STATUS_HW5 CAUSE_HW5   /**< Timer interrupt enable               */

/**
 * Define kernel memory segments for MIPS32 processors
 */
#define KUSEG_BASE 0x00000000  /**< userspace mapped base                */
#define KUSEG_SIZE 0x80000000  /**< userspace mapped (2 GB)              */
#define KSEG0_BASE 0x80000000  /**< kernel unmapped, cached base         */
#define KSEG0_SIZE 0x20000000  /**< kernel unmapped, cahced (512 MB)     */
#define KSEG1_BASE 0xA0000000  /**< kernel unmapped, uncached base       */
#define KSEG1_SIZE 0x20000000  /**< kernel unmapped, uncached (512 MB)   */
#define KSEG2_BASE 0xC0000000  /**< kernel mapped base                   */
#define KSEG2_SIZE 0x40000000  /**< kernel mapped (1 GB)                 */

/**
 * Context record offsets
 */
#define CONTEXT 64             /**< context record size in bytes         */
                               /**< context record size in words         */
#define CONTEXT_WORDS (CONTEXT / sizeof(long))

#define RA_CON  (CONTEXT - 8)  /**< return address                       */
#define S0_CON  52             /**< callee saved registers               */
#define S1_CON  48
#define S2_CON  44
#define S3_CON  40
#define S4_CON  36
#define S5_CON  32
#define S6_CON  28
#define S7_CON  24
#define GP_CON  20             /**< global pointer                       */
#define FP_CON  16             /**< frame pointer                        */

/** 
 * Cache register locations within the Config1 register 
 */
#define CONFIG1_IS    22       /**< instruction cache                    */
#define CONFIG1_IL    19
#define CONFIG1_IA    16
#define CONFIG1_DS    13       /**< data cache                           */
#define CONFIG1_DL    10
#define CONFIG1_DA     7
#define CONFIG1_MASK   7       /**< value mask                           */

/**
 * Cache functions 
 */
#define INDEX_STORE_TAG_I  0x8  /**< invalidate instruction cache tag    */
#define FILL_I_CACHE       0x14 /**< fill instruction cache              */
#define INDEX_STORE_TAG_D  0x9  /**< invalidate data cache tag           */

#endif	/* _MIPS_H */
