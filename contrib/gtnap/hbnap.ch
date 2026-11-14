/* HBNAP - NAppGUI/NAppForms Harbour port */

/* Flags for HBNAP_FORMS_LOAD() */
#define HBNAP_FORMS_RESIZABLE           1
#define HBNAP_FORMS_CLOSE_ON_ESC        2
#define HBNAP_FORMS_CLOSE_ON_RETURN     4
#define HBNAP_FORMS_EMBEDDED_PANEL      8

/* Return values for HBNAP_FORMS_MODAL() */
#define HBNAP_CLOSED_BY_ESC             1
#define HBNAP_CLOSED_BY_RETURN          2
#define HBNAP_CLOSED_BY_BUTTON          3

/* Alignment values */
#define HBNAP_LEFT                      1
#define HBNAP_CENTER                    2
#define HBNAP_RIGHT                     3

/* Key codes, for HBNAP_HOTKEY() */
#define HBNAP_KEY_F1                     1
#define HBNAP_KEY_F2                     2
#define HBNAP_KEY_F3                     3
#define HBNAP_KEY_F4                     4
#define HBNAP_KEY_F5                     5
#define HBNAP_KEY_F6                     6
#define HBNAP_KEY_F7                     7
#define HBNAP_KEY_F8                     8
#define HBNAP_KEY_F9                     9
#define HBNAP_KEY_F10                    10
#define HBNAP_KEY_F11                    11
#define HBNAP_KEY_F12                    12
#define HBNAP_KEY_F13                    13
#define HBNAP_KEY_F14                    14
#define HBNAP_KEY_F15                    15
#define HBNAP_KEY_F16                    16
#define HBNAP_KEY_F17                    17
#define HBNAP_KEY_F18                    18
#define HBNAP_KEY_F19                    19

#define HBNAP_KEY_ESC                    20
#define HBNAP_KEY_RETURN                 21

#define HBNAP_KEY_A                      30
#define HBNAP_KEY_B                      31
#define HBNAP_KEY_C                      32
#define HBNAP_KEY_D                      33
#define HBNAP_KEY_E                      34
#define HBNAP_KEY_F                      35
#define HBNAP_KEY_G                      36
#define HBNAP_KEY_H                      37
#define HBNAP_KEY_I                      38
#define HBNAP_KEY_J                      39
#define HBNAP_KEY_K                      40
#define HBNAP_KEY_L                      41
#define HBNAP_KEY_M                      42
#define HBNAP_KEY_N                      43
#define HBNAP_KEY_O                      44
#define HBNAP_KEY_P                      45
#define HBNAP_KEY_Q                      46
#define HBNAP_KEY_R                      47
#define HBNAP_KEY_S                      48
#define HBNAP_KEY_T                      49
#define HBNAP_KEY_U                      50
#define HBNAP_KEY_V                      51
#define HBNAP_KEY_W                      52
#define HBNAP_KEY_X                      53
#define HBNAP_KEY_Y                      54
#define HBNAP_KEY_Z                      55

#define HBNAP_KEY_0                      60
#define HBNAP_KEY_1                      61
#define HBNAP_KEY_2                      62
#define HBNAP_KEY_3                      63
#define HBNAP_KEY_4                      64
#define HBNAP_KEY_5                      65
#define HBNAP_KEY_6                      66
#define HBNAP_KEY_7                      67
#define HBNAP_KEY_8                      68
#define HBNAP_KEY_9                      69

/* Key modifiers, for HBNAP_HOTKEY() - combine with hb_bitOr() */
#define HBNAP_MOD_SHIFT                  1
#define HBNAP_MOD_CONTROL                2
#define HBNAP_MOD_ALT                    4
