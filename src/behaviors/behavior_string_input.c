/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_string_input

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

//#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct string_input_break_item {
    uint16_t page;
    uint32_t id;
    uint8_t implicit_modifiers;
};

struct behavior_string_input_config {
    struct zmk_behavior_binding accept_behavior;
    struct zmk_behavior_binding cancel_behavior;
    uint8_t accept_count;
    struct string_input_break_item accept_list[CONFIG_ZMK_BEHAVIOR_STRING_INPUT_MAX_CONTINUE_LIST_SIZE];
    uint8_t cancel_count;
    struct string_input_break_item cancel_list[CONFIG_ZMK_BEHAVIOR_STRING_INPUT_MAX_CONTINUE_LIST_SIZE];
};

static int xxx = 0;

static void log_string_input_config(const struct behavior_string_input_config *cfg) {
    LOG_DBG("String Input Behavior Configuration:");
    LOG_DBG("accept behavior: %s", cfg->accept_behavior.behavior_dev);
    LOG_DBG("cancel behavior: %s", cfg->cancel_behavior.behavior_dev);
    for (int i = 0; i < cfg->accept_count; i++) {
        LOG_DBG("accept item %d: page %04x, id %08x, implicit_modifiers %02x",
                i, cfg->accept_list[i].page, cfg->accept_list[i].id,
                cfg->accept_list[i].implicit_modifiers);
    }
    for (int i = 0; i < cfg->cancel_count; i++) {
        LOG_DBG("cancel item %d: page %04x, id %08x, implicit_modifiers %02x",
                i, cfg->cancel_list[i].page, cfg->cancel_list[i].id,
                cfg->cancel_list[i].implicit_modifiers);
    }
    LOG_DBG("xxx: %d", xxx);
}

struct behavior_string_input_data {
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    struct behavior_parameter_metadata_set set;
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

static int on_string_input_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_string_input_config *cfg = dev->config;

    log_string_input_config(cfg);

    LOG_DBG("String input binding pressed; binding: %s; event.position: %d", binding->behavior_dev, event.position);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_string_input_binding_released(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_string_input_config *cfg = dev->config;

    LOG_DBG("String input binding released; binding: %s; event.position: %d", binding->behavior_dev, event.position);

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_string_input_driver_api = {
    .binding_pressed = on_string_input_binding_pressed,
    .binding_released = on_string_input_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};


static int behavior_string_input_init(const struct device *dev) {
    static bool init_first_run = true;

    LOG_DBG("string_input behavior init - first run");

    if (init_first_run) {
        LOG_DBG("string_input behavior init - first run");
        const struct behavior_string_input_config *cfg = dev->config;
        log_string_input_config(cfg);
        ++xxx;
    }
    ++xxx;
    init_first_run = false;
    return 0;
}

#define PARSE_BREAK(i)                                                                              \
    {.page = ZMK_HID_USAGE_PAGE(i), .id = ZMK_HID_USAGE_ID(i), .implicit_modifiers = SELECT_MODS(i)}

#define BREAK_ITEM_ACCEPT(i, n) PARSE_BREAK(DT_INST_PROP_BY_IDX(n, accept_list, i))
#define BREAK_ITEM_CANCEL(i, n) PARSE_BREAK(DT_INST_PROP_BY_IDX(n, cancel_list, i))

#define SI_INST(n)                                                                                  \
    static const struct behavior_string_input_config behavior_string_input_config_##n = {           \
        .accept_behavior = ZMK_KEYMAP_EXTRACT_BINDING(0, DT_DRV_INST(n)),                           \
        .cancel_behavior = ZMK_KEYMAP_EXTRACT_BINDING(1, DT_DRV_INST(n)),                           \
        .accept_count = DT_INST_PROP_LEN(n, accept_list),                                           \
        .accept_list = {LISTIFY(DT_INST_PROP_LEN(n, accept_list), BREAK_ITEM_ACCEPT, (, ), n)},     \
        .cancel_count = DT_INST_PROP_LEN(n, cancel_list),                                           \
        .cancel_list = {LISTIFY(DT_INST_PROP_LEN(n, cancel_list), BREAK_ITEM_CANCEL, (, ), n)},     \
    };                                                                                              \
    static struct behavior_string_input_data behavior_string_input_data_##n = {};                   \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_string_input_init, NULL, &behavior_string_input_data_##n,   \
                            &behavior_string_input_config_##n, POST_KERNEL,                         \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_string_input_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SI_INST)

//#endif
