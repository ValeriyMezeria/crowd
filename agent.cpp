#include "agent.h"

Agent::Agent(int id, double size, double mass, QVector2D position, QVector2D speed, QColor color, double wishSpeed) :
    SceneObject(id, position, color),
    m_size(size),
    m_mass(mass),
    m_speed(speed),
    m_color(color),
    m_wishSpeed(wishSpeed)
{

}

QVector2D Agent::getCenter() const
{
    return QVector2D(getPos().x() + m_size / 2.0, getPos().y() + m_size / 2.0);
}

