if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM AND ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
    zephyr_library()
    zephyr_library_sources(${ZEPHYR_BASE}/misc/empty_file.c)
    zephyr_library_include_directories(${ZEPHYR_LVGL_MODULE_DIR})
    zephyr_library_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl/)
    zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
    zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
    zephyr_library_sources(custom_status_screen.c)
    if (CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
        zephyr_library_sources(widgets/bongo_cat.c)
        zephyr_library_sources(widgets/bongo_cat_images.c)
    endif()
    if (CONFIG_ZMK_HID_INDICATORS)
        zephyr_library_sources(widgets/hid_indicators.c)

        # The stock ZMK Caps Word behavior keeps its state private and emits
        # no event. Override it on the central/dongle build only, preserving
        # upstream behavior and adding state-change notifications for the OLED.
        # This uses the same source-override mechanism as Prospector ZMK.
        if (CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED)
            zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
            zephyr_library_sources(src/events/caps_word_state_changed.c)
            set_source_files_properties(
                ${APPLICATION_SOURCE_DIR}/src/behaviors/behavior_caps_word.c
                TARGET_DIRECTORY app
                PROPERTIES HEADER_FILE_ONLY ON)
            target_sources(app PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}/src/behaviors/behavior_caps_word.c)
        endif()
    endif()
    if (CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
        zephyr_library_sources(widgets/layer_status.c)
    endif()
    if (CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
        zephyr_library_sources(widgets/modifiers.c)
        zephyr_library_sources(widgets/modifiers_sym.c)
    endif()
        zephyr_library_sources(widgets/output_status.c)
        zephyr_library_sources(widgets/output_status_sym.c)
    if (CONFIG_ZMK_BATTERY)
        zephyr_library_sources(widgets/battery_status.c)
    endif()
    if (CONFIG_ZMK_DONGLE_DISPLAY_WPM)
        zephyr_library_sources(widgets/wpm_status.c)
        zephyr_library_sources(widgets/wpm_status_sym.c)
    endif()
endif()
