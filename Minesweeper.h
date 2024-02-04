//
// Created by qwe on 18.05.2022.
//

#ifndef PROJECTKA_MINESWEEPER_H
#define PROJECTKA_MINESWEEPER_H
#include <gtkmm.h>
#include <iostream>
#include <string>
class Time{
    int seconds;
    int minutes;
    std::string clock;
public:
    Time();
    Time& operator++();
    void clear();
    std::string show_time();
};
class Minesweeper: public Gtk::ApplicationWindow {
    Gtk::Box *box;
    Gtk::Button button[10][10];
    bool first_click;
    int status[10][10];
    bool visited[10][10];
    int flag[10][10];
    Gtk::Grid *grid;
    Glib::RefPtr <Gtk::Label> timer_label;
    Glib::RefPtr <Gtk::Label> flags_left;
    bool flag_fail;
    Glib::RefPtr <Gtk::Builder> ui;
    Glib::RefPtr <Gtk::Button> play_button;
    Time timer;
    void clear_cells();
    void plant_mines();
    bool flag_put(GdkEventButton *event, int i, int j);
    int count_flags;
    void new_game();
public:
    Minesweeper();
    bool timer_change();
    void on_button_clicked(int i, int j);
    void open_cells(int x, int y);
    int count_cells(int x, int y);
};


#endif //PROJECTKA_MINESWEEPER_H
