#ifndef THUMBSENDER_H
#define THUMBSENDER_H

#include <QHostAddress>
#include <memory>
#include "users.h"

class ThumbSender : public QObject
{
    Q_OBJECT
    public:
        ThumbSender(QHostAddress addr, qint16 port, shared_ptr<Users> users, QObject *parent = 0);
        ~ThumbSender();
    public slots:
        void process();
    signals:
        void finished();
        void error(QString err);
    private:
        QHostAddress addr;
        qint16 port;
        shared_ptr<Users> users;

    signals:

    public slots:
};

#endif // THUMBSENDER_H
