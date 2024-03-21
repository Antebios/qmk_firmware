/* Copyright 2024 Richard Nunez - Antebios
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "print.h"

// Keep track of key presses for Modifier
typedef struct {
  bool is_press_action;
  int state;
} tap;

// Key Tap enumerator
enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  TRIPLE_TAP = 5,
  TRIPLE_HOLD = 6
};

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum custom_layer {
  _QWERTY,
  _MIRROR,
  _ALT,
  _CTRL,
  _LOWER,
  _RAISE,
  _MOUSE,
  _ADJUST,
  _CORNE_QWERTY
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  ALT,
  CTRL,
  LOWER,
  RAISE,
  MOUSE,
  ADJUST,
  DRAG_SCROLL,
  MSTEAMS_MUTEMIC,
  MSTEAMS_ANSWER_CALL,
  MSTEAMS_END_CALL
};

// TAP DANCE ***********************************************************
//Tap Dance Declarations
enum {
  ALT_OSL1 = 0,
  CTL_OSL1,
  TD_DEL_BSPC ,
  TD_ESC_GRAVE,
  TD_TAB_TILDE,
  TD_9_LPRN,
  TD_0_RPRN,
  TD_MINS_UNDS,
  TD_EQL_PLUS,
  TD_LBRC_LCBR,
  TD_RBRC_RCBR,
  TD_SCLN_COLN,
  TD_QUOT_DQT,
  TD_COMM_LABK,
  TD_DOT_RABK,
  TD_SLSH_QUES,
  TD_SMCL_CLN,
  TD_BSLS_PIPE,
  TD_SMCL_CLN,

  TD_PGUP_HOME,
  TD_PGDN_END,

  TD_Q_LrALT,
  TD_W_Lyr2,
  TD_E_Lyr3,
  TD_R_LrKey,
  TD_T_LrMS,

  TD_SHIFT_CAPS,
  TD_SPC_ENT,
  TD_SPC_BKSPC,
  TD_LSPACE,
  TD_RSPACE,

  TD_LayerDn,
  TD_LayerUp,
  TD_Q_BSPC,
  TD_W_ESC,
  TD_S,
  TD_G_ENT,
  TD_LayerLower,
  TD_LayerRaise,
  TD_Z_UNDO,
  TD_X_CUT,
  TD_C_COPY,
  TD_V_PASTE
};


// Alt key action:
// SINGLE_TAP = toggle layer, then wait for keypress on called layer, then switch back to original layer.
// SINGLE_HOLD = will work as normal sending the keypress.
// DOUBLE_TAP = 3,
// DOUBLE_HOLD = will then toggle layer and keep sending the Alt keycode until next keypress, then switch back to original layer.
// TRIPLE_TAP = 5,
// TRIPLE_HOLD = 6
// // Alt held down, then use as normal.
//
// Alt tapped, then hold Alt,
int cur_dance (tap_dance_state_t *state);
void alt_finished (tap_dance_state_t *state, void *user_data);
void alt_reset (tap_dance_state_t *state, void *user_data);

int cur_dance (tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->pressed) return SINGLE_HOLD;
    else return SINGLE_TAP;
  }
  else if (state->count == 2) {
    if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  else if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8;
}

static tap alttap_state = {
  .is_press_action = true,
  .state = 0
};

void alt_finished (tap_dance_state_t *state, void *user_data) {
  alttap_state.state = cur_dance(state);
  switch (alttap_state.state) {
    case SINGLE_TAP: set_oneshot_layer(_ALT, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break;
    case SINGLE_HOLD: register_code(KC_LALT); break;
    case DOUBLE_TAP: set_oneshot_layer(_ALT, ONESHOT_START); set_oneshot_layer(_ALT, ONESHOT_PRESSED); break;
    case DOUBLE_HOLD: register_code(KC_LALT); layer_on(_ALT); break;
    //Last case is for fast typing. Assuming your key is `f`:
    //For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
    //In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
  }
}

void alt_reset (tap_dance_state_t *state, void *user_data) {
  switch (alttap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LALT); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: layer_off(_ALT); unregister_code(KC_LALT); break;
  }
  alttap_state.state = 0;
}

// Ctrl tapped, then hold Ctrl,
void ctl_finished (tap_dance_state_t *state, void *user_data);
void ctl_reset (tap_dance_state_t *state, void *user_data);

static tap ctltap_state = {
  .is_press_action = true,
  .state = 0
};

void ctl_finished (tap_dance_state_t *state, void *user_data) {
  ctltap_state.state = cur_dance(state);
  switch (ctltap_state.state) {
    case SINGLE_TAP: set_oneshot_mods(MOD_BIT(KC_LCTL)); break;
    case SINGLE_HOLD: register_code(KC_LCTL); break;
    case DOUBLE_TAP: set_oneshot_layer(_ALT, ONESHOT_START); set_oneshot_layer(_ALT, ONESHOT_PRESSED); break;
    case DOUBLE_HOLD: register_code(KC_LCTL); layer_on(_ALT); break;
    case TRIPLE_TAP: register_mods(MOD_BIT(KC_LCTL)); register_mods(MOD_BIT(KC_LSFT)); break;
  }
}

void ctl_reset (tap_dance_state_t *state, void *user_data) {
  switch (ctltap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LCTL); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: layer_off(_ALT); unregister_code(KC_LCTL); break;
    case TRIPLE_TAP: break;
  }
  ctltap_state.state = 0;
}

// ==========================================================================

// Layer Lower tap dance
// Layer Lower key action:
// Layer Lower held down, move to LOWER layer then move back to previous layer.
// Layer Lower tapped, enable QWERTY layer.
// Layer Lower double-tapped, move to LOWER layer.
// Layer Lower tapped and held down, nothing.
// Layer Lower tripple-tapped, do a soft reset.
void layerLower_finished (tap_dance_state_t *state, void *user_data);
void layerLower_reset (tap_dance_state_t *state, void *user_data);

static tap layerLower_tap_state = {
  .is_press_action = true,
  .state = 0
};

void layerLower_finished (tap_dance_state_t *state, void *user_data) {
  layerLower_tap_state.state = cur_dance(state);
  switch (layerLower_tap_state.state) {
    case SINGLE_TAP:  layer_move(_QWERTY); break;
    case SINGLE_HOLD: layer_on(_LOWER); break;
    case DOUBLE_TAP:  layer_move(_LOWER); break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP:  soft_reset_keyboard(); break;
  }
}

void layerLower_reset (tap_dance_state_t *state, void *user_data) {
  switch (layerLower_tap_state.state) {
    case SINGLE_TAP:  break;
    case SINGLE_HOLD: layer_off(_LOWER); break;
    case DOUBLE_TAP:  break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP: break;
  }
  layerLower_tap_state.state = 0;
}



// Layer Raise tap dance
// Layer Raise key action:
// Layer Raise held down, move to Raise layer then move back to previous layer.
// Layer Raise tapped, enable Raise layer.
// Layer Raise double-tapped, nothing.
// Layer Raise tapped and held down, nothing.
void layerRaise_finished (tap_dance_state_t *state, void *user_data);
void layerRaise_reset (tap_dance_state_t *state, void *user_data);

static tap layerRaise_tap_state = {
  .is_press_action = true,
  .state = 0
};

void layerRaise_finished (tap_dance_state_t *state, void *user_data) {
  layerRaise_tap_state.state = cur_dance(state);
  switch (layerRaise_tap_state.state) {
    case SINGLE_TAP:  register_code(KC_SPACE); break;
    case SINGLE_HOLD: layer_on(_RAISE); break;
    case DOUBLE_TAP:  break;
    case DOUBLE_HOLD: layer_on(_MIRROR); break;
    case TRIPLE_TAP: layer_move(_MIRROR); break;
  }
}

void layerRaise_reset (tap_dance_state_t *state, void *user_data) {
  switch (layerRaise_tap_state.state) {
    case SINGLE_TAP:  unregister_code(KC_SPACE); break;
    case SINGLE_HOLD: layer_off(_RAISE); break;
    case DOUBLE_TAP:  tap_code(KC_ENT); break;
    case DOUBLE_HOLD: layer_off(_MIRROR); break;
    case TRIPLE_TAP: break;
  }
  layerRaise_tap_state.state = 0;
}

// ==========================================================================

// Layer Down tap dance
// Layer Down key action:
// Layer Down held down, move to MOUSE layer then move back to previous layer.
// Layer Down tapped, enable MOUSE layer.
// Layer Down double-tapped, nothing.
// Layer Down tapped and held down, nothing.
void layerDown_finished (tap_dance_state_t *state, void *user_data);
void layerDown_reset (tap_dance_state_t *state, void *user_data);

static tap layerDown_tap_state = {
  .is_press_action = true,
  .state = 0
};

void layerDown_finished (tap_dance_state_t *state, void *user_data) {
  layerDown_tap_state.state = cur_dance(state);
  switch (layerDown_tap_state.state) {
    case SINGLE_TAP:  register_code(KC_SPACE); break;
    case SINGLE_HOLD: layer_on(_MOUSE); break;
    case DOUBLE_TAP:  layer_move(_MOUSE); break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP: break;
  }
}

void layerDown_reset (tap_dance_state_t *state, void *user_data) {
  switch (layerDown_tap_state.state) {
    case SINGLE_TAP:  unregister_code(KC_SPACE); break;
    case SINGLE_HOLD: layer_off(_MOUSE); break;
    case DOUBLE_TAP:  break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP: break;
  }
  layerDown_tap_state.state = 0;
}

// Layer Up tap dance
// Layer Up key action:
// Layer Up held down, move to Adjust layer then move back to previous layer.
// Layer Up tapped, nothing.
// Layer Up double-tapped, enable Adjust layer.
// Layer Up tapped and held down, nothing.
void layerUp_finished (tap_dance_state_t *state, void *user_data);
void layerUp_reset (tap_dance_state_t *state, void *user_data);

static tap layerUp_tap_state = {
  .is_press_action = true,
  .state = 0
};

void layerUp_finished (tap_dance_state_t *state, void *user_data) {
  layerUp_tap_state.state = cur_dance(state);
  switch (layerUp_tap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: layer_on(_ADJUST); break;
    case DOUBLE_TAP: layer_move(_ADJUST); break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP: break;
  }
}

void layerUp_reset (tap_dance_state_t *state, void *user_data) {
  switch (layerUp_tap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: layer_off(_ADJUST); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
    case TRIPLE_TAP: break;
  }
  layerUp_tap_state.state = 0;
}

// ==========================================================================

// Shift key action:
// Shift held down, then use as normal and use Shift Mode of key.
// Shift tapped, then Capitlize next keystroke only.
// Shift double-tapped, then CAPSLOCK
// Shift double-tapped again, CAPS UNLOCKED
void lshift_finished (tap_dance_state_t *state, void *user_data);
void lshift_reset (tap_dance_state_t *state, void *user_data);

static tap lshifttap_state = {
  .is_press_action = true,
  .state = 0
};

void lshift_finished (tap_dance_state_t *state, void *user_data) {
  lshifttap_state.state = cur_dance(state);
  switch (lshifttap_state.state) {
    case SINGLE_TAP: set_oneshot_mods(MOD_BIT(KC_LSFT)); break;
    case SINGLE_HOLD: register_code(KC_LSFT); break;
    case DOUBLE_TAP: tap_code (KC_CAPS); break;
    case DOUBLE_HOLD: break;
  }
}

void lshift_reset (tap_dance_state_t *state, void *user_data) {
  switch (lshifttap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LSFT); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
  lshifttap_state.state = 0;
}

//TD_LSPACE
void lspace_finished (tap_dance_state_t *state, void *user_data);
void lspace_reset (tap_dance_state_t *state, void *user_data);

static tap lspacetap_state = {
  .is_press_action = true,
  .state = 0
};

void lspace_finished (tap_dance_state_t *state, void *user_data) {
  lspacetap_state.state = cur_dance(state);
  switch (lspacetap_state.state) {
    case SINGLE_TAP: tap_code (KC_SPACE); break;
    case SINGLE_HOLD: layer_on(_LOWER); break;
    case DOUBLE_TAP: tap_code (KC_ENT);break;
    case DOUBLE_HOLD: break;
  }
}

void lspace_reset (tap_dance_state_t *state, void *user_data) {
  switch (lspacetap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: layer_off(_LOWER); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
  lspacetap_state.state = 0;
}



// Q key for Custom Mod
// Q key action:
// Q tapped, then Q keystroke only.
// Q held down, then use Backspace key.
// Q double-tapped, then nothing.
// Q double-tapped and held, nothing.
void quekey_finished (tap_dance_state_t *state, void *user_data);
void quekey_reset (tap_dance_state_t *state, void *user_data);

static tap quekeytap_state = {
  .is_press_action = true,
  .state = 0
};

void quekey_finished (tap_dance_state_t *state, void *user_data) {
  quekeytap_state.state = cur_dance(state);
  switch (quekeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_Q); break;
    case SINGLE_HOLD: register_code(KC_BSPC); break;
    case DOUBLE_TAP: tap_code(KC_BSPC); break;
    case DOUBLE_HOLD: break;
  }
}

void quekey_reset (tap_dance_state_t *state, void *user_data) {
  switch (quekeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_BSPC); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
  quekeytap_state.state = 0;
}



// W key for Custom Mod
// W key action:
// W tapped, then W keystroke only.
// W held down, then use ESC key.
// W double-tapped, then nothing.
// W double-tapped and held, nothing.
void esckey_finished (tap_dance_state_t *state, void *user_data);
void esckey_reset (tap_dance_state_t *state, void *user_data);

static tap esckeytap_state = {
  .is_press_action = true,
  .state = 0
};

void esckey_finished (tap_dance_state_t *state, void *user_data) {
  esckeytap_state.state = cur_dance(state);
  switch (esckeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_W); break;
    case SINGLE_HOLD: register_code(KC_ESC); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void esckey_reset (tap_dance_state_t *state, void *user_data) {
  switch (esckeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_ESC); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
  esckeytap_state.state = 0;
}




// S key for Custom Home Row Mod
// S key action:
// S tapped, then S keystroke only.
// S held down, then use L_GUI key.
// S double-tapped, then L_CTL + S.
// S double-tapped and held, nothing.
void eskey_finished (tap_dance_state_t *state, void *user_data);
void eskey_reset (tap_dance_state_t *state, void *user_data);

static tap eskeytap_state = {
  .is_press_action = true,
  .state = 0
};

void eskey_finished (tap_dance_state_t *state, void *user_data) {
  eskeytap_state.state = cur_dance(state);
  switch (eskeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_S); break;
    case SINGLE_HOLD: register_code(KC_LGUI); break;
    case DOUBLE_TAP: register_code(KC_LCTL);register_code(KC_S); break;
    case DOUBLE_HOLD: break;
  }
}

void eskey_reset (tap_dance_state_t *state, void *user_data) {
  switch (eskeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LGUI); break;
    case DOUBLE_TAP: unregister_code(KC_S);unregister_code(KC_LCTL); break;
    case DOUBLE_HOLD: break;
  }
  eskeytap_state.state = 0;
}




// G key for Custom Home Row Mod
// G key action:
// G tapped, then G keystroke only.
// G held down, then use ENTER key.
// G double-tapped, then nothing.
// G double-tapped and held, nothing.
void geekey_finished (tap_dance_state_t *state, void *user_data);
void geekey_reset (tap_dance_state_t *state, void *user_data);

static tap geekeytap_state = {
  .is_press_action = true,
  .state = 0
};

void geekey_finished (tap_dance_state_t *state, void *user_data) {
  geekeytap_state.state = cur_dance(state);
  switch (geekeytap_state.state) {
    case SINGLE_TAP: register_code(KC_G); break;
    case SINGLE_HOLD: register_code(KC_ENT); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void geekey_reset (tap_dance_state_t *state, void *user_data) {
  switch (geekeytap_state.state) {
    case SINGLE_TAP: unregister_code(KC_S); break;
    case SINGLE_HOLD: unregister_code(KC_ENT); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
  geekeytap_state.state = 0;
}




// Semi-Colon key for Custom Mod
// Semi-Colon key action:
// Semi-Colon tapped, then ; keystroke only.
// Semi-Colon held down, then use GUI mode of key.
// Semi-Colon double-tapped, then : keystroke.
// Semi-Colon double-tapped and held, nothing.
void semicolon_finished (tap_dance_state_t *state, void *user_data);
void semicolon_reset (tap_dance_state_t *state, void *user_data);

static tap semicolontap_state = {
  .is_press_action = true,
  .state = 0
};

void semicolon_finished (tap_dance_state_t *state, void *user_data) {
  semicolontap_state.state = cur_dance(state);
  switch (semicolontap_state.state) {
    case SINGLE_TAP: tap_code(KC_SCLN); break;
    case SINGLE_HOLD: register_code(KC_RGUI); break;
    case DOUBLE_TAP: register_code(KC_RSFT);register_code(KC_SCLN); break;
    case DOUBLE_HOLD: break;
  }
}

void semicolon_reset (tap_dance_state_t *state, void *user_data) {
  switch (semicolontap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_RGUI); break;
    case DOUBLE_TAP: unregister_code(KC_SCLN);unregister_code(KC_RSFT); break;
    case DOUBLE_HOLD: break;
  }
  semicolontap_state.state = 0;
}





// Forward-Slash key for Custom Mod
// Forward-Slash key action:
// Forward-Slash tapped, then / keystroke only.
// Forward-Slash held down, nothing.
// Forward-Slash double-tapped, then ? keystroke.
// Forward-Slash triple-tapped , CTRL+/.
void fwdslash_finished (tap_dance_state_t *state, void *user_data);
void fwdslash_reset (tap_dance_state_t *state, void *user_data);

static tap fwdslashtap_state = {
  .is_press_action = true,
  .state = 0
};

void fwdslash_finished (tap_dance_state_t *state, void *user_data) {
  fwdslashtap_state.state = cur_dance(state);
  switch (fwdslashtap_state.state) {
    case SINGLE_TAP:  break;
    case SINGLE_HOLD: break;
    case DOUBLE_TAP: register_code(KC_RSFT);register_code(KC_SLSH); break;
    case TRIPLE_TAP: register_code(KC_LCTL);register_code(KC_SLSH); break;
  }
}

void fwdslash_reset (tap_dance_state_t *state, void *user_data) {
  switch (fwdslashtap_state.state) {
    case SINGLE_TAP:  tap_code(KC_SLSH); break;
    case SINGLE_HOLD: break;
    case DOUBLE_TAP: unregister_code(KC_SLSH);unregister_code(KC_RSFT); break;
    case TRIPLE_TAP: unregister_code(KC_SLSH);unregister_code(KC_LCTL); break;
  }
  fwdslashtap_state.state = 0;
}





// Z key for Custom Mod
// Z key action:
// Z tapped, then Z keystroke only.
// Z held down, then send UNDO key.
// Z double-tapped, then nothing.
// Z double-tapped and held, nothing.
void  zeekey_finished (tap_dance_state_t *state, void *user_data);
void  zeekey_reset (tap_dance_state_t *state, void *user_data);

static tap  zeekeytap_state = {
  .is_press_action = true,
  .state = 0
};

void  zeekey_finished (tap_dance_state_t *state, void *user_data) {
   zeekeytap_state.state = cur_dance(state);
  switch ( zeekeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_Z); break;
    case SINGLE_HOLD: register_code(KC_LCTL);register_code(KC_Z); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void  zeekey_reset (tap_dance_state_t *state, void *user_data) {
  switch ( zeekeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_Z);unregister_code(KC_LCTL); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
   zeekeytap_state.state = 0;
}



// X key for Custom Mod
// X key action:
// X tapped, then X keystroke only.
// X held down, then send CUT key.
// X double-tapped, then nothing.
// X double-tapped and held, nothing.
void  exkey_finished (tap_dance_state_t *state, void *user_data);
void  exkey_reset (tap_dance_state_t *state, void *user_data);

static tap  exkeytap_state = {
  .is_press_action = true,
  .state = 0
};

void  exkey_finished (tap_dance_state_t *state, void *user_data) {
   exkeytap_state.state = cur_dance(state);
  switch ( exkeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_X); break;
    case SINGLE_HOLD: register_code(KC_LCTL);register_code(KC_X); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void  exkey_reset (tap_dance_state_t *state, void *user_data) {
  switch ( exkeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_X);unregister_code(KC_LCTL); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
   exkeytap_state.state = 0;
}



// C key for Custom Mod
// C key action:
// C tapped, then C keystroke only.
// C held down, then send COPY key.
// C double-tapped, then nothing.
// C double-tapped and held, nothing.
void  seekey_finished (tap_dance_state_t *state, void *user_data);
void  seekey_reset (tap_dance_state_t *state, void *user_data);

static tap  seekeytap_state = {
  .is_press_action = true,
  .state = 0
};

void  seekey_finished (tap_dance_state_t *state, void *user_data) {
   seekeytap_state.state = cur_dance(state);
  switch ( seekeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_C); break;
    case SINGLE_HOLD: register_code(KC_LCTL);register_code(KC_C); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void  seekey_reset (tap_dance_state_t *state, void *user_data) {
  switch ( seekeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_C);unregister_code(KC_LCTL); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
   seekeytap_state.state = 0;
}



// V key for Custom Mod
// V key action:
// V tapped, then V keystroke only.
// V held down, then send PASTE key.
// V double-tapped, then nothing.
// V double-tapped and held, nothing.
void  veekey_finished (tap_dance_state_t *state, void *user_data);
void  veekey_reset (tap_dance_state_t *state, void *user_data);

static tap  veekeytap_state = {
  .is_press_action = true,
  .state = 0
};

void  veekey_finished (tap_dance_state_t *state, void *user_data) {
   veekeytap_state.state = cur_dance(state);
  switch ( veekeytap_state.state) {
    case SINGLE_TAP: tap_code(KC_V); break;
    case SINGLE_HOLD: register_code(KC_LCTL);register_code(KC_V); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
}

void  veekey_reset (tap_dance_state_t *state, void *user_data) {
  switch ( veekeytap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_V);unregister_code(KC_LCTL); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: break;
  }
   veekeytap_state.state = 0;
}









//Tap Dance Definitions
tap_dance_action_t tap_dance_actions[] = {
  [TD_DEL_BSPC]   = ACTION_TAP_DANCE_DOUBLE(KC_BSPC, KC_DEL),
  [TD_ESC_GRAVE]  = ACTION_TAP_DANCE_DOUBLE(KC_ESC, KC_GRAVE),
  [TD_TAB_TILDE]  = ACTION_TAP_DANCE_DOUBLE(KC_TAB, KC_TILDE),

  [TD_9_LPRN]     = ACTION_TAP_DANCE_DOUBLE(KC_9, KC_LPRN),
  [TD_0_RPRN]     = ACTION_TAP_DANCE_DOUBLE(KC_0, KC_RPRN),

  [TD_MINS_UNDS]  = ACTION_TAP_DANCE_DOUBLE(KC_MINS, KC_UNDS),
  [TD_EQL_PLUS]   = ACTION_TAP_DANCE_DOUBLE(KC_EQL, KC_PLUS),

  [TD_LBRC_LCBR]  = ACTION_TAP_DANCE_DOUBLE(KC_LBRC, KC_LCBR),
  [TD_RBRC_RCBR]  = ACTION_TAP_DANCE_DOUBLE(KC_RBRC, KC_RCBR),

  [TD_SCLN_COLN]  = ACTION_TAP_DANCE_DOUBLE(KC_SCLN, KC_COLN),
  [TD_QUOT_DQT]   = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, KC_DQT),

  [TD_COMM_LABK]  = ACTION_TAP_DANCE_DOUBLE(KC_COMM, KC_LABK),
  [TD_DOT_RABK]   = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_RABK),
  // [TD_SLSH_QUES]  = ACTION_TAP_DANCE_DOUBLE(KC_SLSH, KC_QUES),
  [TD_SLSH_QUES]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL,fwdslash_finished, fwdslash_reset),
  [TD_BSLS_PIPE]  = ACTION_TAP_DANCE_DOUBLE(KC_BSLS, KC_PIPE),
  [TD_SMCL_CLN]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL,semicolon_finished, semicolon_reset),

  [TD_PGUP_HOME]  = ACTION_TAP_DANCE_DOUBLE(KC_PGUP, KC_HOME),
  [TD_PGDN_END]   = ACTION_TAP_DANCE_DOUBLE(KC_PGDN, KC_END),

  [TD_Q_LrALT]    = ACTION_TAP_DANCE_LAYER_MOVE(KC_Q, _ALT),
  [TD_R_LrKey]    = ACTION_TAP_DANCE_LAYER_MOVE(KC_R, _RAISE),
  [TD_T_LrMS]     = ACTION_TAP_DANCE_LAYER_MOVE(KC_T, _MOUSE),

  [TD_SHIFT_CAPS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL,lshift_finished, lshift_reset),
  [TD_SPC_ENT]    = ACTION_TAP_DANCE_DOUBLE(KC_SPACE, KC_ENT),
  [TD_SPC_BKSPC]  = ACTION_TAP_DANCE_DOUBLE(KC_SPACE, KC_BSPC),
  [TD_LSPACE]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,lspace_finished,lspace_reset),

  [ALT_OSL1]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,alt_finished, alt_reset),
  [CTL_OSL1]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,ctl_finished, ctl_reset),

  [TD_LayerDn]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL,layerDown_finished, layerDown_reset),
  [TD_LayerUp]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL,layerUp_finished, layerUp_reset),

  [TD_LayerLower] = ACTION_TAP_DANCE_FN_ADVANCED(NULL,layerLower_finished, layerLower_reset),
  [TD_LayerRaise] = ACTION_TAP_DANCE_FN_ADVANCED(NULL,layerRaise_finished, layerRaise_reset),

  [TD_Q_BSPC]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,quekey_finished, quekey_reset),
  [TD_W_ESC]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,esckey_finished, esckey_reset),
  [TD_S]          = ACTION_TAP_DANCE_FN_ADVANCED(NULL,eskey_finished, eskey_reset),
  [TD_G_ENT]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,geekey_finished, geekey_reset),
  [TD_Z_UNDO]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,zeekey_finished, zeekey_reset),
  [TD_X_CUT]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,exkey_finished, exkey_reset),
  [TD_C_COPY]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,seekey_finished, seekey_reset),
  [TD_V_PASTE]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL,veekey_finished, veekey_reset),
};


// END OF TAP DANCE ***********************************************************

// COMBOS ***********************************************************
// Combos Declarations
enum combos {
  FG_ENT,
  THUMB_ENT,
  NUM45
};

const uint16_t PROGMEM fg_combo[]          = {KC_F, KC_G, COMBO_END};
// const uint16_t PROGMEM thumb_enter_combo[] = {TD_LayerLower, TD_LayerRaise, COMBO_END};
const uint16_t PROGMEM num45_combo[]       = {KC_4, KC_5, COMBO_END};

combo_t key_combos[] = {
  [FG_ENT] = COMBO_ACTION(fg_combo),
  //[THUMB_ENT] = COMBO_ACTION(thumb_enter_combo),
  [NUM45] = COMBO_ACTION(num45_combo),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
  switch(combo_index) {
    case NUM45:
      if (pressed) {
        SEND_STRING("5881320");
        tap_code16(KC_ENT);
      }
      break;
    case FG_ENT:
      if (pressed) {
        tap_code16(KC_ENT);
      }
      break;
    // case THUMB_ENT:
    //   if (pressed) {
    //     tap_code16(KC_ENT);
    //   }
    //   break;
  }
}

// END OF COMBOS ***********************************************************



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | Bspc |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |  0   |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |  P   |  +=  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LShift|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |  ;:  |  '"  |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * | Ctrl |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |  ,<  |  .>  |  /?  |  \|  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |   ˄  |   ˄  | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_QWERTY] = LAYOUT(
    KC_BSPC,          KC_1,               KC_2,        KC_3,        KC_4,        KC_5,                              KC_6,        KC_7,             KC_8,          TD(TD_9_LPRN),   TD(TD_0_RPRN),   TD(TD_MINS_UNDS),
    TD(TD_TAB_TILDE),TD(TD_Q_BSPC),TD(TD_W_ESC),       KC_E,        KC_R,        KC_T,                              KC_Y,        KC_U,             KC_I,             KC_O,            KC_P,         TD(TD_EQL_PLUS),
    TD(TD_SHIFT_CAPS),LGUI_T(KC_A),TD(TD_S),    LSFT_T(KC_D),LCTL_T(KC_F),TD(TD_G_ENT),                             KC_H, RCTL_T(KC_J),     RSFT_T(KC_K),     RALT_T(KC_L),        TD(TD_SMCL_CLN), TD(TD_QUOT_DQT),
    TD(CTL_OSL1),    TD(TD_Z_UNDO),TD(TD_X_CUT),TD(TD_C_COPY),TD(TD_V_PASTE),    KC_B,                              KC_N,        KC_M,          TD(TD_COMM_LABK), TD(TD_DOT_RABK), TD(TD_SLSH_QUES),TD(TD_BSLS_PIPE),
                               OSM(MOD_LGUI),TD(ALT_OSL1),TD(TD_LayerLower),TD(TD_LayerRaise), KC_SPC,     KC_ENT,TD(TD_LayerDn),TD(TD_LayerUp),TD(TD_LayerRaise),   KC_ESC

),

/*
 * MIRROR
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |  -_  |  0   |   9  |   8  |   7  |   6  |                    |   6  |   7  |   8  |   9  |  0   |  -_  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  +=  |  P   |   O  |   I  |   U  |   Y  |                    |   Y  |   U  |   I  |   O  |  P   |  +=  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  '"  |  ;:  |   L  |   K  |   J  |   H  |-------.    ,-------|   H  |   J  |   K  |   L  |  ;:  |  '"  |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |  \|  |  /?  |  .>  |  ,<  |   M  |   N  |-------|    |-------|   N  |   M  |  ,<  |  .>  |  /?  |  \|  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |   ˄  |   ˄  | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_MIRROR] = LAYOUT(
    TD(TD_MINS_UNDS),      KC_0,            KC_9,        KC_8,        KC_7,        KC_6,                                KC_6,        KC_7,             KC_8,          TD(TD_9_LPRN),   TD(TD_0_RPRN),   TD(TD_MINS_UNDS),
    TD(TD_EQL_PLUS),       KC_P,            KC_O,        KC_I,        KC_U,        KC_Y,                                KC_Y,        KC_U,             KC_I,             KC_O,            KC_P,         TD(TD_EQL_PLUS),
    TD(TD_QUOT_DQT), TD(TD_SMCL_CLN),RALT_T(KC_L),RSFT_T(KC_K),RCTL_T(KC_J),       KC_H,                                KC_H, RCTL_T(KC_J),     RSFT_T(KC_K),     RALT_T(KC_L),        TD(TD_SMCL_CLN), TD(TD_QUOT_DQT),
    TD(TD_BSLS_PIPE),TD(TD_SLSH_QUES),TD(TD_DOT_RABK),TD(TD_COMM_LABK),KC_M,       KC_N,                                KC_N,        KC_M,          TD(TD_COMM_LABK), TD(TD_DOT_RABK), TD(TD_SLSH_QUES),TD(TD_BSLS_PIPE),

        OSM(MOD_LGUI),TD(ALT_OSL1),TD(TD_LayerLower),TD(TD_LayerRaise), KC_SPC,     KC_ENT,TD(TD_LayerDn),TD(TD_LayerUp),    TD(TD_LayerRaise),   KC_ESC
),

/*
 * ALT Layout
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | Ins  |  F1  |  F2  |  F3  |  F4  |  F5  |                    |  F6  |  F7  |  F8  |  F9  |  F0  | F11  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | PrtSc|      |      |  UP  |      |  (   |                    |   )  |   7  |   8  |   9  |   /  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | LEFT | DOWN | RIGHT|  [   |-------.    ,-------|   ]  |   4  |   5  |   6  |   *  |   -  |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |      |      |      |      |      |  {   |-------|    |-------|   }  |   1  |   2  |   3  |   =  |   +  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \   0 |   0  |   .  |   ,  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_ALT] = LAYOUT(
    KC_INS,  KC_F1,   KC_F2,  KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_F7, KC_F8,  KC_F9,  KC_F10,  KC_F11,
    KC_PSCR, KC_NO,   KC_NO,  KC_UP,   KC_NO,   KC_LPRN,                       KC_RPRN, KC_7,  KC_8,   KC_9,   KC_PSLS, KC_F12,
    KC_NO,   KC_NO,   KC_LEFT,KC_DOWN, KC_RIGHT,KC_LBRC,                       KC_RBRC, KC_4,  KC_5,   KC_6,   KC_PAST, KC_PMNS,
    KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_LCBR,                       KC_RCBR, KC_1,  KC_2,   KC_3,   KC_PEQL, KC_PPLS,
                      KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_PENT, KC_0,    KC_0,  KC_DOT, KC_COMMA
),


/*
 * LOWER Layout
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | Del  | Ins  |      |      |      |      |                    |      |   =  |   /  |   *  |   -  | Bspc |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | PrtSc|      |      |  UP  |      |  (   |                    |   )  |   7  |   8  |   9  |   +  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | LEFT | DOWN | RIGHT|  [   |-------.    ,-------|   ]  |   4  |   5  |   6  |   +  |      |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |      |      |      |      |      |  {   |-------|    |-------|   }  |   1  |   2  |   3  |Enter |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \   0 |   0  |   .  |   ,  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_LOWER] = LAYOUT(
    KC_DEL,  KC_INS,  KC_NO,  KC_NO,   KC_NO,   KC_NO,                         KC_NO,   KC_EQL,KC_PSLS,KC_PAST,KC_PMNS, KC_BSPC,
    KC_PSCR, KC_NO,   KC_NO,  KC_UP,   KC_NO,   KC_LPRN,                       KC_RPRN, KC_7,  KC_8,   KC_9,   KC_PPLS, KC_NO,
    KC_NO,   KC_NO,   KC_LEFT,KC_DOWN, KC_RIGHT,KC_LBRC,                       KC_RBRC, KC_4,  KC_5,   KC_6,   KC_PPLS, KC_NO,
    KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_LCBR,                       KC_RCBR, KC_1,  KC_2,   KC_3,   KC_PENT, KC_NO,
                      KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_PENT, KC_0,    KC_0,  KC_DOT, KC_COMMA
),


/*
 * RAISE Layout
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |  !   |  @   |  #   |  $   |  %   |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  ~   |  ^   |  &   |  *   |  (   |  )   |                    | PGUP |      | WHUP |      |      |SPEED0|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  `   |  _   |  +   |  |   |  [   |  ]   |-------.    ,-------| PGDN | BTN1 | BTN2 | BTN2 |      |SPEED1|
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |   /  |   \  |  <   |  >   |  {   |  }   |-------|    |-------|      |WHLeft| WHDN |WHRite|      |SPEED2|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |   ˄  |   ˄  | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_RAISE] = LAYOUT(
    KC_NO,   KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                       KC_NO,            KC_NO,         KC_NO,         KC_NO,          KC_NO, KC_NO,
    KC_TILD, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,                       TD(TD_PGUP_HOME), KC_NO,         KC_MS_WH_UP,   KC_NO,          KC_NO, KC_MS_ACCEL0,
    KC_GRV,  KC_UNDS, KC_PLUS, KC_PIPE, KC_LBRC, KC_RBRC,                       TD(TD_PGDN_END),  KC_MS_BTN1,    KC_MS_BTN2,    KC_MS_BTN3,     KC_NO, KC_MS_ACCEL1,
    KC_SLSH, KC_BSLS, KC_LT,   KC_GT,   KC_LCBR, KC_RCBR,                       KC_NO,            KC_MS_WH_LEFT, KC_MS_WH_DOWN, KC_MS_WH_RIGHT, KC_NO, KC_MS_ACCEL2,
                      KC_TRNS,KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS,          KC_TRNS,       KC_TRNS,       KC_TRNS
),


/*
 * Mouse Layout
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |WHLeft|      |WHRite| WHUP |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |SPEED0|      | BTN3 | BTN2 | BTN1 | WHDN |                    |SPEED0|      | WHUP |      | PGUP | HOME |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |SPEED1|      |      |      |      |      |-------.    ,-------|SPEED1| BTN1 | BTN2 | BTN3 | PGDN | END  |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |SPEED2|      |      |      |      |      |-------|    |-------|SPEED2|WHLeft| WHDN |WHRite|      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |  ˄   |  ˄   | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_MOUSE] = LAYOUT(
    KC_NO,   KC_NO, KC_MS_WH_LEFT,KC_NO,      KC_MS_WH_RIGHT,KC_MS_WH_UP,                    KC_NO,   KC_NO,         KC_NO,         KC_NO,          KC_NO,            KC_NO,
    KC_ACL0, KC_NO, KC_MS_BTN3,   KC_MS_BTN2, KC_MS_BTN1,    KC_MS_WH_DOWN,                  KC_ACL0, KC_NO,         KC_MS_WH_UP,   KC_NO,          TD(TD_PGUP_HOME), KC_HOME,
    KC_ACL1, KC_NO, KC_NO,        KC_NO,      KC_NO,         KC_NO,                          KC_ACL1, KC_MS_BTN1,    KC_MS_BTN2,    KC_MS_BTN3,     TD(TD_PGDN_END),  KC_END,
    KC_ACL2, KC_NO, KC_NO,        KC_NO,      KC_NO,         KC_NO,                          KC_ACL2, KC_MS_WH_LEFT, KC_MS_WH_DOWN, KC_MS_WH_RIGHT, KC_NO,            KC_NO,
                    KC_TRNS,      KC_TRNS,    KC_TRNS,       KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS,       KC_TRNS,       KC_TRNS
),


/*
 * RGB and Media
 * ,-----------------------------------------.                    ,------------------------------------------.
 * |      |      |      |      |      |      |                    |LBri+ |      |      |TMute |TAnswr| TEnd  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+-------|
 * |Reboot|      |      |      |      |      |                    |LBri- |      |      |      |      |       |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+-------|
 * |      |RGBTog| Hue+ | Sat+ |Brite+|Efct+ |-------.    ,-------|      | Vol- | Mute | Vol+ |      |CMB On |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+-------|
 * |      |RGBMd+| Hue- | Sat- |Brite-|Efct- |-------|    |-------|      | Prev | Play | Next |      |CMB Off|
 * `-----------------------------------------/       /     \      \------------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |  ˄   |  ˄   | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_ADJUST] = LAYOUT(
    KC_NO,  KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_NO,                          KC_BRIU, KC_NO,  KC_NO,   MSTEAMS_MUTEMIC,   MSTEAMS_ANSWER_CALL,  MSTEAMS_END_CALL,
    QK_RBT, KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_NO,                          KC_BRID, KC_NO,  KC_NO,   KC_NO,   KC_NO,  KC_NO,
    KC_NO,  RGB_TOG, RGB_HUI,RGB_SAI, RGB_VAI, RGB_SPI,                        KC_NO, KC_VOLD,KC_MUTE, KC_VOLU, KC_NO,  QK_COMBO_ON,
    KC_NO,  RGB_MOD, RGB_HUD,RGB_SAD, RGB_VAD, RGB_SPD,                        KC_NO, KC_MPRV,KC_MPLY, KC_MNXT, KC_NO,  QK_COMBO_OFF,
                     KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS
),

/*
 * Corne_QWERTY Layout
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |  P   |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |  ;:  |      |
 * |------+------+------+------+------+------| JoySt |    | JoySt |------+------+------+------+------+------|
 * |      |   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |  ,<  |  .>  |  /?  |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *               | LGUI | LALT |Lower |Raise/ Space /       \ENTER \  ˅  |   ˄  |   ˄  | ESC  |
 *               |      |      |      |    /       /         \      \    |      |      |      |
 *               `------------------------'-------'           '------'------------------------'
 */

  [_CORNE_QWERTY] = LAYOUT(
    KC_BSPC,          KC_1,               KC_2,        KC_3,        KC_4,        KC_5,                              KC_6,        KC_7,             KC_8,          TD(TD_9_LPRN),   TD(TD_0_RPRN),   TD(TD_MINS_UNDS),
    TD(TD_TAB_TILDE), KC_Q,        TD(TD_W_ESC),       KC_E,        KC_R,        KC_T,                              KC_Y,        KC_U,             KC_I,             KC_O,            KC_P,         TD(TD_EQL_PLUS),
    TD(TD_SHIFT_CAPS),LGUI_T(KC_A),LALT_T(KC_S),LSFT_T(KC_D),LCTL_T(KC_F),TD(TD_G_ENT),                             KC_H, RCTL_T(KC_J),     RSFT_T(KC_K),     RALT_T(KC_L),        TD(TD_SMCL_CLN), TD(TD_QUOT_DQT),
    TD(CTL_OSL1),     KC_Z,               KC_X,        KC_C,        KC_V,        KC_B,                              KC_N,        KC_M,          TD(TD_COMM_LABK), TD(TD_DOT_RABK), TD(TD_SLSH_QUES),TD(TD_BSLS_PIPE),
                               OSM(MOD_LGUI),TD(ALT_OSL1), TD(TD_LayerDn), KC_SPC, KC_ENT,     KC_ENT,TD(TD_LayerUp),TD(TD_LayerLower),TD(TD_LayerRaise), KC_TRNS

)


};


bool set_scrolling = false;
bool enable_mouse_pointer = false;
uint8_t current_layer = 0;

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  #ifdef DEBUG_PRINT_STATEMENTS
    debug_enable=true;
  #endif
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  #ifdef CONSOLE_ENABLE
    // uprintf("%s keycode\n", keycode);
    dprintf("process --> KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    //dprintf("process --> KL: kc: 0x%04X, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
  #endif

  // uint8_t current_layer = get_highest_layer(layer_state);

  // switch (current_layer) {
  //   case _QWERTY:
  //       enable_mouse_pointer = false;
  //       break;
  //   default: //  for any other layers, or the default layer
  //       enable_mouse_pointer = true;
  //       break;
  //   }

  switch (keycode) {
    case MSTEAMS_MUTEMIC:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTL(SS_LSFT("m")));
        return false;
      }
      break;
    case MSTEAMS_ANSWER_CALL:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTL(SS_LSFT("a")));
        return false;
      }
      break;
    case MSTEAMS_END_CALL:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTL(SS_LSFT("h")));
        return false;
      }
      break;


//     case KC_TRNS:
//     case KC_NO:
//       /* Always cancel one-shot layer when another key gets pressed */
//       if (record->event.pressed && is_oneshot_layer_active())
//       clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
//       return true;
//     case QK_BOOT:
//       /* Don't allow reset from oneshot layer state */
//       if (record->event.pressed && is_oneshot_layer_active()){
//         clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
//         return false;
//       }
//       return true;
//     case QWERTY:
//       if (record->event.pressed) {
//         set_single_persistent_default_layer(_QWERTY);
//       }
//       return false;
//       break;
//     case LOWER:
//       if (record->event.pressed) {
//         layer_on(_LOWER);
//         update_tri_layer(_LOWER, _RAISE, _ADJUST);
//       } else {
//         layer_off(_LOWER);
//         update_tri_layer(_LOWER, _RAISE, _ADJUST);
//       }
//       return false;
//       break;
//     case RAISE:
//       if (record->event.pressed) {
//         layer_on(_RAISE);
//         update_tri_layer(_LOWER, _RAISE, _ADJUST);
//       } else {
//         layer_off(_RAISE);
//         update_tri_layer(_LOWER, _RAISE, _ADJUST);
//       }
//       return false;
//       break;
//     case MOUSE:
//       if (record->event.pressed) {
//         set_single_persistent_default_layer(_MOUSE);
//       }
//       return false;
//       break;
  }
  return true;
};


layer_state_t layer_state_set_user(layer_state_t state) {
    // This function is called whenever the layer state changes
    // You can perform actions here based on the layer state

    // Example: Print the current active layer to the debug console
    switch (get_highest_layer(state)) {
        case _QWERTY:
            // Code for when the base layer is active
            current_layer = _QWERTY;
            enable_mouse_pointer = false;
            break;
        case _MIRROR:
            // Code for when the base layer is active
            current_layer = _MIRROR;
            enable_mouse_pointer = false;
            break;
        case _ALT:
            // Code for when the symbol layer is active
            current_layer = _ALT;
            enable_mouse_pointer = false;
            break;
        case _CTRL:
            // Code for when the navigation layer is active
            current_layer = _CTRL;
            enable_mouse_pointer = false;
            break;
        case _LOWER:
            // Code for when the navigation layer is active
            current_layer = _LOWER;
            enable_mouse_pointer = false;
            break;
        case _RAISE:
            // Code for when the navigation layer is active
            current_layer = _RAISE;
            enable_mouse_pointer = false;
            break;
        case _MOUSE:
            // Code for when the navigation layer is active
            current_layer = _MOUSE;
            enable_mouse_pointer = true;
            break;
        case _ADJUST:
            // Code for when the navigation layer is active
            current_layer = _ADJUST;
            enable_mouse_pointer = false;
            break;
        default: //  for any other layers, or the default layer
            current_layer = 0;
            enable_mouse_pointer = false;
            break;
    }

    return state;
}


static bool left_pointer_tap_code_sent = false;
#if defined(SPLIT_POINTING_ENABLE)
#  if defined(POINTING_DEVICE_COMBINED)
     static bool right_pointer_tap_code_sent = false;
#  endif
#define LEFT_POINTER_X_ARRAY_SIZE 12
#define RIGHT_POINTER_X_ARRAY_SIZE 18
int arrLeftX[LEFT_POINTER_X_ARRAY_SIZE]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int arrLeftY[LEFT_POINTER_X_ARRAY_SIZE]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int arrRightX[RIGHT_POINTER_X_ARRAY_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int arrRightY[RIGHT_POINTER_X_ARRAY_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void updateLeftX(int arrLeftX[], int newValue) {
    // Shift elements to the left
    for (int i = 0; i < LEFT_POINTER_X_ARRAY_SIZE - 1; i++) {
        arrLeftX[i] = arrLeftX[i + 1];
    }
    // Add the newest integer
    arrLeftX[LEFT_POINTER_X_ARRAY_SIZE - 1] = newValue;
}
void updateLeftY(int arrLeftY[], int newValue) {
    // Shift elements to the left
    for (int i = 0; i < LEFT_POINTER_X_ARRAY_SIZE - 1; i++) {
        arrLeftY[i] = arrLeftY[i + 1];
    }
    // Add the newest integer
    arrLeftY[LEFT_POINTER_X_ARRAY_SIZE - 1] = newValue;
}

void updateRightX(int arrRightX[], int newValue) {
    // Shift elements to the left
    for (int i = 0; i < RIGHT_POINTER_X_ARRAY_SIZE - 1; i++) {
        arrRightX[i] = arrRightX[i + 1];
    }
    // Add the newest integer
    arrRightX[RIGHT_POINTER_X_ARRAY_SIZE - 1] = newValue;
}
void updateRightY(int arrRightY[], int newValue) {
    // Shift elements to the left
    for (int i = 0; i < RIGHT_POINTER_X_ARRAY_SIZE - 1; i++) {
        arrRightY[i] = arrRightY[i + 1];
    }
    // Add the newest integer
    arrRightY[RIGHT_POINTER_X_ARRAY_SIZE - 1] = newValue;
}

bool checkAllZeros(int arr[]) {
    for (int i = 0; i < LEFT_POINTER_X_ARRAY_SIZE; i++) {
        if (arr[i] != 0) {
            return false; // Not all zeros
        }
    }
    return true; // All zeros
}
#endif

#if defined(SPLIT_POINTING_ENABLE)
report_mouse_t pointing_device_task_combined_kb(report_mouse_t left_report, report_mouse_t right_report) {
  dprintf("=========================================================================================\n");
  dprintf("Left Pointing Device ==> left_report.x: %d, left_report.y: %d\n", left_report.x, left_report.y);
  dprintf("Right Pointing Device ==> right_report.x: %d, right_report.y: %d\n", right_report.x, right_report.y);

  updateLeftX(arrLeftX, left_report.x);
  updateLeftY(arrLeftY, left_report.y);
  updateRightX(arrRightX, right_report.x);
  updateRightY(arrRightY, right_report.y);
  if(checkAllZeros(arrLeftX) && checkAllZeros(arrLeftY)) {
    //   dprintf("All zeros\n");
      left_pointer_tap_code_sent = false;
    //   dprintf("Set left_pointer_tap_code_sent to FALSE\n");
//   } else {
    //   dprintf("Not all zeros\n");
  }
  if(checkAllZeros(arrRightX) && checkAllZeros(arrRightY)) {
    //   dprintf("All zeros\n");
      right_pointer_tap_code_sent = false;
    //   dprintf("Set right_pointer_tap_code_sent to FALSE\n");
//   } else {
    //   dprintf("Not all zeros\n");
  }

  if (!enable_mouse_pointer) {
    dprintf("Left Mouse already_sent: %d\n", left_pointer_tap_code_sent);
    if (!left_pointer_tap_code_sent) {
      dprintf("About to send keys...\n");
      dprintf("left_report.x: %d,left_report.y: %d\n", left_report.x, left_report.y);
      if (left_report.x == 7) {
          tap_code(KC_END);
        //   dprintf("Sent KC_END\n");
          left_pointer_tap_code_sent = true;
        //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
      } else if (left_report.x == -7) {
          tap_code(KC_HOME);
        //   dprintf("Sent KC_HOME\n");
          left_pointer_tap_code_sent = true;
        //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
      }

      if (left_report.y == 7) {
          tap_code(KC_PGDN);
        //   dprintf("Sent KC_PGDN\n");
          left_pointer_tap_code_sent = true;
        //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
      } else if (left_report.y == -7) {
          tap_code(KC_PGUP);
        //   dprintf("Sent KC_PGUP\n");
          left_pointer_tap_code_sent = true;
        //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
      }
    }
    // =================================================================================
    dprintf("Right Mouse already_sent: %d\n", right_pointer_tap_code_sent);
    if (!right_pointer_tap_code_sent) {
      dprintf("About to send keys...\n");
      dprintf("right_report.x: %d,right_report.y: %d\n", right_report.x, right_report.y);
      if (right_report.x == 7) {
          tap_code(KC_RIGHT);
        //   dprintf("Sent KC_RIGHT\n");
          right_pointer_tap_code_sent = true;
        //   dprintf("Set right_pointer_tap_code_sent to TRUE\n");
      } else if (right_report.x == -7) {
          tap_code(KC_LEFT);
        //   dprintf("Sent KC_LEFT\n");
          right_pointer_tap_code_sent = true;
        //   dprintf("Set right_pointer_tap_code_sent to TRUE\n");
      }

      if (right_report.y == 7) {
          tap_code(KC_DOWN);
        //   dprintf("Sent KC_DOWN\n");
          right_pointer_tap_code_sent = true;
        //   dprintf("Set right_pointer_tap_code_sent to TRUE\n");
      } else if (right_report.y == -7) {
          tap_code(KC_UP);
        //   dprintf("Sent KC_UP\n");
          right_pointer_tap_code_sent = true;
        //   dprintf("Set right_pointer_tap_code_sent to TRUE\n");
      }
    }

    left_report.x = 0;
    left_report.y = 0;
    right_report.x = 0;
    right_report.y = 0;
  }

  return pointing_device_task_combined_user(left_report, right_report);
}
#endif

// #if defined(SPLIT_POINTING_ENABLE)
// report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
//     dprintf("=========================================================================================\n");
//     dprintf("Checking mouse_report.x: %d, mouse_report.y: %d\n", mouse_report.x, mouse_report.y);
//     updateLeftX(arrLeftX, mouse_report.x);
//     updateLeftY(arrLeftY, mouse_report.y);
//     if(checkAllZeros(arrLeftX) && checkAllZeros(arrLeftY)) {
//         dprintf("All zeros\n");
//         left_pointer_tap_code_sent = false;
//         dprintf("Set left_pointer_tap_code_sent to FALSE\n");
//     } else {
//         dprintf("Not all zeros\n");
//     }

//     if (!enable_mouse_pointer) {
//         dprintf("Mouse already_sent: %d\n", left_pointer_tap_code_sent);

//       if (!left_pointer_tap_code_sent) {
//         dprintf("About to send keys...\n");
//         dprintf("Mouse_report.x: %d,Mouse_report.y: %d\n", mouse_report.x, mouse_report.y);
//         if (mouse_report.x == 7) {
//             tap_code(KC_END);
//             dprintf("Sent KC_END\n");
//             left_pointer_tap_code_sent = true;
//             dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//         } else if (mouse_report.x == -7) {
//             tap_code(KC_HOME);
//             dprintf("Sent KC_HOME\n");
//             left_pointer_tap_code_sent = true;
//             dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//         }

//         if (mouse_report.y == 7) {
//             tap_code(KC_PGDN);
//             dprintf("Sent KC_PGDN\n");
//             left_pointer_tap_code_sent = true;
//             dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//         } else if (mouse_report.y == -7) {
//             tap_code(KC_PGUP);
//             dprintf("Sent KC_PGUP\n");
//             left_pointer_tap_code_sent = true;
//             dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//         }
//       }

//       mouse_report.x = 0;
//       mouse_report.y = 0;

//     }
//     return mouse_report;
// }
// #endif

#if defined(xSPLIT_POINTING_ENABLE) && defined(xPOINTING_DEVICE_COMBINED)
// report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {

//   dprintf("=========================================================================================\n");
//   dprintf("Left Pointing Device ==> left_report.x: %d, left_report.y: %d\n", left_report.x, left_report.y);
//   dprintf("Right Pointing Device ==> right_report.x: %d, right_report.y: %d\n", right_report.x, right_report.y);

//   dprintf("Checking left_report.x: %d, left_report.y: %d\n", left_report.x, left_report.y);
//   updateLeftX(arrLeftX, left_report.x);
//   updateLeftY(arrLeftY, left_report.y);
//   updateRightX(arrRightX, right_report.x);
//   updateRightY(arrRightY, right_report.y);
//   if(checkAllZeros(arrLeftX) && checkAllZeros(arrLeftY)) {
//       dprintf("All zeros\n");
//       left_pointer_tap_code_sent = false;
//     //   dprintf("Set left_pointer_tap_code_sent to FALSE\n");
//   } else {
//     //   dprintf("Not all zeros\n");
//   }
//   if(checkAllZeros(arrRightX) && checkAllZeros(arrRightY)) {
//     //   dprintf("All zeros\n");
//       right_pointer_tap_code_sent = false;
//     //   dprintf("Set right_pointer_tap_code_sent to FALSE\n");
//   } else {
//     //   dprintf("Not all zeros\n");
//   }

//   if (!enable_mouse_pointer) {
//     // dprintf("Left Mouse already_sent: %d\n", left_pointer_tap_code_sent);

//     if (!left_pointer_tap_code_sent) {
//     //   dprintf("About to send keys...\n");
//     //   dprintf("left_report.x: %d,left_report.y: %d\n", left_report.x, left_report.y);
//       if (left_report.x == 7) {
//           tap_code(KC_END);
//         //   dprintf("Sent KC_END\n");
//           left_pointer_tap_code_sent = true;
//         //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//       } else if (left_report.x == -7) {
//           tap_code(KC_HOME);
//         //   dprintf("Sent KC_HOME\n");
//           left_pointer_tap_code_sent = true;
//         //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//       }

//       if (left_report.y == 7) {
//           tap_code(KC_PGDN);
//         //   dprintf("Sent KC_PGDN\n");
//           left_pointer_tap_code_sent = true;
//         //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//       } else if (left_report.y == -7) {
//           tap_code(KC_PGUP);
//         //   dprintf("Sent KC_PGUP\n");
//           left_pointer_tap_code_sent = true;
//         //   dprintf("Set left_pointer_tap_code_sent to TRUE\n");
//       }
//     }

    // dprintf("Right Mouse already_sent: %d\n", right_pointer_tap_code_sent);
    // if (!right_pointer_tap_code_sent) {
    //   dprintf("About to send keys...\n");
    //   dprintf("right_report.x: %d,right_report.y: %d\n", right_report.x, right_report.y);
    //   if (right_report.x == 7) {
    //       tap_code(KC_RIGHT);
    //       dprintf("Sent KC_RIGHT\n");
    //       right_pointer_tap_code_sent = true;
    //       dprintf("Set right_pointer_tap_code_sent to TRUE\n");
    //   } else if (right_report.x == -7) {
    //       tap_code(KC_LEFT);
    //       dprintf("Sent KC_LEFT\n");
    //       right_pointer_tap_code_sent = true;
    //       dprintf("Set right_pointer_tap_code_sent to TRUE\n");
    //   }

    //   if (right_report.y == 7) {
    //       tap_code(KC_DOWN);
    //       dprintf("Sent KC_DOWN\n");
    //       right_pointer_tap_code_sent = true;
    //       dprintf("Set right_pointer_tap_code_sent to TRUE\n");
    //   } else if (right_report.y == -7) {
    //       tap_code(KC_UP);
    //       dprintf("Sent KC_UP\n");
    //       right_pointer_tap_code_sent = true;
    //       dprintf("Set right_pointer_tap_code_sent to TRUE\n");
    //   }
    // }


//     left_report.x = 0;
//     left_report.y = 0;
//     right_report.x = 0;
//     right_report.y = 0;

//   }


  return pointing_device_combine_reports(left_report, right_report);
}
#endif

// bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//     if (keycode == DRAG_SCROLL && record->event.pressed) {
//         set_scrolling = !set_scrolling;
//     }
//     return true;
// }


void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
  #ifdef CONSOLE_ENABLE
    //print("Running post_process_record_user.\n");
    dprintf("post_process --> KL: kc: 0x%04X, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
  #endif
  if (!record->event.pressed) {
    if(keyboard_report->mods & MOD_BIT(KC_LCTL)) {
      #ifdef CONSOLE_ENABLE
        dprint("FOUND 2nd challenge:  KC_LCTL is enabled.\n");
      #endif
      if(keyboard_report->mods & MOD_BIT(KC_LSFT)) {
        #ifdef CONSOLE_ENABLE
          dprint("FOUND 3rd challenge:  KC_LSFT is enabled.\n");
        #endif
        // Remove Left Ctrl and Left Shift keys from Triple Tap of Left Ctrl key.
        // Works with 'ctl_finished' function, Triple Tap.
        unregister_mods(MOD_BIT(KC_LCTL));
        unregister_mods(MOD_BIT(KC_LSFT));
        #ifdef CONSOLE_ENABLE
          dprint("** MODs for LCTL and LSFT have been released.\n");
        #endif
      }
    }
  }
}




#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master())
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  if (is_keyboard_master())
    return OLED_ROTATION_270  ;  // flips the display 90 degrees(portrait) if left-hand
  return rotation;
}

// const char *read_logo(void);
// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
static void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
    };

    oled_write_P(qmk_logo, false);
}

// added for oled simplification - removed *read_layer_stat

bool oled_task_user(void) {
    // Host Keyboard Layer Status
    if (is_keyboard_master()) {
      // LEFT OLED
      oled_write_P(PSTR("LAYER\n"), false);
      switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Base\n"), false);
            break;
        case _MIRROR:
            oled_write_P(PSTR("Miror\n"), false);
            break;
        // case _MEDIA:
        //     oled_write_P(PSTR("Media\n"), false);
        //     break;
        // case _NAV:
        //     oled_write_P(PSTR("Nav\n"), false);
        //     break;
        case _MOUSE:
            oled_write_P(PSTR("Mouse\n"), false);
            break;
        // case _SYM:
        //     oled_write_P(PSTR("Symbol\n"), false);
        //     break;
        // case _NUM:
        //     oled_write_P(PSTR("Number\n"), false);
        //     break;
        // case _FN:
        //     oled_write_P(PSTR("Function\n"), false);
        //     break;
        // case _GAME:
        //     oled_write_P(PSTR("Game\n"), false);
        //     break;
        case _ALT:
            oled_write_P(PSTR("Alt\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("RGB\n"), false);
            break;
        // case _RGB:
        //     oled_write_P(PSTR("RGB\n"), false);
        //     break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
      }
      // oled_write_ln(read_mode_icon(keymap_config.swap_lalt_lgui), false);
      // oled_write_ln(read_host_led_state(), false);

      // Host Keyboard LED Status
      led_t led_state = host_keyboard_led_state();
      oled_write_P(led_state.num_lock ? PSTR("\nNUMLK\n") : PSTR("    "), false);
      oled_write_P(led_state.caps_lock ? PSTR("\nCPSLK\n") : PSTR("    "), false);
      oled_write_P(led_state.scroll_lock ? PSTR("\nSCRLK\n") : PSTR("    "), false);
    } else {
      // RIGHT OLED
      // oled_write(read_logo(), false);
      render_logo();
    }
    return false;
}
#endif
