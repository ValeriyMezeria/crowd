#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>

#include "scene.h"
#include "calculator.h"
#include "objects_pool.h"
#include "chrono"


class Engine : public QObject
{
    Q_OBJECT
public:
    Engine();

    bool isStarted() const { return m_timer->isActive(); }

    int getSimulationTime() const { return m_simulationTime; }


    void update();

    void draw(QPainter& painter);

    void pause();

    void resume();

    void scrollEvent(QWheelEvent * event);

    void mouseClickEvent(QMouseEvent * event);

    void mouseReleaseEvent(QMouseEvent * event);

signals:
    void tick();


private:
    int m_timerTick = 10;//milliseconds
    int m_simulationTime = 0;

    bool m_isMouseMove = false;

    QPoint m_mousePrevPos;

    std::chrono::time_point<std::chrono::system_clock> m_timeFrame;
    std::unique_ptr<QTimer> m_timer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Calculator> m_calculator;
};

#endif // ENGINE_H
