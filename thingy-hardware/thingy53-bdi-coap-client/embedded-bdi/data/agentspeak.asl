+user_turn_on <- !!preserve_light.

+!preserve_light : bright_inside & user_turn_on <- !!preserve_light.

+!preserve_light : dark_inside & user_turn_on <- !brighten ; !!preserve_light.

-!preserve_light : user_turn_on <- !!preserve_light.

+sunny_outside: user_turn_on <- +eco_mode_available.

+cloudy_outside: user_turn_on <- -eco_mode_available ; +standard_mode_available.

+night_outside: user_turn_on <- -eco_mode_available ; +standard_mode_available.

+!brighten: eco_mode_available <- turn_off_lights ; raise_blinds;  -eco_mode_available.

+!brighten: standard_mode_available <- turn_on_lights; -standard_mode_available.

+user_turn_off <- !!preserve_dark.

+!preserve_dark: night_outside & user_turn_off <- lower_blinds ; !!preserve_dark.

+!preserve_dark: bright_inside & user_turn_off  <- turn_off_lights; lower_blinds ; !!preserve_dark.

-!preserve_dark: user_turn_off <- !!preserve_dark.
