/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/caps_word_state_changed.h>

#include "hid_indicators.h"

// The host reports Caps Lock through HID LEDs. Caps Word is a local ZMK
// behavior, so we listen to its extra state-change event separately.
#define LED_CLCK 0x02

struct hid_indicators_state {
    bool caps_lock;
    bool caps_word;
};

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static bool caps_lock_active;
static bool caps_word_active;

static void set_hid_indicators(lv_obj_t *label, struct hid_indicators_state state) {
    if (state.caps_lock) {
        // Caps Lock takes precedence if both modes are enabled.
        lv_label_set_text(label, "CAPS");
        lv_obj_set_style_pad_right(label, 0, LV_PART_MAIN);
        lv_obj_remove_flag(label, LV_OBJ_FLAG_HIDDEN);
    } else if (state.caps_word) {
        lv_label_set_text(label, "WORD");
        // Nudge only WORD's text 1px left inside the fixed 40x10 border.
        // Right padding of 2px moves centered content left by 1px.
        lv_obj_set_style_pad_right(label, 2, LV_PART_MAIN);
        lv_obj_remove_flag(label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
}

static void hid_indicators_update_cb(struct hid_indicators_state state) {
    struct zmk_widget_hid_indicators *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_hid_indicators(widget->obj, state);
    }
}

static struct hid_indicators_state hid_indicators_get_state(const zmk_event_t *eh) {
    if (eh != NULL) {
        struct zmk_hid_indicators_changed *host =
            as_zmk_hid_indicators_changed(eh);
        if (host != NULL) {
            caps_lock_active = (host->indicators & LED_CLCK) != 0;
        }

        struct zmk_caps_word_state_changed *word =
            as_zmk_caps_word_state_changed(eh);
        if (word != NULL) {
            caps_word_active = word->active;
        }
    }

    return (struct hid_indicators_state){
        .caps_lock = caps_lock_active,
        .caps_word = caps_word_active,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_hid_indicators, struct hid_indicators_state,
                            hid_indicators_update_cb, hid_indicators_get_state)

ZMK_SUBSCRIPTION(widget_hid_indicators, zmk_hid_indicators_changed);
ZMK_SUBSCRIPTION(widget_hid_indicators, zmk_caps_word_state_changed);

int zmk_widget_hid_indicators_init(struct zmk_widget_hid_indicators *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_label_set_text(widget->obj, "CAPS");
    lv_label_set_long_mode(widget->obj, LV_LABEL_LONG_CLIP);

    // A 40x10 outlined indicator fits directly below the 50x26 cat.
    lv_obj_set_size(widget->obj, 40, 10);
    lv_obj_set_style_text_align(widget->obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(widget->obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_flag(widget->obj, LV_OBJ_FLAG_HIDDEN);

    sys_slist_append(&widgets, &widget->node);

    widget_hid_indicators_init();

    return 0;
}

lv_obj_t *zmk_widget_hid_indicators_obj(struct zmk_widget_hid_indicators *widget) {
    return widget->obj;
}
