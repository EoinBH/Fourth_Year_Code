// GUI to run DynamicEM algorithm & analyse generated graphs
#include "App.h"
#include <iostream>
#include <qapplication.h>
#include <QMovie>
#include <QLabel>
#include <QApplication>
#include <sstream>
#include <QtDebug>
#include <QFileDialog>
#include <QtCore/QCoreApplication>
#include "mythread.h"
#include <fstream>

App::App() {
    show_args(true);
    /*******************/
    /*     Buttons:    */
    /*******************/
    button = new QPushButton("Run Algorithm", this);
    button->setGeometry(610,250,100,30);
    connect(button,SIGNAL(clicked()), this, SLOT(run_alg()));

    quit_button = new QPushButton("Quit", this);
    quit_button->setGeometry(10,760,100,30);
    connect(quit_button,SIGNAL(clicked()),qApp,SLOT(quit()));

    jump_button = new QPushButton("Display Final Iteration", this);
    jump_button->setGeometry(10,550,150,30);
    connect(jump_button,SIGNAL(clicked()), this, SLOT(jump_to_final_it()));

    show_button = new QPushButton("Start Animation", this);
    show_button->setGeometry(10,610,120,30);
    connect(show_button,SIGNAL(clicked()), this, SLOT(show_graph()));

    speed_label = new QLabel(this);
    speed_label->setText("Control Speed of Graphs:");
    speed_label->setGeometry(10,475,150,30);

    speed_control = new QSlider(Qt::Horizontal, this);
    speed_control->setGeometry(10,505,100,30);
    speed_control->setSliderPosition(50);
    connect(speed_control,SIGNAL(valueChanged(int)), this, SLOT(speed(int)));

    toggle_view_args = new QPushButton("Hide Parameter Settings", this);
    toggle_view_args->setGeometry(10,10,170,30);
    connect(toggle_view_args,SIGNAL(clicked()), this, SLOT(toggle()));

    /*******************/
    /*   Animations:   */
    /*******************/
    graph_display_width = 1200;
    graph_display_height = 500;
    graph_display_spacing = 10;
    curr_num_graphs = 5; //Max
    int curr_x = 210;
    int curr_y = 290;
    int x_per_graph = (graph_display_width - ((curr_num_graphs - 1) * graph_display_spacing)) / curr_num_graphs;
    int y_per_graph = graph_display_height;
    if (y_per_graph >= x_per_graph) {
        y_per_graph = x_per_graph;
    } else {
        x_per_graph = y_per_graph;
    }
    for (int i = 0; i < curr_num_graphs; i++) {

        movie[i] = new QMovie(this);
        movie_label[i] = new QLabel(this);
        movie_label[i]->setGeometry(curr_x,curr_y,x_per_graph,y_per_graph);
        movie_label[i]->setMovie(movie[i]);
        movie[i]->setCacheMode(QMovie::CacheAll);
        curr_x += (x_per_graph + graph_display_spacing);
    }

    animate = new QPushButton("Generate Animation", this);
    animate->setGeometry(10,370,150,30); //was 1100,250,150,30
    connect(animate,SIGNAL(clicked()), this, SLOT(gen_anim()));

    running = new QPushButton("Is running?", this);
    running->setGeometry(10,400,100,30);
    connect(running,SIGNAL(clicked()), this, SLOT(check_running()));
    running->setHidden(true);

    finished = new QPushButton("Is finished?", this);
    finished->setGeometry(10,460,100,30);
    connect(finished,SIGNAL(clicked()), this, SLOT(check_finished()));
    finished->setHidden(true);

    terminate = new QPushButton("Terminate Process", this);
    terminate->setGeometry(10,700,150,30);
    connect(terminate,SIGNAL(clicked()), this, SLOT(terminate_process()));
    terminate->setHidden(true);

    graph_button = new QPushButton("Which graphs?", this);
    graph_button->setGeometry(10,430,100,30); //was 810,280,100,30
    connect(graph_button,SIGNAL(clicked()), this, SLOT(which_graphs()));

    graphs = new QMessageBox(this);
    dialog = true;
    info = new QMessageBox(this);
}

void App::run_alg() {
    bool permissible = true;
    unsigned int curr_num_senses = 0;
    QString dynamicEM = "./Input/DynamicEM/dynamicEM";
    QString target_word = word_entry->text();
    int curr_corpus_type = corpus_types->currentIndex() + 1;
    QString corpus_type = QString::number(curr_corpus_type); //Number from 1 to 4
    QString output_dir = "";
    QString input_files = "";
    QString path = "./Input";
    if (dir_exists(path, 1)) {
        QString list_path = path + "/list.txt";
        QByteArray ba = list_path.toLocal8Bit();
        const char* ls = ba.data();
        std::ifstream list_file;
        list_file.open(ls, std::ios::in);
        //QString data_src = "";
        if (list_file) {
            std::string item;
            bool data_found = false;
            while (getline(list_file, item) && !data_found) {
                QString curr_item = QString::fromStdString(item);
                if (curr_item == target_word + "_list") {
                    //data found
                    data_found = true;
                    input_files = path + "/" + curr_item;
                }
            }
            if (input_files == "") {
                permissible = false;
            }
            list_file.close();
        }
    } else {
        //Problem opening Input directory
        QString txt = "Problem opening Input directory";
        info->setText(txt);
        info->exec();
    }
    QString left = "";
    QString right = "";
    QString max_it = "";
    QString num_senses = "";
    bool ok;
    unsigned int val = (entries[0]->text()).toUInt(&ok);
    if (!ok) {
        left = "5"; //Default value
        entries[0]->setText("5");
    } else {
        left = QString::number(val);
    }
    val = (entries[1]->text()).toUInt(&ok);
    if (!ok) {
        right = "5"; //Default value
        entries[1]->setText("5");
    } else {
        right = QString::number(val);
    }
    val = (entries[2]->text()).toUInt(&ok);
    if (!ok) {
        max_it = "100"; //Default value
        entries[2]->setText("100");
    } else {
        max_it = QString::number(val);
    }
    val = (senses->text()).toUInt(&ok);
    if (!ok) {
        //No default, so:
        permissible = false;
    } else {
        num_senses = QString::number(val);
        curr_num_senses = val;
        if (permissible) {
            //Check output dir
            output_dir = "./Output/" + word_entry->text() + "-" + senses->text() + "-sens-" + entries[2]->text() + "-iter";
            if (!dir_exists(output_dir, 0)) {
                //dir doesn't exist so create it
                QString command = "cd ./Output && mkdir " + word_entry->text() + "-" + senses->text() + "-sens-" + entries[2]->text() + "-iter";
                QByteArray ba = command.toLocal8Bit();
                const char* cmd = ba.data();
                system(cmd);
            }
        }
    }
    QString rand_word_mix_level = "";
    float ft_val = (mix_level->text()).toFloat(&ok);
    if (!ok) {
        rand_word_mix_level = "0.0"; //Default
        mix_level->setText("0.0");
    }
    else {
        if ((ft_val >= 0.0) && (ft_val <= 1.0)) {
            rand_word_mix_level = QString::number(ft_val);
        }
        else {
            rand_word_mix_level = "0.0"; //Default
            mix_level->setText("0.0");
        }
    }
    QString whether_csv_suffix = yn[0]->currentText();
    QString whether_pad = yn[1]->currentText();
    QString sense_probs = "";
    QString vals = sense_vals->text();
    QStringList delim = vals.split("/");
    bool valid_str = true;
    float total = 0.0;
    if (delim.length() != curr_num_senses) {
        //Check if different length
        valid_str = false;
    } else {
        for (int i = 0; i < delim.length(); i++) {
            float curr_val = delim[i].toFloat(&ok);
            if (!ok) {
                //Check if curr_val is not a float
                valid_str = false;
            } else {
                total += curr_val;
            }
        }
        if (total != 1) {
            //Check if total probabilities != 1
            valid_str = false;
        }
    }
    if (valid_str) {
        sense_probs = vals;
    } else {
        //Invalid string:
        //Set to default (all values are equal and sum to 1)
        double default_value = 1.0 / curr_num_senses;
        for (unsigned int i = 0; i < curr_num_senses; i++) {
            sense_probs += QString::number(default_value);
            if (!(i + 1 == curr_num_senses)) { // if not last sense
                sense_probs += "/";
            }
        }
        sense_vals->setText(sense_probs);
    }
    QString wpt = "";
    if (words_prior_type->currentText() == "unif") {
        wpt = "unif";
    } else {
        wpt = "corpus"; //Default
    }
    QString sense_rand = yn[2]->currentText();
    QString word_rand = yn[3]->currentText();
    QString set_seed = yn[4]->currentText();
    QString rand_mix = yn[5]->currentText();
    QString rand_word_mix = yn[6]->currentText();
    QString unsup = yn[7]->currentText();
    QString time_stamp = yn[8]->currentText();
    /*  -targ: Form1/.../FormN
     *  -corpus_type: N={1,2,3,4}, default: 1
     *  -group_size: N COME BACK TO THIS
     *  -left: N, default: 5
     *  -right: N, default: 5
     *  ...
     */
    if (permissible) {
        QString EM_command = dynamicEM + " -targ " + target_word + " -corpus_type " + corpus_type +
                " -expts " + output_dir + " -files " + input_files + " -left " + left + " -right " + right +
                " -whether_csv_suffix " + whether_csv_suffix + " -whether_pad " + whether_pad +
                " -num_senses " + num_senses + " -senseprobs_string " + sense_probs + " -sense_rand " + sense_rand +
                " -word_rand " + word_rand + " -set_seed " + set_seed + " -rand_mix " + rand_mix + " -rand_word_mix "
                + rand_word_mix + " -rand_word_mix_level " + rand_word_mix_level + " -words_prior_type " + wpt +
                " -unsup " + unsup + " -max_it " + max_it + " -time_stamp " + time_stamp;
        QByteArray ba = EM_command.toLocal8Bit();
        mThread[0] = new MyThread(ba.data());
        mThread[0]->start();
    }
    else {
        QString txt = "Error: impermissible arguments";
        info->setText(txt);
        info->exec();
    }
}

void App::show_graph() {
    //Check if any animations exist
    dialog = false;
    if (which_graphs()) {
        QString txt = "No graphs available";
        info->setText(txt);
        info->exec();
    }
    else {
        bool no_graphs_selected = false;
        int orig_num_graphs = curr_num_graphs;
        QStringList file_names = QFileDialog::getOpenFileNames(this, tr("Open File"), "./Graphs", "All files (*.gif)");
        if (file_names.length() == 0) {
            no_graphs_selected = true;
        } else if (file_names.length() <= 5) {
            curr_num_graphs = file_names.length();
        } else {
            curr_num_graphs = 5;
            QString txt = "Too many files.\nDefaulting to maximum number of files = 5";
            info->setText(txt);
            info->exec();
        }
        if (!no_graphs_selected) {
            //Stop previous gifs
            for (int i = 0; i < orig_num_graphs; i++) {
                movie[i]->stop();
                movie_label[i]->hide();
            }

            int curr_x = 210;
            int curr_y = 290;
            int x_per_graph = (graph_display_width - ((curr_num_graphs - 1) * graph_display_spacing)) / curr_num_graphs;
            int y_per_graph = graph_display_height;
            if (y_per_graph >= x_per_graph) {
                y_per_graph = x_per_graph;
            } else {
                x_per_graph = y_per_graph;
            }
            for (int i = 0; i < curr_num_graphs; i++) {
                movie_label[i]->setGeometry(curr_x,curr_y,x_per_graph,y_per_graph);
                movie_label[i]->show();
                movie[i]->setFileName(file_names[i]);
                movie[i]->start();
                movie[i]->setScaledSize(QSize(x_per_graph,y_per_graph));
                curr_x += (x_per_graph + graph_display_spacing);
            }
        }
    }
    dialog = true;
}

void App::speed(int speed) {
    //Speed is percentage so 100 = normal speed, 200 = twice as fast
    for (int i = 0; i < curr_num_graphs; i++) {
        movie[i]->setSpeed(2*speed);
    }
}

void App::jump_to_final_it() {
    for (int i = 0; i < curr_num_graphs; i++) {
        int final_frame = (movie[i]->frameCount()-1);
        movie[i]->jumpToFrame(final_frame);
        movie[i]->stop();
    }
}

void App::gen_anim() {
    //Graph based on current word and number of senses
    //First check if animation already exists
    QString file_name = "./Animation/animation-" + word_entry->text() + "-" + senses->text() + "-sens-" + entries[2]->text() + "-iter.gif";
    QByteArray ba = file_name.toLocal8Bit();
    const char* anim_path = ba.data();
    std::ifstream anim_file;
    anim_file.open(anim_path);
    if (anim_file) {
        QString txt = "File already exists";
        info->setText(txt);
        info->exec();
    }
    else {
        //File doesn't already exist so check if data exists
        QString path = "./Output/" + word_entry->text() + "-" + senses->text() + "-sens-" + entries[2]->text() + "-iter";
        if (dir_exists(path, 1)) {
            //Data exists
            QString list_path = path + "/list.txt";
            ba = list_path.toLocal8Bit();
            const char* ls = ba.data();
            std::ifstream list_file;
            list_file.open(ls, std::ios::in);
            QString data_src = "";
            if (list_file) {
                std::string item;
                bool data_found = false;
                while (getline(list_file, item) && !data_found) {
                   if (item.substr(0, 1) == "s") { //sense_distribs
                       //data found
                       data_found = true;
                       data_src = path + "/" + QString::fromStdString(item);
                       QString command = "./Animation/animation " + data_src + " " + word_entry->text() + " " + entries[2]->text();
                       ba = command.toLocal8Bit();
                       //Start blocking demo 2
                       //system(ba.data());
                       //End blocking demo 2
                       //Generate animation in new thread
                       mThread[1] = new MyThread(ba.data());
                       mThread[1]->start();
                   }
                }
                if (data_src == "") {
                    QString txt = "No data found in folder";
                    info->setText(txt);
                    info->exec();
                }
                list_file.close();
            }
        } else {
            //Data doesn't exist
            QString txt = "No data available";
            info->setText(txt);
            info->exec();
        }
    }
}

bool App::dir_exists(QString path, bool ls) {
    QString command = "cd " + path + " && ls -t > list.txt";
    QByteArray ba = command.toLocal8Bit();
    const char* cmd = ba.data();
    system(cmd);
    QString list_src = path + "/list.txt";
    ba = list_src.toLocal8Bit();
    const char* list_path = ba.data();
    std::ifstream list_file;
    list_file.open(list_path);
    if (list_file) {
        //list.txt was created so dir exists
        list_file.close();
        if (!ls) {
            //if remove list
            command = "cd " + path + " && rm list.txt";
            ba = command.toLocal8Bit();
            const char* cmd = ba.data();
            system(cmd);
        }
        return true;
    } else {
        //list.txt wasn't created so dir doesn't exist
        return false;
    }
}

bool App::which_graphs() {
    bool no_graphs = false;
    QString cmd = "cd ./Graphs && ls -t > list.txt";
    QByteArray ba = cmd.toLocal8Bit();
    mThread[2] = new MyThread(ba.data());
    mThread[2]->start();
    QString list_path = "./Graphs/list.txt";
    ba = list_path.toLocal8Bit();
    const char* ls = ba.data();
    std::ifstream list_file;
    list_file.open(ls, std::ios::in);
    std::string msg = "";
    if (list_file) { //Thread creating list.txt mightn't be finished
        std::string item;
        while (getline(list_file, item)) {
            if (item != "list.txt") {
                msg += item + "\n";
            }
        }
        if (msg == "") {
            msg += "No animations available yet";
            no_graphs = true;
        }
    } else {
        QString txt = "Problem opening Graphs directory";
        info->setText(txt);
        info->exec();
    }
    if (dialog) {
        QString txt = QString::fromStdString(msg);
        graphs->setText(txt);
        graphs->exec();
    }
    return no_graphs;
}

void App::check_running() {
    /*
    if (mThread[thread]->isRunning() == true) {
        qDebug() << "Process is running...\n";
    } else {
        qDebug() << "Process is not running.\n";
    }
    */
}

void App::check_finished() {
    /*
    if (mThread[thread]->isFinished() == true) {
        qDebug() << "Process is finished.\n";
    } else {
        qDebug() << "Process is not finished...\n";
    }
    */
}

void App::terminate_process() {
    /*
    qDebug() << "Process termination...\n";
    mThread[thread]->terminate();
    */
}

void App::hide_args() {
    button->hide();
    button->setGeometry(0,0,0,0);
    word_entry->hide();
    word_entry->setGeometry(0,0,0,0);
    corpus_types->hide();
    corpus_types->setGeometry(0,0,0,0);
    for (int i = 0; i < 3; i++) {
        entries[i]->hide();
        entries[i]->setGeometry(0,0,0,0);
    }
    for (int i = 0; i < 9; i++) {
        yn[i]->hide();
        yn[i]->setGeometry(0,0,0,0);
    }
    senses->hide();
    senses->setGeometry(0,0,0,0);
    mix_level->hide();
    mix_level->setGeometry(0,0,0,0);
    words_prior_type->hide();
    words_prior_type->setGeometry(0,0,0,0);
    sense_vals->hide();
    sense_vals->setGeometry(0,0,0,0);
    for (int i = 0; i < 18; i++) {
        label[i]->hide();
        label[i]->setGeometry(0,0,0,0);
    }
    hidden = true;
    toggle_view_args->setText("Show Parameter Settings");
}

void App::show_args(bool init) {
    /*******************/
    /*    Arguments:   */
    /*******************/
    if (init) {
        word_entry = new QLineEdit(this);
        word_entry->setGeometry(10,70,100,30); //setGeometry(x, y, w, h)
        word_entry->setText("mouse");

        corpus_types = new QComboBox(this);
        corpus_types->setGeometry(210,70,100,30);
        int total_corpus_types = 4;
        QString text = "";
        for (int i = 1; i <= total_corpus_types; i++) {
            text = "Type " + QString::number(i);
            corpus_types->insertItem(i, text);
        }

        entries[0] = new QLineEdit(this);
        entries[1] = new QLineEdit(this);
        entries[2] = new QLineEdit(this);
        entries[0]->setGeometry(410,70,100,30);
        entries[1]->setGeometry(610,70,100,30);
        entries[2]->setGeometry(10,250,100,30);
        entries[0]->setText("5"); //Initial
        entries[1]->setText("5"); //Initial
        entries[2]->setText("100"); //Initial

    } else {
        button->setGeometry(610,250,100,30);
        button->show();

        word_entry->setGeometry(10,70,100,30);
        word_entry->show();

        corpus_types->setGeometry(210,70,100,30);
        corpus_types->show();

        entries[0]->setGeometry(410,70,100,30);
        entries[1]->setGeometry(610,70,100,30);
        entries[2]->setGeometry(10,250,100,30);
        entries[0]->show();
        entries[1]->show();
        entries[2]->show();
    }

    int h = 70;
    int w = 0;
    int j = 0;
    for (int i = 0; i < 17; i++, w++, j++) {
        //w++;
        if (i%5 == 0) {
            w = 0;
            if (i != 0) {
                h += 60;
            }
        }
        if ((i != 0) && (i != 1) && (i != 2) && (i != 3) && (i != 6) && (i != 12)
                && (i != 13) && (i != 15)) {
            if (init) {
                yn[j] = new QComboBox(this);
                yn[j]->setGeometry(10+(200*w),h,100,30);
                yn[j]->insertItem(0, "yes");
                yn[j]->insertItem(1, "no");
            } else {
                yn[j]->setGeometry(10+(200*w),h,100,30);
                yn[j]->show();
            }

        }
        else {
            j--;
        }
    }
    if (init) {
        //Defaults: "no"
        yn[2]->setCurrentIndex(1);
        yn[3]->setCurrentIndex(1);
        yn[5]->setCurrentIndex(1);
        yn[6]->setCurrentIndex(1);
        yn[8]->setCurrentIndex(1);

        senses = new QLineEdit(this);
        senses->setGeometry(210,130,100,30);
        senses->setText("2"); //Initial

        mix_level = new QLineEdit(this);
        mix_level->setGeometry(410,190,100,30);
        mix_level->setText("0.0"); //Default

        words_prior_type = new QComboBox(this);
        words_prior_type->setGeometry(610,190,100,30);
        words_prior_type->insertItem(0, "corpus");
        words_prior_type->insertItem(1, "unif");

        sense_vals = new QLineEdit(this);
        sense_vals->setGeometry(410,250,100,30);
        sense_vals->setText("0.5/0.5"); //Initial
    } else {
        senses->setGeometry(210,130,100,30);
        senses->show();

        mix_level->setGeometry(410,190,100,30);
        mix_level->show();

        words_prior_type->setGeometry(610,190,100,30);
        words_prior_type->show();

        sense_vals->setGeometry(410,250,100,30);
        sense_vals->show();
    }

    /*******************/
    /*     Labels:     */
    /*******************/
    h = 40;
    w = 0;
    for (int i = 0; i < 18; i++) {
        w++;
        if (i%5 == 0) {
            w = 0;
            if (i != 0) {
                h += 60;
            }
        }
        if (init) {
            label[i] = new QLabel(this);
        }
        if (i == 17) {
            if (init) {
                label[i]->setGeometry(10+(200*w),h,200,30);
            } else {
                label[i]->setGeometry(10+(200*w),h,200,30);
                label[i]->show();
            }
        } else {
            if (init) {
                label[i]->setGeometry(10+(200*w),h,150,30);
            } else {
                label[i]->setGeometry(10+(200*w),h,150,30);
                label[i]->show();
            }
        }
    }

    if (init) {
        label[0]->setText("Target Word:");
        label[1]->setText("Corpus Type:");
        label[2]->setText("Left:");
        label[3]->setText("Right:");
        label[4]->setText("Whether CSV Suffix:");
        label[5]->setText("Whether Pad:");
        label[6]->setText("Number of Senses:");
        label[7]->setText("Sense Random:");
        label[8]->setText("Word Random:");
        label[9]->setText("Set Seed:");
        label[10]->setText("Random Mix:");
        label[11]->setText("Random Word Mix:");
        label[12]->setText("Random Word Mix Level:");
        label[13]->setText("Words Prior Type:");
        label[14]->setText("Unsupervised:");
        label[15]->setText("Maximum Iteration:");
        label[16]->setText("Time Stamp:");
        label[17]->setText("Sense Values Delimited with \"/\":");
    }
    hidden = false;
    if (!init) {
        toggle_view_args->setText("Hide Parameter Settings");
    }
}

void App::toggle() {
    if (hidden) {
        show_args(false);
    } else {
        hide_args();
    }
}
