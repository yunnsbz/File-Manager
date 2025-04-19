#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QMap>
#include <QModelIndex>
#include <QSplitter>
#include <QTableView>
#include <QTreeView>
#include <QToolButton>

class QFileSystemModel;
class MainWindow;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget* tabWidget, bool forRightPane, QObject* parent);

    void moveTabWidget(int index);
    void onTabCloseRequested(int index);
    void onTabClicked(int index);

    // getters:
    auto GetPreviousSplitter();
    // yeni sekmeye geçiş yapıldığı anda önceki sekmeyi gösterir:
    [[nodiscard]] auto _getPreviousLeftTabIndex() const -> int;

    // yeni sekmeye geçiş tamamlandıktan sonra önceki sekmeyi gösterir:
    [[nodiscard]] auto getPersistentPreviousLeftTabIndex() const -> int;

    // auto tabCount() const -> int;

    // setters:
    void setPreviousLeftTabIndex(int value);
    // void addTab(const QString& title);
    // void removeTab(int index);

    void EnableNavWidget(bool enable);
    void updateNavButtons(bool BackEnable, bool ForwardEnable, bool UpEnabled);


public slots:
    void onTabMoved(int toIndex, int fromIndex);
    void addNewTab();


protected:


private:
    void Setup_();


private:
    MainWindow* mainWindow;
    QTabWidget* tabWidget;

    QWidget* cornerNavButtons;

    QToolButton* backTabButton;
    QToolButton* forwTabButton;
    QToolButton* upTabButton;

    bool forRightPane_;

    // last opened tabs (when moving to another tab system should know the last one)
    int lastRightTabIndex = 0;

    // _previous geçici olarak veri tutar.
    // yeni tab'e geçtiğin anda geçici önceki sekmenin index'ini tutar. daha sonra bu index'i kalıcıya aktarır.
    // daha sonra current index'e eşitlenir.
    int _previousLeftTabIndex = 0;
    int persistentPreviousLeftTabIndex = 0;
};


#endif // TABMANAGER_H
