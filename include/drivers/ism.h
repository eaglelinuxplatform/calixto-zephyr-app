#ifndef INCLUDE_DRIVERS_ISM_ISM_H_
#define INCLUDE_DRIVERS_ISM_ISM_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/toolchain.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ism_callback_t)(uint8_t channel, uint8_t state);

__subsystem struct ism_driver_api {

    int (*init)(const struct device *dev,
                ism_callback_t cb);

    int (*deinit)(const struct device *dev);

    int (*rd_chnl_state)(const struct device *dev,
                         uint8_t chnl,
                         uint16_t *chnl_state);
};

__syscall int ISM_init(const struct device *dev,
                         ism_callback_t cb);

static inline int z_impl_ISM_init(const struct device *dev,
                            ism_callback_t cb)
{
    __ASSERT_NO_MSG(dev != NULL && dev->api != NULL && cb != NULL);

    const struct ism_driver_api *api =
        (const struct ism_driver_api *)dev->api;

    return api->init(dev, cb);
}

__syscall int ISM_deinit(const struct device *dev);

static inline int z_impl_ISM_deinit(const struct device *dev)
{
    __ASSERT_NO_MSG(dev != NULL && dev->api != NULL);

    const struct ism_driver_api *api =
        (const struct ism_driver_api *)dev->api;

    return api->deinit(dev);
}

__syscall int ISM_rd_chnl_state(const struct device *dev,
                                 uint8_t chnl,
                                 uint16_t *chnl_state);
static inline int z_impl_ISM_rd_chnl_state(const struct device *dev,
                                     uint8_t chnl,
                                     uint16_t *chnl_state)
{
    __ASSERT_NO_MSG(dev != NULL && dev->api != NULL && chnl_state != NULL);

    const struct ism_driver_api *api =
        (const struct ism_driver_api *)dev->api;

    return api->rd_chnl_state(dev, chnl, chnl_state);
}


#include <syscalls/ism.h>

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_ISM_ISM_H_ */
