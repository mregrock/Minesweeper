//
// Created by qwe on 18.05.2022.
//

#include "Minesweeper.h"

Time::Time() : minutes(0), seconds(0) {} //конструктор класса Time

Time &Time::operator++() { //перегрузка оператора ++ класса Time
    seconds++;
    if (seconds == 60) {
        minutes++;
        seconds = 0;
    }
    if (std::to_string(seconds).size() == 1) {
        clock = std::to_string(minutes) + ":0" + std::to_string(seconds);
    } else {
        clock = std::to_string(minutes) + ":" + std::to_string(seconds);
    }
    return *this;
}

std::string Time::show_time() { //функция, возвращающая время в формате строки
    return this->clock;
}

void Time::clear() {
    this->seconds = 0;
    this->minutes = 0;
}

Minesweeper::Minesweeper() : ui{Gtk::Builder::create_from_file("minesweeper.glade")} { //конструктор класса Minesweeper
    this->count_flags = 10;
    this->first_click = false;
    this->flag_fail = false;
    this->timer = Time();
    ui->get_widget<Gtk::Box>("box", this->box);
    ui->get_widget<Gtk::Grid>("grid", this->grid);
    this->play_button = Glib::RefPtr<Gtk::Button>::cast_dynamic(ui->get_object("play_button"));
    this->play_button->signal_clicked().connect(sigc::mem_fun(*this, &Minesweeper::new_game));
    this->timer_label = Glib::RefPtr<Gtk::Label>::cast_dynamic(ui->get_object("timer_label"));
    this->flags_left = Glib::RefPtr<Gtk::Label>::cast_dynamic(ui->get_object("flags_left"));
    Gtk::Image *image = Gtk::manage(new Gtk::Image{"image/play_button.png"});
    this->play_button->set_image(*image);
    this->add_events(Gdk::BUTTON_RELEASE_MASK);
    this->add_events(Gdk::BUTTON_PRESS_MASK);
    //заполнение таблицы кнопками с обнулением используемых массивов
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            this->status[i][j] = 0;
            this->visited[i][j] = false;
            this->flag[i][j] = 0;
            this->button[i][j] = Gtk::Button();
            this->button[i][j].signal_clicked().connect(
                    sigc::bind<int, int>(sigc::mem_fun(*this, &Minesweeper::on_button_clicked), i, j)); //связь сигнала нажатия кнопки с соответствующей функцией
            this->button[i][j].signal_button_press_event().connect(
                    sigc::bind<int, int>(sigc::mem_fun(*this, &Minesweeper::flag_put), i, j)); //связь сигнала нажатия кнопкой мыши(в моей реализации - ПКМ) с соответствующей функцией постановки флага
            this->grid->attach(this->button[i][j], i, j, 1, 1); ; // добавление кнопки в сетку
            this->button[i][j].show();
        }
    }
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Minesweeper::timer_change), 1000);  //связь сигнала времени с соответствующей функцией через каждую 1 секунду
    this->add(*this->box);
    this->set_title("Cапёр");
    this->set_border_width(5);
    this->show_all();

}

bool Minesweeper::timer_change() { //изменение таймера на экране
    if (!flag_fail) {
        ++this->timer;
        this->timer_label->set_text(this->timer.show_time());
    }
    return true;
}

void Minesweeper::on_button_clicked(int i, int j) { //функция обработки нажатия на кнопку мышкой
    if (!flag_fail && flag[i][j] == 0) {
        if (!this->first_click) { //случай первого нажатия
            int count = 0;
            this->plant_mines();
            while (count_cells(i, j) != 0) { //генерируем поле до тех пор, пока клетка, на которую нажали, не станет равна 0
                this->clear_cells();
                this->plant_mines(); //расстановка мин
            }

            this->open_cells(i, j); //открытие и показ незаминированных клеток
            first_click = true;
        } else {
            if (status[i][j] == 0) {
                this->open_cells(i, j);
            } else {
                //показ всех бомб на экране, если мы нажали на клетку с бомбой
                for (int x = 0; x < 10; x++) {
                    for (int y = 0; y < 10; y++) {
                        if (status[x][y] == 1) {
                            if (!(x == i && y == j)) {
                                Gtk::Image *image = Gtk::manage(new Gtk::Image{"image/mine.png"});
                                this->button[x][y].set_image(*image);
                            }
                        }
                    }
                }
                Gtk::Image *image1 = Gtk::manage(new Gtk::Image{"image/dead_smile.png"});
                this->play_button->set_image(*image1);
                Gtk::Image *image2 = Gtk::manage(new Gtk::Image{"image/mine_temp.png"});
                this->button[i][j].set_image(*image2);
                flag_fail = true;
            }
        }
    }
}

bool check_cell(int x, int y) { //функция проверки на выход за пределы
    if (x - 1 < -1 || y - 1 < -1 || x + 1 > 10 || y + 1 > 10) {
        return false;
    }
    return true;
}

int Minesweeper::count_cells(int x, int y) { //подсчет для каждой клетки количество соседних мин
    if (!check_cell(x, y)) return 0;
    int count = 0;
    for (int x1 = -1; x1 < 2; x1++) {
        for (int y1 = -1; y1 < 2; y1++) {
            if (check_cell(x + x1, y + y1))
                count += status[x + x1][y + y1];
        }
    }
    //установка картинки для соответствующей цифры
    std::string s = "image/" + std::to_string(count) + ".png";
    Gtk::Image *image = Gtk::manage(new Gtk::Image{s});
    if (flag[x][y] == 1){ //если открытая клетка была занята флагом - флаг возвращается в запас
        flag[x][y] = 0;
        this->count_flags++;
        this->flags_left->set_text(std::to_string(this->count_flags));
    }
    this->button[x][y].set_image(*image);
    //this->button[x][y].set_label(std::to_string(count));
    return count;
}

void Minesweeper::open_cells(int x, int y) { //проход по всем возможным соседним клеткам
    if (!check_cell(x, y)) return;
    if (this->visited[x][y]) return;
    this->visited[x][y] = true;
    if (this->count_cells(x, y) != 0) return;
    for (int x1 = -1; x1 < 2; x1++) {
        for (int y1 = -1; y1 < 2; y1++) {
            open_cells(x + x1, y + y1);
        }
    }

    //this->button[x][y].set_image(image);
}

void Minesweeper::clear_cells() { //функция очистки клеток
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            this->visited[i][j] = false;
            if (this->first_click) this->flag[i][j] = 0; //проверяем, делаем ли мы первый клик, иначе все наши флаги будут утеряны
            this->status[i][j] = 0;
            Gtk::Image *image;
            if (flag[i][j] == 0) { //условие убирания флага, если на ее месте появилось незаминированная клетка
                image = Gtk::manage(new Gtk::Image{"image/blank.png"});
            }
            else{
                image = Gtk::manage(new Gtk::Image{"image/flag.png"});
            }
            this->button[i][j].set_image(*image);
        }
    }
}

void Minesweeper::new_game() { //функция перезапуска игры
    clear_cells();
    this->flag_fail = false;
    this->first_click = false;
    this->count_flags = 10;
    this->flags_left->set_text(std::to_string(count_flags));
    this->timer.clear();
    Gtk::Image *image = Gtk::manage(new Gtk::Image{"image/play_button.png"});
    this->play_button->set_image(*image);
}

void Minesweeper::plant_mines() { //функция расстановки бомб
    int count = 0;
    while (count < 10) {
        int x = rand() % 10;
        int y = rand() % 10;
        if (status[x][y] != 1) {
            status[x][y] = 1;
            count++;
        }
    }
}

bool Minesweeper::flag_put(GdkEventButton *event, int i, int j) { //функция, обрабатывающая постановку флага
    if (event->button == 3 && !flag_fail) { //проверяем, нажжат ли ПКМ и продолжается ли игра
        if (this->flag[i][j] == 0 && !visited[i][j] && count_flags > 0) { //проверка на наличие пустой клетки на месте постановки флага
            Gtk::Image *image = Gtk::manage(new Gtk::Image{"image/flag.png"});
            this->count_flags--;
            this->flags_left->set_text(std::to_string(this->count_flags));
            this->button[i][j].set_image(*image);
            this->flag[i][j] = 1;
            if (this->count_flags == 0) { //проверяем, выиграли ли мы, если потратили все флаги
                int count_win = 0;
                for (int x = 0; x < 10; x++) {
                    for (int y = 0; y < 10; y++) {
                        if (flag[x][y] == 1 && status[x][y] == 1) {
                            count_win++;
                        }
                    }
                }
                if (count_win == 10) { //если мы обозначили флажками все бомбы - мы победили
                    Gtk::Image *image1 = Gtk::manage(new Gtk::Image{"image/win_smile.png"});
                    this->play_button->set_image(*image1);
                    flag_fail = true;
                }
            }
        } else {
            if (this->flag[i][j] == 1) { //если на месте нашей клетки стоит флажок - то мы его забираем обратно
                Gtk::Image *image = Gtk::manage(new Gtk::Image{"image/blank.png"});
                this->button[i][j].set_image(*image);
                this->count_flags++;
                this->flags_left->set_text(std::to_string(this->count_flags));
                this->flag[i][j] = 0;
            }
        }
    }
    return true;
}


