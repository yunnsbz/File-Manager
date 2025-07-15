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
#include <QPixmap>
#include <QSplitter>

#include <FM_Macros.hpp>

FM_BEGIN_NAMESPACE

class MainWindow;
class TableManager;
class TreeManager;
class FileModelOperations;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget* tabWidget, QTreeView* treeView, QTableView* tableView, QObject* parent);

    void moveTabWidget(int index);
    void onTabCloseRequested(int index);
    void onTabClicked(int index);

    // getters:
    auto getPreviousSplitter() const -> QSplitter*;
    // yeni sekmeye geçiş yapıldığı anda önceki sekmeyi gösterir:
    [[nodiscard]] auto _getPreviousLeftTabIndex()           const -> int;

    // yeni sekmeye geçiş tamamlandıktan sonra önceki sekmeyi gösterir:
    [[nodiscard]] auto getPersistentPreviousLeftTabIndex()  const -> int;

    [[nodiscard]] auto getFileModelOp() const -> FileModelOperations*   { return m_fileModelOp; }

    // auto tabCount() const -> int;

    // setters:
    void setTabContent(int tabIndex);
    void setPreviousLeftTabIndex(int value);
    // void addTab(const QString& title);
    // void removeTab(int index);

    void enableNavWidget(bool enable);
    void updateNavButtons(bool BackEnable, bool ForwardEnable, bool UpEnabled);

public slots:
    void onTabMoved(int toIndex, int fromIndex);
    void addNewTab();
    void onTabBarClicked(int tabIndex);
    void onTabClosed(int tabIndex);

signals:
    void tabChanged(int ChangedTabIndex, QString ChangedFilePath);

private:
    void setNavButtonThemes();
    void setCornerNavButtons();
    void setAddTabButton();

private:
    MainWindow* m_mainWindow_;
    QTabWidget* m_tabWidget_;

    FileModelOperations* m_fileModelOp;

    TableManager* m_tableManager;
    TreeManager* m_treeManager;

    QWidget* m_cornerNavButtons{};

    QToolButton* m_backTabButton{};
    QToolButton* m_forwTabButton{};
    QToolButton* m_upTabButton{};

    // last opened tabs (when moving to another tab system should know the last one)
    int m_lastRightTabIndex = 0;

    // _previous geçici olarak veri tutar.
    // yeni tab'e geçtiğin anda geçici önceki sekmenin index'ini tutar. daha sonra bu index'i kalıcıya aktarır.
    // daha sonra current index'e eşitlenir.
    int m_previousLeftTabIndex = 0;
    int m_persistentPreviousLeftTabIndex = 0;
};

FM_END_NAMESPACE
#endif // TABMANAGER_H
