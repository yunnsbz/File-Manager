#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include "IFileOperation.h"

#include <QString>
#include <QFileInfo>
#include <QObject>

class FileOperationManager : public QObject {
    Q_OBJECT

public:
    explicit FileOperationManager(QObject *parent = nullptr);

    // undo last operation queue
    void undoLast();

    // seçimlenleri (birden fazla olabilir) taşıma işlemi yapar
    void MoveOperation(QList<QString> src,QString dst);

    // copyalananları yapıştırır:
    void PasteOperation(QString dst);

    // tek bir dosyanın ismini değiştirmek için kullanılır.
    void RenameOperationBasic(QString src,QString name);

    // hedef yol içinde boş klasör oluşturur.
    void CreateNewFolder(QString dst);

    // seçilenleri kesme işlemi için kaydeder. sonrasında PasteOperation çağırılmalıdır
    void addToCut(QString src);

    // sadece geçici olarak kopyalanma adreslerini tutar. kopyalama işlemini bitirmez. sonrasında PasteOperation çağırılmalıdır.
    void addToCopy(QString src);

private:
    QList<QVariantMap> operationHistory;
    QMap<QString, IFileOperation*> OperationMap;
    QVariantMap currentOperation;


private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};

#endif // FILEOPERATIONMANAGER_H
