#include "traceimportdialog.h"
#include "ui_traceimportdialog.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QModelIndex>
#include <QColorDialog>

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
    for(unsigned int i=0;i<tableModel->params.size();i++) {
        if(tableModel->params[i].enabled) {
            auto trace = new Trace(tableModel->params[i].name, tableModel->params[i].color);
            trace->fillFromTouchstone(t, i, ui->touchstoneImport->getFilename());
            model.addTrace(trace);
        }
    }
}

int TouchstoneParameterModel::rowCount(const QModelIndex &) const {
    return params.size();
}

int TouchstoneParameterModel::columnCount(const QModelIndex &) const {
    return 4;
}

QVariant TouchstoneParameterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if ((unsigned int) index.row() >= params.size())
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
        if (role == Qt::BackgroundRole || role == Qt::EditRole) {
            if(p.enabled) {
                return p.color;
            } else {
                return (QColor) Qt::gray;
            }
        } else {
            return QVariant();
        }
    } else if (index.column() == 3) {
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
        case 2: return "Color"; break;
        case 3: return "Tracename"; break;
        default: return QVariant(); break;
        }
    } else {
        return QVariant();
    }
}

bool TouchstoneParameterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if((unsigned int) index.row() >= params.size()) {
        return false;
    }
    auto &p = params[index.row()];
    if(role == Qt::CheckStateRole && index.column()==1) {
        if((Qt::CheckState)value.toInt() == Qt::Checked) {
            p.enabled = true;
        } else {
            p.enabled = false;
        }
        dataChanged(this->index(index.row(),2), this->index(index.row(),3));
        return true;
    } else if(role == Qt::EditRole && index.column() == 2) {
        p.color = value.value<QColor>();
        return true;
    } else if(role == Qt::EditRole && index.column() == 3) {
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
        if(params[index.row()].enabled) {
            flags |= Qt::ItemIsEnabled;
        }
    } else if(index.column() == 3) {
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
            p.color = QColor::fromHsl(((i*ports + j) * 50) % 360, 250, 128);
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

void TraceImportDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.column() == 2 && tableModel->params[index.row()].enabled) {
        auto initialColor = tableModel->params[index.row()].color;
        auto newColor = QColorDialog::getColor(initialColor, this, "Select color", QColorDialog::DontUseNativeDialog);
        if(newColor.isValid()) {
            tableModel->setData(index, newColor);
        }
    }
}
