#ifndef TRACEIMPORTDIALOG_H
#define TRACEIMPORTDIALOG_H

#include <QDialog>
#include "tracemodel.h"

namespace Ui {
class TraceImportDialog;
}

class TouchstoneParameterModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class TraceImportDialog;
public:
    TouchstoneParameterModel(QObject *parent = 0)
        : QAbstractTableModel(parent){};
    ~TouchstoneParameterModel(){};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void fromTouchstone(Touchstone &t, QString name_prefix);
    void clear();
private:
    class Parameter {
    public:
        bool enabled;
        QString trace;
        QString name;
        QColor color;
    };
    std::vector<Parameter> params;
};

class TraceImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TraceImportDialog(TraceModel &model, QWidget *parent = nullptr);
    ~TraceImportDialog();

private slots:
    void on_buttonBox_accepted();

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::TraceImportDialog *ui;
    TraceModel &model;
    TouchstoneParameterModel *tableModel;
};

#endif // TRACEIMPORTDIALOG_H
