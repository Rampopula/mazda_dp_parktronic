#ifndef __MAZDA_DP_PARKTRONIC_H__
#define __MAZDA_DP_PARKTRONIC_H__

#include "boardinfo.h"
#include "log.h"

/**
 * @brief Initialize Mazda Display Parktronic application.
 *        This function must be called before the main loop.
 */
void mdp_init(void);

/**
 * @brief Run Mazda Display Parktronic application.
 *        This function must be called constantly in the main loop.
 */
void mdp_run(void);

#endif /* __MAZDA_DP_PARKTRONIC_H__ */
