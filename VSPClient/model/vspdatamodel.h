#pragma once

#include <QAbstractItemModel>

class VSPDataModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    typedef enum {
        PortItem,
        PortLink,
    } TDataType;
    Q_ENUM(TDataType)

    typedef struct {
        quint8 id;
        QString name;
    } TPortItem;

    typedef struct {
        quint8 id;
        QString name;
        TPortItem source;
        TPortItem target;
    } TPortLink;

    typedef struct {
        TDataType type;
        TPortItem port;
        TPortLink link;
    } TDataRecord;

    explicit VSPDataModel(QObject* parent = nullptr);

    // Basic functionality:
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    virtual void resetModel();
    virtual void append(const TPortItem& port);
    virtual void append(const TPortLink& link);
    QVariant at(int index) const;

protected:
    virtual TDataType dataType() const = 0;

private:
    QList<TDataRecord> m_records;
};
Q_DECLARE_METATYPE(VSPDataModel::TPortItem)
Q_DECLARE_METATYPE(VSPDataModel::TPortLink)
Q_DECLARE_METATYPE(VSPDataModel::TDataRecord)
Q_DECLARE_METATYPE(VSPDataModel::TDataType)

class VSPPortListModel: public VSPDataModel
{
    Q_OBJECT

public:
    VSPPortListModel(QObject* parent = nullptr)
        : VSPDataModel(parent)
    {
    }

    TDataType dataType() const
    {
        return TDataType::PortItem;
    }
};

class VSPLinkListModel: public VSPDataModel
{
    Q_OBJECT

public:
    VSPLinkListModel(QObject* parent = nullptr)
        : VSPDataModel(parent)
    {
    }

    TDataType dataType() const
    {
        return TDataType::PortLink;
    }
};
