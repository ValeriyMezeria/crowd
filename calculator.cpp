#include "calculator.h"

#include <QTime>

#include <algorithm>

#include "json_manager.h"
#include "general_builder.h"


Calculator::Calculator(QPoint sceneSize, std::shared_ptr<ObjectsPool> pool) :
    m_sceneSize(sceneSize),
    m_pool(pool)
{
    QTime t = QTime::currentTime();
    qsrand((uint)t.msec());
}

void Calculator::move(Agent &agent)
{
    agent.setPos(agent.getPos() + agent.getSpeed() * m_time);
}

bool Calculator::isInExit(const Agent &agent)
{
    auto exits = m_pool->getExits();
    QVector2D a = agent.getCenter();
    QVector2D b = agent.getPrevPos();

    for(auto i : exits)
    {
        QVector2D c = i.getBegin();
        QVector2D d = i.getEnd();
        double common = (b.x() - a.x())*(d.y() - c.y()) - (b.y() - a.y())*(d.x() - c.x());

        if (common == 0)
            continue;

        double rH = (a.y() - c.y())*(d.x() - c.x()) - (a.x() - c.x())*(d.y() - c.y());
        double sH = (a.y() - c.y())*(b.x() - a.x()) - (a.x() - c.x())*(b.y() - a.y());

        double r = rH / common;
        double s = sH / common;

        if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
            return true;
    }
    return false;
}

double Calculator::getRandomNumber(double a, double b)
{
    //usleep(100);

    if (a == b)
        return a;

    double hack = 1000;

    a *= hack;
    b *= hack;

    return (qrand() % ((int)b - (int)a) + (int)a) / hack;
}

QVector2D Calculator::getNearestExit(const Agent &agent)
{
    auto exits = m_pool->getExits();
    QVector2D coord(0,0);
    double distance = INFINITY;

    for(auto i : exits)
    {
        QVector2D point(0, 0);

        double tmp = getDistanceToSide(i.getBegin(), i.getEnd(), agent, point);

        if(tmp < distance)
        {
            distance = tmp;
            coord = point;
        }
    }

    return coord;
}

QVector2D Calculator::calcPanicForce(const Agent &agent)
{
    QVector2D coord = getNearestExit(agent);

    if(agent.getCenter() == coord)
        return QVector2D(0, 0);

    QVector2D desiredSpeed = calcNormal(agent.getCenter(), coord);

    double a = sqrt(pow(agent.getWishSpeed(),2)/desiredSpeed.lengthSquared());
    desiredSpeed *= a;
    QVector2D panicForce = (desiredSpeed - agent.getSpeed()) / m_param.deltaT * agent.getMass();
    return panicForce;
}

QVector2D Calculator::calcNormal(QVector2D a, QVector2D b)
{
    return QVector2D(b.x() -  a.x(), b.y() - a.y()).normalized();
}

QVector2D Calculator::calcTau(QVector2D n, QVector2D speed)
{
    QVector2D tauContrClockWise(n.y(), -n.x());
    QVector2D tauClockWise(-n.y(), n.x());

    if (scalarMultiplication(tauClockWise, speed.normalized()) > 0)
        return tauClockWise;
    else
        return tauContrClockWise;
}

QVector2D Calculator::calcCrossAgentForce(const Agent &agent)
{
    QVector2D crossAgentForce(0,0);

    auto agents = m_pool->getAgents();
    for(Agent i : agents)
    {
        if(agent == i)
            continue;

        double D = agent.getSize() + i.getSize() - (i.getCenter() - agent.getCenter()).length();

        QVector2D n = calcNormal( i.getCenter(), agent.getCenter());
        QVector2D deltaV = i.getSpeed() - agent.getSpeed();
        QVector2D tau = calcTau(n, agent.getSpeed());//normal rotated on 90 degrees

        QVector2D repulsionForce = m_param.K*Heaviside(D)*D*n;
        QVector2D frictionForce = m_param.K*Heaviside(D)*D*deltaV*tau*tau;

        m_physicalForcesAgentSum += repulsionForce.length();
        m_physicalForcesAgentSum += frictionForce.length();

        crossAgentForce += m_param.A*n*exp(D/m_param.B) + repulsionForce + frictionForce;
    }

    return crossAgentForce;
}

QVector2D Calculator::calcWallForce(const Agent &agent)
{
    QVector2D wallForce(0,0);
    auto obstacles = m_pool->getObstacles();
    for(auto i : obstacles)
    {
        QVector2D nearestPoint(getMinDistanceToObstalce(agent, i));

        double D = agent.getSize() - (agent.getCenter() - nearestPoint).length();

        QVector2D n = calcNormal(nearestPoint, agent.getCenter());
        QVector2D tau = calcTau(n, agent.getSpeed());

        QVector2D repulsionForce = m_param.Kwall*Heaviside(D)*D*n;
        QVector2D frictionForce = m_param.Kwall*Heaviside(D)*D*agent.getSpeed()*tau*tau;

        m_physicalForcesAgentSum += repulsionForce.length();
        m_physicalForcesAgentSum += frictionForce.length();

        wallForce += m_param.Awall*n*exp(D/m_param.Bwall) + repulsionForce - frictionForce;
    }
    return wallForce;
}

QVector2D Calculator::getMinDistanceToObstalce(const Agent &agent, const Obstacle &obstacle)
{
    QVector2D nearestSide(0,0);
    double distanceToSide = INFINITY;
    auto apexes = obstacle.getAbsolutePoints();

    QVector2D prevApex(-1,-1);

    for(auto i : apexes)
    {
        QVector2D nearestPoint;
        if(prevApex == QVector2D(-1,-1))
        {
            prevApex = i;
            continue;
        }
        double tmp = getDistanceToSide(prevApex, i, agent, nearestPoint);

        if( tmp < distanceToSide )
        {
            distanceToSide = tmp;
            nearestSide = nearestPoint;
        }
    }
    return nearestSide;
}


//original algorithm pseudocode
//    distance( Point P, Segment P0:P1 )
//          {
//          v = P1 - P0
//          w = P - P0
//          if ( (c1 = w · v) <= 0 )
//                return d(P, P0)
//          if ( (c2 = v · v) <= c1 )
//                return d(P, P1)
//          b = c1 / c2
//          Pb = P0 + bv
//          return d(P, Pb)
//          }
double Calculator::getDistanceToSide(const QVector2D &a,const QVector2D &b, const Agent &agent, QVector2D &result)
{
    QVector2D v(b - a);
    QVector2D w(agent.getCenter() - a);

    double c1 = scalarMultiplication(w, v);
    double c2 = scalarMultiplication(v, v);

    if(c1 <= 0)
    {
        result = a;
        return distanceBetweenPoints(agent.getCenter(), a);
    }

    if(c2 <= c1)
    {
        result = b;
        return distanceBetweenPoints(agent.getCenter(), b);
    }

    double k = c1/c2;

    result.setX(a.x() + k*v.x());
    result.setY(a.y() + k*v.y());

    return distanceBetweenPoints(agent.getCenter(), result);
}

double Calculator::distanceBetweenPoints(const QVector2D &a, const QVector2D &b)
{
    return sqrt(pow(b.x() - a.x(), 2) + pow( b.y() - a.y(), 2) );
}

double Calculator::scalarMultiplication(const QVector2D &a, const QVector2D &b)
{
    return a.x()*b.x() + a.y()*b.y();
}

int Calculator::Heaviside(double n)
{
    return n >= 0 ? 1 : 0;
}

void Calculator::calcForce(Agent &agent)
{
    QVector2D panicForce = calcPanicForce(agent);
    QVector2D crossAgentForce = calcCrossAgentForce(agent);
    QVector2D wallForce = calcWallForce(agent);
    QVector2D totalForce = panicForce + crossAgentForce + wallForce;

    if (m_iscollectStat)
        emit sendStatSignal(agent, m_physicalForcesAgentSum);

    //qDebug() << m_physicalForcesAgentSum;
    m_physicalForcesAgentSum = 0;

    QVector2D speed = agent.getSpeed() + m_time * totalForce / agent.getMass();
    //qDebug() << "id" << agent.getID() << "Speed" << speed;
    agent.setSpeed(speed);
}

QVector2D Calculator::getPointOnLine(QVector2D a, QVector2D b)
{
    if (b.x() == a.x())
        return QVector2D(a.x(), getRandomNumber(std::min(a.y(), b.y()), std::max(a.y(), b.y())));
    else
    {
        double k = (b.y() - a.y()) / (b.x() - a.x());
        double c = a.y() - k * a.x();
        double x = getRandomNumber(std::min(a.x(), b.x()), std::max(a.x(), b.x()));

        return QVector2D(x, k * x + c);
    }
}

void Calculator::entryProcess()
{
    for (auto& i : m_pool->getEntries())
    {
        double curPeriod = getRandomNumber(i.getPeriodRange().first, i.getPeriodRange().second);

        QJsonObject configData = JsonManager::parseJsonFile(JsonManager::getConfPath());

        if (i.getTimeFromLastGenerate() >= curPeriod)
        {
            QVector2D entry(i.getEnd() - i.getPos());
            entry.normalize();
            QVector2D entryDir(entry.y(), -entry.x());

            m_pool->addAgent(GeneralBuilder::buildSingleAgent(configData, getPointOnLine(i.getPos(), i.getEnd()), entryDir));

            i.resetTimeFromLastGenerate();
        }
        else
        {
            i.setTimeFromLastGenerate(i.getTimeFromLastGenerate() + m_time);
        }
    }
}

std::vector<QVector2D> Calculator::update(double delta)
{
    m_time = delta / 1000;
    std::vector<QVector2D> moveRecord;

    m_iterations++;

    entryProcess();

    for (auto i = m_pool->getAgents().begin(); i != m_pool->getAgents().end();)
    {
        calcForce(*i);
        move(*i);

        moveRecord.push_back(i->getCenter());

        if (isInExit(*i))
        {
            if (m_iscollectStat)
                emit removeAgentSignal(*i);

            i =  m_pool->removeAgent(*i);
        }
        else
            i++;
    }

    return moveRecord;
}

QVector<int> Calculator::buildAStarMatrix(int & height, int & width)
{
    QVector<int> res;
    height  = m_sceneSize.x()/gridStep;
    width = m_sceneSize.y()/gridStep;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; i < height; i++)
        {
            res.push_back(isInObstacle(gridStep*i + gridStep/2, gridStep*j + gridStep/2));
        }
    }
}

int Calculator::isInObstacle(double x, double y)
{
    for(auto i : m_pool->getObstacles())
    {
        auto points =  i.getAbsolutePoints();
        bool result = false;
        int j = points.size() - 1;
        for (int i = 0; i < points.size(); i++) {
            if ( (points[i].x() < y && points[j].y() >= y || points[j].y() < y && points[i].y() >= y) &&
                 (points[i].x() + (y - points[i].y()) / (points[j].y() - points[i].y()) * (points[j].x() - points[i].x()) < x) )
                result = !result;
            j = i;
        }
        if(result)
            return true;
    }
    return false;
}

