#include "network.h"
#include "QHostInfo"
#include "QMessageBox"

Network::Network() : QObject()
{
    udpsocket = new QUdpSocket;
    connect(udpsocket, SIGNAL(readyRead()), this, SLOT(reciveMessage()));
}

Network::~Network(){
    if( udpsocket ) delete udpsocket;
}


void Network::setCtrlCenter(CtrlCenter *center)
{
    if(ctrl == NULL) ctrl = center;
}

void Network::setListeningPort(int port)
{
    listening_port = port;
}

void Network::setBroadcastPort(int port)
{
    broadcast_port = port;
}

void Network::openPort()
{
    udpsocket->bind(listening_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

void Network::closePort()
{
    udpsocket->disconnectFromHost();
}

QByteArray Network::addFrameHead(const QByteArray &data)
{
    QByteArray ret;
    const char head[] ={'G','A','M','E'};
    ret.append(head,4);
    const char len = data.count()+6;
    ret.append(len);
    char bcc = 0;
    for(int i=0; i<data.count(); i++)
        bcc ^= data[i];
    ret.append(data);
    ret.append(bcc);
    return ret;
}

bool Network::isBePrased(const QByteArray &data, QByteArray * ret)
{
    if(data.count() < 5) return false;
    const char head[] = {'G','A','M','E'};

    for(int i=0; i<data.count()-5; i++){
        bool flag = true;
        for(int j=0; j<4; j++) if(data[i+j] != head[j]){flag = false; break;}

        if(flag) {
            const unsigned char len = data[i+4];
            const unsigned char bcc = data[i+len-1];
            if(len + i > data.count()) return false;
            QByteArray tmp;
            for(int k=5; k<len-1; k++){
                tmp.append(data[i+k]);
            }
            unsigned char bcc2 = 0;
            for(int k=0; k<tmp.count(); k++){
                bcc2 ^= tmp[k];
            }

            if(ret != 0) {
                ret->clear();
                ret->append(tmp);
            }

            if(bcc == bcc2) return true;
            else return false;
        }
    }

    return false;
}

void Network::sendMessage(const QByteArray &msg)
{
    QByteArray byte = addFrameHead(msg);
    udpsocket->writeDatagram(byte, QHostAddress::Broadcast, broadcast_port);
}

void Network::sendMessage(const QString ip, const QByteArray &msg)
{
    QHostAddress address;
    if(address.setAddress(ip)){
        udpsocket->writeDatagram(addFrameHead(msg), address, broadcast_port);
    }
}

void Network::reciveMessage()
{
    while(udpsocket->hasPendingDatagrams()){
        QByteArray data;
        QHostAddress sender;

        data.resize(udpsocket->pendingDatagramSize());
        udpsocket->readDatagram(data.data(), data.size(), &sender);


        QByteArray data2;

        bool isWork = isBePrased(data, &data2);
        if(isWork && ctrl) {
            ctrl->Handle(data2, sender.toString());
        }
    }
}

QString Network::getLocalIp(){
    QString localHostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(localHostName);

    QList<QHostAddress> listAddress = hostInfo.addresses();
    QString ret;
    for(int i=0; i<listAddress.count(); i++){
        QHostAddress address = listAddress.at(i);
        if( address.protocol() == QAbstractSocket::IPv4Protocol){
            ret = address.toString();break;
        }
    }
    return ret;
}
