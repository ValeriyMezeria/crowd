#ifndef CROWD_PARAMETERS_H
#define CROWD_PARAMETERS_H

#include <QWidget>
#include <QJsonObject>

#include "json_manager.h"

namespace Ui {
class CrowdParameters;
}

class CrowdParameters : public QWidget
{
    Q_OBJECT

public:
    explicit CrowdParameters(const QJsonObject &data, QWidget *parent = 0);
    ~CrowdParameters();

private slots:
    void on_saveParamsButton_clicked();

signals:
    void sendCrowdParamsJson(QJsonObject data);

private:
    Ui::CrowdParameters *ui;
    QJsonObject createJson(QJsonObject data);
    void readJson(const QJsonObject &file);

};

#endif // CROWD_PARAMETERS_H
