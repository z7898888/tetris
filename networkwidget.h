#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>

class Map;
class MapView;
class Client;
class Server;
class QListWidget;
class QTime;
class QTimer;
class QComboBox;
class QLCDNumber;

class NetworkWidget : public QWidget
{
    Q_OBJECT
private:
    QStackedWidget* stackWidget;
    QListWidget* list;
    QComboBox* select_time;
    QLCDNumber* number;
    QLabel* opponent;
    QLabel* state;

    void CreateSelectWidget();
public:
    enum GameState{Free, Ready, Gaming};

    QString opponent_ip;
    GameState game_state;

    void JoinClientList(const QString& ip);
    void DetermineOpponent();
    void StartGame();
    void EndGame();
    int start_time;
private:
    int index;
    Client* client;
    Server* server;
    MapView* view;

    QTime* time;
    QTimer* timer;
public:
    NetworkWidget(QWidget* parent = 0);

    void SendMap(const Map* map, int score, int level, int state);
    void ShowView(const Map* map, int score, int level, int state);

signals:
    void Notify_Game_Start();
    void Notify_Game_End();
    void ShowOpponentView(const Map* map, int score, int level, int state);
private slots:
    void GotoMainGUI();
    void GotoClientGUI();
    void GotoServerGUI();

    void JoiningHost();
    void StartingGame();
    void ShowTime();
};

#endif // NETWORKWIDGET_H
