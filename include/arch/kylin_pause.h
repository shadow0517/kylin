#ifndef _KYLIN_PAUSE_H_
#define _KYLIN_PAUSE_H_

/**
 * @file
 *
 * CPU pause operation.
 *
 */

/**
 * Pause CPU execution for a short while
 *
 * This call is intended for tight loops which poll a shared resource or wait
 * for an event. A short pause within the loop may reduce the power consumption.
 */
static inline void kylin_pause(void);

#endif /*_KYLIN_PAUSE_H_*/
