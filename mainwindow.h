#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <graphicsview.h>
#include <mapview.h>

class QStackedWidget;
class NetworkWidget;
class Map;

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    bool end_flag;
    int time_id;
    MapView* game_view;
    MapView* nextItemFrame;
    MapView* net_game_view;

    QFrame* middleFrame;
    NetworkWidget* online;
    QStackedWidget* selectStackedWidget;

    QLabel* score;
    QLabel* level;
    QLabel* state;
    void CreateMiddlePanel();
    void CreateSelectPanel();

    QWidget* self;
    QLabel* self_score;
    QLabel* self_level;
    QLabel* self_state;
    QWidget* other;
    QLabel* other_score;
    QLabel* other_level;
    QLabel* other_state;

    GraphicsView* view;

    enum GameState{Ready, Gaming};
    GameState game_state;
    QPushButton* selectButton;
    bool flag;  //当前是网络版还是单机版

    void UpdatePrint();
    void UpdateNetPrint();
    void CheckWinOrLose();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
public slots:
    void IsNetGame();
    void StartGame();
    void EndGame();
    void ShowOpponentView(const Map* map, int score, int level, int state);
};

#endif // MAINWINDOW_H
