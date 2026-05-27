#pragma once

#include <QAbstractListModel>
#include <QTimer>
#include "tokendata.h"
#include "totp.h"

class TokenManager;
class ServiceIconProvider;

class TokenListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        IssuerRole,
        AccountRole,
        CodeRole,
        NextCodeRole,
        ProgressRole,
        DigitsRole,
        PeriodRole,
        TokenTypeRole,
        BadgeColorRole,
        IconPathRole
    };

    explicit TokenListModel(TokenManager *manager, ServiceIconProvider *icons, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    QString filterText() const;
    void setFilterText(const QString &text);

    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE void copyToClipboard(int index);
    Q_INVOKABLE void copyTokenJsonToClipboard(int index);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void incrementHotp(int index);
    Q_INVOKABLE bool modelUpdateToken(int viewIndex, const QVariantMap &data);

public slots:
    void refreshTokens();

signals:
    void countChanged();
    void filterTextChanged();

private slots:
    void onTokensChanged();

private:
    int realIndex(int viewIndex) const;
    void rebuildFilter();

    TokenManager *m_manager;
    ServiceIconProvider *m_iconProvider;
    QTimer *m_timer;
    mutable QHash<int, TotpResult> m_cache;
    QString m_filterText;
    QList<int> m_filteredIndices;
};
