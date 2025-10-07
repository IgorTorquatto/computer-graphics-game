#pragma once

void desenhaMenu();

/* Trata hover e clique do mouse */
void menu_motion(int button, int state, int x, int y);

void menu_focus_previous();
void menu_select_focused();
void menu_focus_next();
