// -*- tabs-indent-mode: t */
/* See LICENSE file for copyright and license details. */
#include <X11/Xft/Xft.h>
#include <X11/XF86keysym.h>

/* appearance */
static const char *fonts[] = {
	"Iosevka:size=10",
	"Icons:size=10"
};
static const char dmenufont[]       = "DejaVu Sans Mono:size=12";
static const char normbordercolor[] = "#767675";
static const char normbgcolor[]     = "#292929";
static const char normfgcolor[]     = "#FFFFFF";
static const char selbordercolor[]  = "#B34D4D";
static const char selbgcolor[]      = "#767675";
static const char selfgcolor[]      = "#FFFFFF";
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int gappx              = 8;


/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating  isterminal noswallow monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,          0,         0,        -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,          0,         0,        -1 },
	{ "st-",      NULL,       NULL,       0,            0,          1,         1,        -1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const int resizehints  = 0;    /* 1 means respect size hints in tiled resizals */
static const char clock_fmt[] = "%m/%d/%y @ %I:%M %p";

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
  { "[B]",      htile },
  { "[G]",      grid },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-nb", normbgcolor,
                                  "-nf", normfgcolor, "-sb", selbgcolor,
                                  "-sf", selfgcolor, "-l", "10",
                                  NULL };

static const char *term[]    = { "/usr/bin/gnome-terminal", NULL };

static const char *lock_cmd[]   = { "gnome-screensaver-command", "--lock", NULL };

static Key keys[] = {
	/* modifier            key                      function        argument */
	{ MODKEY,              XK_p,                    spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,    XK_Return,               spawn,          {.v = term } },
	{ MODKEY|ControlMask,  XK_b,                    togglebar,      {0} },
	{ MODKEY,              XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,              XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,              XK_d,                    incnmaster,     {.i = -1 } },
	{ MODKEY,              XK_h,                    setmfact,       {.f = -0.05} },
	{ MODKEY,              XK_l,                    setmfact,       {.f = +0.05} },
  { MODKEY|ShiftMask,    XK_h,                    setcfact,       {.f = +0.05} },
  { MODKEY|ShiftMask,    XK_l,                    setcfact,       {.f = -0.05} },
  { MODKEY|ShiftMask,    XK_o,                    setcfact,       {.f = 0} },
	{ MODKEY,              XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask,    XK_c,                    killclient,     {0} },
	{ MODKEY,              XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,              XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,              XK_m,                    setlayout,      {.v = &layouts[2]} },
  { MODKEY,              XK_b,                    setlayout,      {.v = &layouts[3]} },
  { MODKEY,              XK_g,                    setlayout,      {.v = &layouts[4]} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(               XK_1,                                    0)
	TAGKEYS(               XK_2,                                    1)
	TAGKEYS(               XK_3,                                    2)
	TAGKEYS(               XK_4,                                    3)
	TAGKEYS(               XK_5,                                    4)
	TAGKEYS(               XK_6,                                    5)
	{ MODKEY|ShiftMask,    XK_q,                    quit,           {0} },
  { MODKEY|ShiftMask,    XK_b,                    banishpointer,  {0} },
  { ControlMask|MODKEY,  XK_space,                spawn,          { .v = lock_cmd } },

};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
