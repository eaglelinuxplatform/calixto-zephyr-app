#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include "drivers/ism.h"

LOG_MODULE_REGISTER(ism_test, LOG_LEVEL_DBG);


static void ism_ipm_callback(uint8_t channel, uint8_t state)
{
    LOG_INF("ISM CHANGE ch%u -> %s  (confirmed after %u x %u ms)",
            channel,
            state ? "ACTIVE" : "INACTIVE",
            CONFIG_ISM_CONSECUTIVE_SAMPLES,
            CONFIG_ISM_SAMPLE_PERIOD_MS);
}

int main(void)
{
    LOG_INF("ISM test ) starting");
    LOG_INF("Debounce window: %u samples x %u ms = %u ms",
            CONFIG_ISM_CONSECUTIVE_SAMPLES,
            CONFIG_ISM_SAMPLE_PERIOD_MS,
            CONFIG_ISM_CONSECUTIVE_SAMPLES * CONFIG_ISM_SAMPLE_PERIOD_MS);

    const struct device *ism_dev = DEVICE_DT_GET(DT_INST(0, calixto_ism));

    if (!device_is_ready(ism_dev)) {
        LOG_ERR("ISM device not ready");
        return -ENODEV;
    }

    int ret = ISM_init(ism_dev, ism_ipm_callback);
    if (ret != 0) {
        LOG_ERR("ISM_init failed (err %d)", ret);
        return ret;
    }

    LOG_INF("ISM polling active - callback fires on confirmed changes");

    while (true) {
        k_sleep(K_SECONDS(10));

        for (uint8_t ch = 0; ch < 8; ch++) {
            uint16_t state = 0;
            int rd = ISM_rd_chnl_state(ism_dev, ch, &state);

            if (rd == -ENODEV) {
                break;
            } else if (rd == -EAGAIN) {
                LOG_DBG("ch%u: buffer not yet full", ch);
            } else if (rd == 0) {
                LOG_INF("ch%u: last confirmed state = %u", ch, state);
            }
        }
    }

    return 0;
}
