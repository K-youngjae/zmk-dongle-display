/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/caps_word_state_changed.h>

#include "caps_word_status.h"

struct caps_word_status_state {
    bool active;
};

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void set_caps_word_status(lv_obj_t *label, struct caps_word_status_state state) {
    if (state.active) {
        lv_label_set_text(label, "WORD");
    } else {
        lv_label_set_text(label, "");
    }
}

void caps_word_status_update_cb(struct caps_word_status_state state) {
    struct zmk_widget_caps_word_status *widget;

    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_caps_word_status(widget->obj, state);
    }
}

static struct caps_word_status_state caps_word_status_get_state(const zmk_event_t *eh) {
    struct zmk_caps_word_state_changed *ev = as_zmk_caps_word_state_changed(eh);

    return (struct caps_word_status_state){
        .active = ev->active,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_word_status,
                            struct caps_word_status_state,
                            caps_word_status_update_cb,
                            caps_word_status_get_state)

ZMK_SUBSCRIPTION(widget_caps_word_status, zmk_caps_word_state_changed);

int zmk_widget_caps_word_status_init(struct zmk_widget_caps_word_status *widget,
                                     lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_label_set_text(widget->obj, "");

    sys_slist_append(&widgets, &widget->node);

    widget_caps_word_status_init();

    return 0;
}

lv_obj_t *zmk_widget_caps_word_status_obj(struct zmk_widget_caps_word_status *widget) {
    return widget->obj;
}
