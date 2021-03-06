/*
 * Copyright (C) 2013 OpenHeadend S.A.R.L.
 *
 * Authors: Christophe Massiot
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file
 * @short probe catching clock_ref and clock_ts events for dejittering
 */

#ifndef _UPIPE_UPROBE_DEJITTER_H_
/** @hidden */
#define _UPIPE_UPROBE_DEJITTER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <upipe/uprobe.h>
#include <upipe/uprobe_helper_uprobe.h>

#include <stdint.h>

/** @This is a super-set of the uprobe structure with additional local
 * members. */
struct uprobe_dejitter {
    /** number of references to average */
    unsigned int divider;

    /** number of references received for offset calculaton */
    unsigned int offset_count;
    /** offset between stream clock and system clock */
    int64_t offset;
    /** residue */
    int64_t offset_residue;

    /** number of references received for deviation calculaton */
    unsigned int deviation_count;
    /** average absolute deviation */
    uint64_t deviation;
    /** residue */
    uint64_t deviation_residue;

    /** structure exported to modules */
    struct uprobe uprobe;
};

UPROBE_HELPER_UPROBE(uprobe_dejitter, uprobe)

/** @This initializes an already allocated uprobe_dejitter structure.
 *
 * @param uprobe_pfx pointer to the already allocated structure
 * @param next next probe to test if this one doesn't catch the event
 * @param divider number of reference clocks to keep for dejittering
 * @return pointer to uprobe, or NULL in case of error
 */
struct uprobe *uprobe_dejitter_init(struct uprobe_dejitter *uprobe_dejitter,
                                    struct uprobe *next, unsigned int divider);

/** @This cleans a uprobe_dejitter structure.
 *
 * @param uprobe_dejitter structure to clean
 */
void uprobe_dejitter_clean(struct uprobe_dejitter *uprobe_dejitter);

/** @This allocates a new uprobe_dejitter structure.
 *
 * @param next next probe to test if this one doesn't catch the event
 * @param divider number of reference clocks to keep for dejittering
 * @return pointer to uprobe, or NULL in case of error
 */
struct uprobe *uprobe_dejitter_alloc(struct uprobe *next, unsigned int divider);

/** @This sets a different divider. If set to 0, dejittering is disabled.
 *
 * @param uprobe pointer to probe
 * @param divider number of reference clocks to keep for dejittering
 */
void uprobe_dejitter_set(struct uprobe *uprobe, unsigned int divider);

#ifdef __cplusplus
}
#endif
#endif
