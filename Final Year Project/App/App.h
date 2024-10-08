#ifndef APP_H
#define APP_H
// #include <QtWidgets>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLCDNumber>
#include <QComboBox>
#include <QSlider>
#include <QMovie>
#include <QLabel>
#include <QMessageBox>
#include <QtCore/QCoreApplication>
#include "mythread.h"


class App : public QWidget {
  Q_OBJECT

public:
  App();
    
public slots:
  void run_alg();
  void show_graph();
  void speed(int speed);
  void jump_to_final_it();
  void check_running();
  void check_finished();
  void terminate_process();
  void gen_anim();
  bool which_graphs();
  bool dir_exists(QString path, bool ls);
  void hide_args();
  void show_args(bool init);
  void toggle();

private:
  QPushButton *button;
  QPushButton *show_button;
  QLineEdit *word_entry;
  QLineEdit *entries[3];
  QComboBox *corpus_types;
  QLineEdit *senses;
  QPushButton *quit_button;
  QMovie *movie[5];
  QLabel *movie_label[5];
  QLabel *label[18];
  QSlider *speed_control;
  QLabel *speed_label;
  QPushButton *jump_button;
  QComboBox *yn[9];
  QComboBox *words_prior_type;
  QLineEdit *mix_level;
  QLineEdit *sense_vals;
  MyThread *mThread[3];
  QPushButton *running;
  QPushButton *finished;
  QPushButton *animate;
  QPushButton *terminate;
  QPushButton *graph_button;
  QMessageBox *graphs;
  QPushButton *toggle_view_args;
  QMessageBox *info;
  bool dialog;
  int graph_display_width;
  int graph_display_height;
  int graph_display_spacing;
  int curr_num_graphs;
  bool hidden;
};

#endif // APP_H
