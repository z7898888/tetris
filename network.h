#ifndef NETWORK_H
#define NETWORK_H

#include "QString"
#include "QUdpSocket"
//#include "ctrlcenter.h"

class Handler{
public:
    virtual bool IsMe(const QByteArray& data, const QString& ip) = 0;
    virtual void Action(const QByteArray& data, const QString& ip) = 0;
};

class CtrlCenter{
    QList<Handler*> list;
public:
    void Handle(const QByteArray& data, const QString& ip){
        for(int i=0; i<list.count(); i++){
            Handler* handler = list.at(i);
            if(handler->IsMe(data, ip)) handler->Action(data,ip);
        }
    }

    void Register(Handler* handler){
        if(handler == NULL) return;

        for(int i=0; i<list.count(); i++){
            if(handler == list.at(i)) return;
        }

        list.append(handler);
    }
};

class Network : public QObject
{
    Q_OBJECT
private:
    QUdpSocket* udpsocket;
    QByteArray addFrameHead(const QByteArray &data);
    bool isBePrased(const QByteArray& data, QByteArray* ret = 0);

    CtrlCenter* ctrl;
    quint16 broadcast_port;
    quint16 listening_port;
public:
    Network();
    ~Network();
    void sendMessage(const QByteArray &msg);
    void sendMessage(const QString ip, const QByteArray& msg);

    void setCtrlCenter(CtrlCenter* center);
    void setBroadcastPort(int port);
    void setListeningPort(int port);

    void openPort();
    void closePort();

    static QString getLocalIp();

private slots:
    void reciveMessage();
};

#endif // NETWORK_H
