/*
 * Copyright (c) 2015, 2016 VMware, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OVS_CONNTRACK_H_
#define __OVS_CONNTRACK_H_ 1

#include "precomp.h"
#include "Flow.h"

struct ct_addr {
    union {
        ovs_be32 ipv4;
        struct in6_addr ipv6;
        uint32_t ipv4_aligned;
        struct in6_addr ipv6_aligned;
    };
};

struct ct_endpoint {
    struct ct_addr addr;
    ovs_be16 port;
    UINT16 pad;
};

typedef enum CT_UPDATE_RES {
    CT_UPDATE_INVALID,
    CT_UPDATE_VALID,
    CT_UPDATE_NEW,
} CT_UPDATE_RES;

/* Metadata mark for masked write to conntrack mark */
typedef struct MD_MARK {
    UINT32 value;
    UINT32 mask;
} MD_MARK;

/* Metadata label for masked write to conntrack label. */
typedef struct MD_LABELS {
    struct ovs_key_ct_labels value;
    struct ovs_key_ct_labels mask;
} MD_LABELS;

typedef struct _OVS_CT_KEY {
    struct ct_endpoint src;
    struct ct_endpoint dst;
    UINT16 dl_type;
    UINT8 nw_proto;
    UINT16 zone;
} OVS_CT_KEY, *POVS_CT_KEY;

typedef struct OVS_CT_ENTRY {
    OVS_CT_KEY  key;
    OVS_CT_KEY  rev_key;
    UINT64      expiration;
    LIST_ENTRY  link;
    UINT32      mark;
    struct ovs_key_ct_labels labels;
} OVS_CT_ENTRY, *POVS_CT_ENTRY;

typedef struct OvsConntrackKeyLookupCtx {
    OVS_CT_KEY      key;
    POVS_CT_ENTRY   entry;
    UINT32          hash;
    BOOLEAN         reply;
    BOOLEAN         related;
} OvsConntrackKeyLookupCtx;

#define CT_HASH_TABLE_SIZE ((UINT32)1 << 10)
#define CT_HASH_TABLE_MASK (CT_HASH_TABLE_SIZE - 1)
#define CT_ENTRY_TIMEOUT (2 * 600000000)   // 2m
#define CT_CLEANUP_INTERVAL (2 * 600000000) // 2m
/* Given POINTER, the address of the given MEMBER in a STRUCT object, returns
   the STRUCT object. */
#define CONTAINER_OF(POINTER, STRUCT, MEMBER)                           \
        ((STRUCT *) (void *) ((char *) (POINTER) - \
         offsetof (STRUCT, MEMBER)))

VOID OvsCleanupConntrack(VOID);
NTSTATUS OvsInitConntrack(POVS_SWITCH_CONTEXT context);

NDIS_STATUS OvsExecuteConntrackAction(PNET_BUFFER_LIST curNbl,
                                      OVS_PACKET_HDR_INFO *layers,
                                      OvsFlowKey *key,
                                      const PNL_ATTR a);
BOOLEAN OvsConntrackValidateTcpPacket(const TCPHdr *tcp);
OVS_CT_ENTRY * OvsConntrackCreateTcpEntry(const TCPHdr *tcp,
                                          PNET_BUFFER_LIST nbl,
                                          UINT64 now);
enum CT_UPDATE_RES OvsConntrackUpdateTcpEntry(OVS_CT_ENTRY* conn_,
                                              const TCPHdr *tcp,
                                              PNET_BUFFER_LIST nbl,
                                              BOOLEAN reply,
                                              UINT64 now);
#endif /* __OVS_CONNTRACK_H_ */