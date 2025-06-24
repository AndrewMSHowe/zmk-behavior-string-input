if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_STRING_INPUT app PRIVATE src/behaviors/behavior_string_input.c)
endif()
