#include "gtkmm.h"
#include "Minesweeper.h"

int main(int argc, char **argv) {
    auto app = Gtk::Application::create(argc, argv);

    Minesweeper minesweeper;

    return app->run(minesweeper);
}