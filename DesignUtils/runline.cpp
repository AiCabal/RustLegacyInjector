#include "DesignUtils/runline.h"
RunLine::RunLine(QWidget *parent): QLabel(parent), m_shift(0), m_timerId(0) {
}

void RunLine::timerEvent(QTimerEvent *) {
  const int length = m_string.length();

  if(++m_shift >= length)
    m_shift = 0;

  setText(m_string.right(m_shift) + m_string.left(length - m_shift));
}

void RunLine::setSpeed(const int speed) {
  if (m_timerId)
    killTimer(m_timerId);
  m_timerId = 0;

  if (speed < 0)
    return;

  if (speed)
    m_timerId = startTimer(1000/speed);
}
void RunLine::mousePressEvent(QMouseEvent* event)
{
    Q_EMIT clicked();
    if(!m_timerId)
        setSpeed(10);
    else setSpeed(0);
}
void RunLine::setString(const QString string) {
  m_string = string;
  m_shift = 0;
  setText(m_string);
}
