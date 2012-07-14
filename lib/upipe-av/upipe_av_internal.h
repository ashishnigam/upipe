/*****************************************************************************
 * upipe_av_internal.h: internal interface to av managers
 *****************************************************************************
 * Copyright (C) 2012 OpenHeadend S.A.R.L.
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
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
 *****************************************************************************/

#include <upipe/udeal.h>
#include <upipe/ulog.h>
#include <upipe/upump.h>

#include <stdbool.h>

#include <libavutil/error.h>

/** @hidden */
enum CodecID;

/** structure to protect exclusive access to avcodec_open() */
extern struct udeal upipe_av_deal;

/** @This allocates a watcher triggering when exclusive access to avcodec_open()
 * is granted.
 *
 * @param upump_mgr management structure for this event loop
 * @param cb function to call when the watcher triggers
 * @param opaque pointer to the module's internal structure
 * @return pointer to allocated watcher, or NULL in case of failure
 */
static inline struct upump *upipe_av_deal_upump_alloc(
        struct upump_mgr *upump_mgr, upump_cb cb, void *opaque)
{
    return udeal_upump_alloc(&upipe_av_deal, upump_mgr, cb, opaque);
}

/** @This starts the watcher on exclusive access to avcodec_open().
 *
 * @param upump watcher allocated by @ref udeal_upump_alloc
 * @return false in case of upump error
 */
static inline bool upipe_av_deal_start(struct upump *upump)
{
    return udeal_start(&upipe_av_deal, upump);
}

/** @This tries to grab the exclusive access to avcodec_open().
 *
 * @return true if the resource may be exclusively used
 */
static inline bool upipe_av_deal_grab(void)
{
    return udeal_grab(&upipe_av_deal);
}

/** @This yields exclusive access to avcodec_open() previously acquired from
 * @ref upipe_av_deal_grab.
 *
 * @param upump watcher allocated by @ref udeal_upump_alloc
 * @return false in case of upump error
 */
static inline bool upipe_av_deal_yield(struct upump *upump)
{
    return udeal_yield(&upipe_av_deal, upump);
}

/** @This aborts the watcher before it has had a chance to run. It must only
 * be called in case of abort, otherwise @ref upipe_av_deal_yield does the
 * same job.
 *
 * @param upump watcher allocated by @ref udeal_upump_alloc
 * @return false in case of upump error
 */
static inline bool upipe_av_deal_abort(struct upump *upump)
{
    return udeal_abort(&upipe_av_deal, upump);
}

/** @This wraps around av_strerror() using ulog storage.
 *
 * @param ulog utility structure passed to the module
 * @param errnum avutil error code
 * @return pointer to a buffer containing a description of the error
 */
static inline const char *upipe_av_ulog_strerror(struct ulog *ulog, int errnum)
{
    if (likely(ulog != NULL)) {
        av_strerror(errnum, ulog->ulog_buffer, ULOG_BUFFER_SIZE);
        return ulog->ulog_buffer;
    }
    return "description unvailable";
}

/** @This allows to convert from avcodec ID to flow definition.
 *
 * @param id avcodec ID
 * @return flow definition, or NULL if not found
 */
const char *upipe_av_to_flow_def(enum CodecID id);

/** @This allows to convert to avcodec ID from flow definition.
 *
 * @param flow_def flow definition
 * @return avcodec ID, or 0 if not found
 */
enum CodecID upipe_av_from_flow_def(const char *flow_def);