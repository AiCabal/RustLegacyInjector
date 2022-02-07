#ifndef RUNLINE_H
#define RUNLINE_H

#include <QLabel>

class RunLine : public QLabel {
  Q_OBJECT
public:
  RunLine(QWidget *parent = 0);
public Q_SLOTS :
  void setString(const QString string);
  void setSpeed(const int speed);
Q_SIGNALS :
    void clicked();

protected:
  virtual void timerEvent(QTimerEvent*);
    void mousePressEvent(QMouseEvent* event);

  int m_shift, m_timerId;
  QString m_string;
};

#endif // RUNLINE_H
