#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QApplication>
#include <QWidget>

class EventHandler : public QObject {
    Q_OBJECT

public:
    explicit EventHandler(QObject* parent = nullptr, QWidget* tabLeft = nullptr, QWidget* tabRight = nullptr)
        : QObject(parent), tabWidgetLeft(tabLeft), tabWidgetRight(tabRight)
    {
        qApp->installEventFilter(this);
    }

signals:
    void tabLeftClicked();
    void tabRightClicked();

protected:
    auto eventFilter(QObject* obj, QEvent* event) -> bool override{
        if (event->type() == QEvent::MouseButtonPress) {
            QWidget* w = qobject_cast<QWidget*>(obj);
            if (w == nullptr) return false;

            if (tabWidgetLeft != nullptr && tabWidgetLeft->isAncestorOf(w)) {
                emit tabLeftClicked();
            } else if (tabWidgetRight != nullptr && tabWidgetRight->isAncestorOf(w)) {
                emit tabRightClicked();
            }
        }

        return QObject::eventFilter(obj, event);
    }

private:
    QWidget* tabWidgetLeft = nullptr;
    QWidget* tabWidgetRight = nullptr;
};
#endif // EVENTHANDLER_H
