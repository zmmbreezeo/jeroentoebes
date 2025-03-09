/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PATH_CACHE_H
#define _PATH_CACHE_H

#include "los_list.h"
#include "fs/mount.h"
#include "vnode.h"

struct PathCache {
    struct Vnode *parentVnode;    /* vnode points to the cache */
    struct Vnode *childVnode;     /* vnode the cache points to */
    LIST_ENTRY parentEntry;       /* list entry for cache list in the parent vnode */
    LIST_ENTRY childEntry;        /* list entry for cache list in the child vnode */
    LIST_ENTRY hashEntry;         /* list entry for buckets in the hash table */
    uint8_t nameLen;              /* length of path component */
#ifdef LOSCFG_DEBUG_VERSION
    int hit;                      /* cache hit count */
#endif
    char name[0];                 /* path component name */
};

int PathCacheInit(void);
int PathCacheFree(struct PathCache *cache);
struct PathCache *PathCacheAlloc(struct Vnode *parent, struct Vnode *vnode, const char *name, uint8_t len);
int PathCacheLookup(struct Vnode *parent, const char *name, int len, struct Vnode **vnode);
void VnodePathCacheFree(struct Vnode *vnode);
void PathCacheMemoryDump(void);
void PathCacheDump(void);
LIST_HEAD* GetPathCacheList(void);
#ifdef LOSCFG_DEBUG_VERSION
void ResetPathCacheHitInfo(int *hit, int *try);
#endif

#endif /* _PATH_CACHE_H */
