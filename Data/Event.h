#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QMap>

class Event
{
public:
    QString name;
    QString description;
    bool isMajor;
    QString source;
    double credibility;
    QMap<QString, double> demandEffects;
    QMap<QString, double> costEffects;
    double customerFlowEffect;
    bool isMisleading;

    Event();
};

#endif // EVENT_H
