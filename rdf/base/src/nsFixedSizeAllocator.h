/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Original Author(s):
 *   Chris Waterson <waterson@netscape.com
 *
 * Contributor(s): 
 */

/*

  A simple fixed-size allocator that allocates its memory from an
  arena.

  Although the allocator can handle blocks of any size, its
  preformance will degrade rapidly if used to allocate blocks of
  arbitrary size. Ideally, it should be used to allocate and recycle a
  large number of fixed-size blocks.

*/

#ifndef nsFixedSizeAllocator_h__
#define nsFixedSizeAllocator_h__

#include "nscore.h"
#include "nsError.h"
#include "plarena.h"

#define NS_SIZE_IN_HEAP(_size) ((_size) + sizeof(double))

class nsFixedSizeAllocator
{
protected:
    PLArenaPool mPool;

    struct Bucket;
    struct FreeEntry;
  
    friend struct Bucket;
    friend struct FreeEntry;

    struct FreeEntry {
        Bucket*    mBucket;
        FreeEntry* mNext;
    };

    struct Bucket {
        size_t     mSize;
        FreeEntry* mFirst;
        Bucket*    mNext;
    };

    Bucket* mBuckets;

    nsresult
    AddBucket(size_t aSize);

public:
    nsFixedSizeAllocator() : mBuckets(nsnull) {}

    ~nsFixedSizeAllocator() {
        if (mBuckets)
            PL_FinishArenaPool(&mPool); }

    /**
     * Initialize the fixed size allocator. 'aName' is used to tag
     * the underlying PLArena object for debugging and measurement
     * purposes. 'aNumBuckets' specifies the number of elements in
     * 'aBucketSizes', which is an array of integral block sizes
     * that this allocator should be prepared to handle.
     */
    nsresult
    Init(const char* aName,
         const size_t* aBucketSizes,
         PRInt32 aNumBuckets,
         PRInt32 aInitialSize,
         PRInt32 aAlign = 0);

    /**
     * Allocate a block of memory 'aSize' bytes big.
     */
    void* Alloc(size_t aSize);

    /**
     * Free a pointer allocated using a fixed-size allocator
     */
    static void Free(void* aPtr, size_t aSize);
};



#endif // nsFixedSizeAllocator_h__
