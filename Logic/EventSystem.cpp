#include "EventSystem.h"

#include <QRandomGenerator>

EventSystem::EventSystem()
{
}

QVector<Event> EventSystem::generateDailyEvents()
{
    QVector<Event> events;

    int minorRoll = QRandomGenerator::global()->bounded(100);
    if (minorRoll < 70) {
        events.append(generateMinorEvent());
    } else if (minorRoll < 90) {
        events.append(generateMinorEvent());
        events.append(generateMinorEvent());
    }

    int majorRoll = QRandomGenerator::global()->bounded(100);
    if (majorRoll < 35) {
        events.append(generateMajorEvent());
    }

    return events;
}

Event EventSystem::generateMinorEvent()
{
    Event event;

    int index = QRandomGenerator::global()->bounded(4);

    if (index == 0) {
        event.name = "天气有点热";
        event.description = "街坊们说这几天明显热起来了，冷饮可能会更受欢迎。";
        event.isMajor = false;
        event.source = "邻里闲谈";
        event.credibility = 0.65;
        event.demandEffects["饮料"] = 1.12;
        event.customerFlowEffect = 1.03;
        event.isMisleading = false;
    } else if (index == 1) {
        event.name = "学生放学人流增加";
        event.description = "常客说附近学校活动变多，放学后可能会有更多学生来买零食和饮料。";
        event.isMajor = false;
        event.source = "常客交流";
        event.credibility = 0.80;
        event.demandEffects["饮料"] = 1.08;
        event.demandEffects["零食"] = 1.10;
        event.customerFlowEffect = 1.05;
        event.isMisleading = false;
    } else if (index == 2) {
        event.name = "游客似乎减少";
        event.description = "有人说最近游客少了一些，但这个消息未必准确。";
        event.isMajor = false;
        event.source = "大爷大妈情报";
        event.credibility = 0.60;
        event.demandEffects["饮料"] = 0.95;
        event.demandEffects["速食"] = 0.95;
        event.customerFlowEffect = 0.94;
        event.isMisleading = true;
    } else {
        event.name = "大家开始囤方便食品";
        event.description = "店外有人讨论最近不少人开始买泡面和面包。";
        event.isMajor = false;
        event.source = "店外闲谈";
        event.credibility = 0.72;
        event.demandEffects["速食"] = 1.12;
        event.costEffects["速食"] = 1.04;
        event.customerFlowEffect = 1.00;
        event.isMisleading = false;
    }

    return event;
}

Event EventSystem::generateMajorEvent()
{
    Event event;

    int index = QRandomGenerator::global()->bounded(4);

    if (index == 0) {
        event.name = "夏日祭典";
        event.description = "县城将举办夏日祭典，游客和学生客流明显增加。";
        event.isMajor = true;
        event.source = "报纸新闻";
        event.credibility = 0.95;
        event.demandEffects["饮料"] = 1.30;
        event.demandEffects["零食"] = 1.25;
        event.costEffects["饮料"] = 1.10;
        event.customerFlowEffect = 1.25;
        event.isMisleading = false;
    } else if (index == 1) {
        event.name = "连续暴雨";
        event.description = "天气预报显示未来将有连续暴雨，雨具和速食需求可能上涨。";
        event.isMajor = true;
        event.source = "天气预警";
        event.credibility = 0.92;
        event.demandEffects["日用品"] = 1.35;
        event.demandEffects["速食"] = 1.20;
        event.costEffects["日用品"] = 1.15;
        event.customerFlowEffect = 0.88;
        event.isMisleading = false;
    } else if (index == 2) {
        event.name = "台风预警";
        event.description = "县城发布台风预警，居民可能提前购买泡面、雨伞和应急用品。";
        event.isMajor = true;
        event.source = "县城公告";
        event.credibility = 0.98;
        event.demandEffects["速食"] = 1.45;
        event.demandEffects["日用品"] = 1.45;
        event.costEffects["速食"] = 1.20;
        event.costEffects["日用品"] = 1.20;
        event.customerFlowEffect = 0.82;
        event.isMisleading = false;
    } else {
        event.name = "学校活动周";
        event.description = "附近学校进入活动周，学生消费需求上升。";
        event.isMajor = true;
        event.source = "公告板";
        event.credibility = 0.90;
        event.demandEffects["饮料"] = 1.20;
        event.demandEffects["零食"] = 1.22;
        event.demandEffects["速食"] = 1.10;
        event.costEffects["零食"] = 1.05;
        event.customerFlowEffect = 1.12;
        event.isMisleading = false;
    }

    return event;
}
