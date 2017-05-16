#ifndef AGENT_H
#define AGENT_H

#include <QVector2D>

#include "scene_object.h"

class Agent : public SceneObject
{
public:
    Agent(int id, double size, double mass, QVector2D position, QVector2D speed, QColor color, double wishSpeed);


    inline double getSize() const { return m_size; }

    inline QVector2D getSpeed() const { return m_speed; }

    inline double getMass() const { return m_mass; }

    inline QVector2D getPrevPos() const { return m_prevPos; }

    inline QColor getColor() const {return m_color; }

    QVector2D getCenter() const;

    inline double getWishSpeed() const { return m_wishSpeed; }


    void setSpeed(const QVector2D speed) { m_speed = speed; }

    void setPrevPos(){ m_prevPos = this->getCenter(); }


    bool operator==(const Agent& a) const
    {
        if(this->getID() == a.getID())
            return true;
        return false;
    }


private:
    double m_size;
    double m_mass;
    QVector2D m_speed;
    QVector2D m_prevPos;
    QVector2D m_acceleration;
    QColor m_color;
    double m_wishSpeed;
};

#endif // AGENT_H
