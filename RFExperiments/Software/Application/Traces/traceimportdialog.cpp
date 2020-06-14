#include "traceimportdialog.h"
#include "ui_traceimportdialog.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QModelIndex>

TraceImportDialog::TraceImportDialog(TraceModel &model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TraceImportDialog),
    model(model)
{
    ui->setupUi(this);
    tableModel = new TouchstoneParameterModel();
    ui->tableView->setModel(tableModel);

    auto updateModel = [=](bool ok) {
        if(ok) {
            auto t = ui->touchstoneImport->getTouchstone();
            auto filename = ui->touchstoneImport->getFilename();
            // remove any directory names (keep only the filename itself)
            int lastSlash = qMax(filename.lastIndexOf('/'), filename.lastIndexOf('\\'));
            if(lastSlash != -1) {
                filename.remove(0, lastSlash + 1);
            }
            // remove file type
            filename.truncate(filename.indexOf('.'));
            tableModel->fromTouchstone(t, filename);
        } else {
             tableModel->clear();
        }
     };

    connect(ui->touchstoneImport, &TouchstoneImport::statusChanged, updateModel);
    connect(ui->touchstoneImport, &TouchstoneImport::filenameChanged, [=](QString) {
        updateModel(ui->touchstoneImport->getStatus());
    });
}

TraceImportDialog::~TraceImportDialog()
{
    delete ui;
    delete tableModel;
}

void TraceImportDialog::on_buttonBox_accepted()
{
    auto t = ui->touchstoneImport->getTouchstone();
    for(int i=0;i<tableModel->params.size();i++) {
        if(tableModel->params[i].enabled) {
            int port1 = i / t.ports();
            int port2 = i % t.ports();
            auto t_copy = t;
            if(port1 == port2) {
                t_copy.reduceTo1Port(port1);
            } else {
                t_copy.reduceTo2Port(port2, port1);
            }
            auto trace = new Trace(tableModel->params[i].name);
            trace->fillFromTouchstone(t_copy);
            model.addTrace(trace);
        }
    }
}

int TouchstoneParameterModel::rowCount(const QModelIndex &parent) const {
    return params.size();
}

int TouchstoneParameterModel::columnCount(const QModelIndex &parent) const {
    return 3;
}

QVariant TouchstoneParameterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= params.size())
        return QVariant();
    auto p = params[index.row()];
    if (index.column() == 0) {
        if (role == Qt::DisplayRole) {
            return p.trace;
        } else {
            return QVariant();
        }
    } else if (index.column() == 1) {
        if (role == Qt::CheckStateRole) {
            if(p.enabled) {
                return Qt::Checked;
            } else {
                return Qt::Unchecked;
            }
        } else {
            return QVariant();
        }
    } else if (index.column() == 2) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return p.name;
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant TouchstoneParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0: return "Parameter"; break;
        case 1: return "Import"; break;
        case 2: return "Tracename"; break;
        default: return QVariant(); break;
        }
    } else {
        return QVariant();
    }
}

bool TouchstoneParameterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() >= params.size()) {
        return false;
    }
    auto &p = params[index.row()];
    if(role == Qt::CheckStateRole && index.column()==1) {
        if((Qt::CheckState)value.toInt() == Qt::Checked) {
            p.enabled = true;
        } else {
            p.enabled = false;
        }
        dataChanged(this->index(index.row(),2), this->index(index.row(),2));
        return true;
    } else if(role == Qt::EditRole && index.column() == 2) {
        p.name = value.toString();
        return true;
    }
    return false;
}

Qt::ItemFlags TouchstoneParameterModel::flags(const QModelIndex &index) const
{
    int flags = Qt::NoItemFlags;
    if(index.column() == 0) {
        flags |= Qt::ItemIsEnabled;
    } else if(index.column() == 1) {
        flags |= Qt::ItemIsUserCheckable;
        flags |= Qt::ItemIsEnabled;
    } else if(index.column() == 2) {
        flags |= Qt::ItemIsEditable;
        if(params[index.row()].enabled) {
            flags |= Qt::ItemIsEnabled;
        }
    }
    return (Qt::ItemFlags) flags;
}

void TouchstoneParameterModel::fromTouchstone(Touchstone &t, QString name_prefix) {
    clear();
    int ports = t.ports();
    beginInsertRows(QModelIndex(), 0, ports * ports - 1);
    for(int i=0;i<ports;i++) {
        for(int j=0;j<ports;j++) {
            Parameter p;
            p.enabled = true;
            p.trace = "S" + QString::number(i+1) + QString::number(j+1);
            p.name = name_prefix + "_" + p.trace;
            params.push_back(p);
        }
    }
    endInsertRows();
}

void TouchstoneParameterModel::clear() {
    beginRemoveRows(QModelIndex(), 0, params.size() - 1);
    params.clear();
    endRemoveRows();
}
