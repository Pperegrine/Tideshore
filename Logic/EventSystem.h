#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <QVector>

#include "../data/Event.h"

class EventSystem
{
public:
    EventSystem();

    QVector<Event> generateDailyEvents();
    Event generateMinorEvent();
    Event generateMajorEvent();
};

#endif // EVENTSYSTEM_H
