#include "tokenlistmodel.h"
#include "tokenmanager.h"
#include "serviceiconprovider.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TokenListModel::TokenListModel(TokenManager *manager, ServiceIconProvider *icons, QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(manager)
    , m_iconProvider(icons)
    , m_timer(new QTimer(this))
{
    connect(m_manager, &TokenManager::tokensChanged, this, &TokenListModel::onTokensChanged);
    connect(m_manager, &TokenManager::tokenUpdated, this, [this](int realIndex) {
        int viewIndex = m_filteredIndices.indexOf(realIndex);
        if (viewIndex >= 0) {
            QModelIndex idx = this->index(viewIndex);
            emit dataChanged(idx, idx);
        }
    });

    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &TokenListModel::refreshTokens);

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int msToNextSecond = 1000 - (now % 1000);
    QTimer::singleShot(msToNextSecond, this, [this]() {
        refreshTokens();
        m_timer->start(1000);
    });

    m_cache.clear();
    rebuildFilter();
}

QString TokenListModel::filterText() const
{
    return m_filterText;
}

void TokenListModel::setFilterText(const QString &text)
{
    if (m_filterText == text) return;
    m_filterText = text;
    emit filterTextChanged();
    beginResetModel();
    rebuildFilter();
    endResetModel();
    emit countChanged();
}

void TokenListModel::rebuildFilter()
{
    m_filteredIndices.clear();
    for (int i = 0; i < m_manager->count(); i++) {
        const TokenInfo &t = m_manager->tokens().at(i);
        if (m_filterText.isEmpty()) {
            m_filteredIndices.append(i);
        } else {
            QString filter = m_filterText.toLower();
            if (t.name.toLower().contains(filter) ||
                t.issuer.toLower().contains(filter) ||
                t.account.toLower().contains(filter)) {
                m_filteredIndices.append(i);
            }
        }
    }
}

int TokenListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_filteredIndices.size();
}

int TokenListModel::realIndex(int viewIndex) const
{
    if (viewIndex < 0 || viewIndex >= m_filteredIndices.size())
        return -1;
    return m_filteredIndices.at(viewIndex);
}

QVariant TokenListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    int ri = realIndex(index.row());
    if (ri < 0) return {};

    const TokenInfo &token = m_manager->tokens().at(ri);

    switch (role) {
    case NameRole:
        return token.name;
    case IssuerRole:
        return token.issuer.isEmpty() ? token.name : token.issuer;
    case AccountRole:
        return token.account;
    case CodeRole: {
        QString code;
        if (token.tokenType == "HOTP")
            code = QString::number(token.hotpCounter).rightJustified(token.digits, '0');
        else {
            TotpResult result = TotpGenerator::generateTotp(token.secret, token.digits,
                                                              token.period, token.algorithm);
            m_cache[ri] = result;
            code = result.code;
        }
        for (int i = code.length() == 8 ? 4 : 3; i < code.length(); i += (code.length() == 8 ? 5 : 4))
            code.insert(i, ' ');
        return code;
    }
    case NextCodeRole: {
        QString code;
        if (token.tokenType == "HOTP")
            code = QString::number(token.hotpCounter + 1).rightJustified(token.digits, '0');
        else {
            TotpResult result = TotpGenerator::generateTotp(token.secret, token.digits,
                                                              token.period, token.algorithm,
                                                              QDateTime::currentSecsSinceEpoch() * 1000
                                                              + token.period * 1000);
            code = result.code;
        }
        for (int i = code.length() == 8 ? 4 : 3; i < code.length(); i += (code.length() == 8 ? 5 : 4))
            code.insert(i, ' ');
        return code;
    }
    case ProgressRole: {
        if (token.tokenType == "HOTP")
            return 1.0;

        auto ts = QDateTime::currentSecsSinceEpoch();
        return (double)(token.period - (ts % token.period)) / token.period;
    }
    case DigitsRole:
        return token.digits;
    case PeriodRole:
        return token.period;
    case TokenTypeRole:
        return token.tokenType;
    case BadgeColorRole:
        return token.badgeColor;
    case IconPathRole: {
        if (m_iconProvider && !token.iconCollectionId.isEmpty())
            return m_iconProvider->iconPathForCollection(token.iconCollectionId);
        return QString();
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> TokenListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {IssuerRole, "issuer"},
        {AccountRole, "account"},
        {CodeRole, "code"},
        {NextCodeRole, "nextCode"},
        {ProgressRole, "progress"},
        {DigitsRole, "digits"},
        {PeriodRole, "period"},
        {TokenTypeRole, "tokenType"},
        {BadgeColorRole, "badgeColor"},
        {IconPathRole, "iconPath"}
    };
}

int TokenListModel::count() const
{
    return m_filteredIndices.size();
}

QVariantMap TokenListModel::get(int index) const
{
    int ri = realIndex(index);
    if (ri < 0) return {};
    return m_manager->getToken(ri);
}

void TokenListModel::copyToClipboard(int index)
{
    int ri = realIndex(index);
    if (ri < 0) return;

    const TokenInfo &token = m_manager->tokens().at(ri);
    QString code;
    if (token.tokenType == "HOTP") {
        code = QString::number(token.hotpCounter).rightJustified(token.digits, '0');
    } else {
        code = TotpGenerator::generateTotp(token.secret, token.digits,
                                            token.period, token.algorithm).code;
    }
    QGuiApplication::clipboard()->setText(code);
}

void TokenListModel::copyTokenJsonToClipboard(int index)
{
    int ri = realIndex(index);
    if (ri < 0) return;

    const TokenInfo &token = m_manager->tokens().at(ri);
    QJsonObject obj = token.toJson();
    QJsonDocument doc(obj);
    QGuiApplication::clipboard()->setText(doc.toJson(QJsonDocument::Indented));
}

void TokenListModel::remove(int index)
{
    int ri = realIndex(index);
    if (ri < 0) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_manager->removeToken(ri);
    rebuildFilter();
    endRemoveRows();
    emit countChanged();
}

void TokenListModel::incrementHotp(int index)
{
    int ri = realIndex(index);
    if (ri < 0) return;

    QVariantMap data;
    data["hotpCounter"] = m_manager->tokens()[ri].hotpCounter + 1;
    m_manager->updateToken(ri, data);
    QModelIndex idx = this->index(index);
    emit dataChanged(idx, idx);
}

bool TokenListModel::modelUpdateToken(int viewIndex, const QVariantMap &data)
{
    int ri = realIndex(viewIndex);
    if (ri < 0) return false;
    return m_manager->updateToken(ri, data);
}

void TokenListModel::refreshTokens()
{
    if (rowCount() == 0) return;
    m_cache.clear();
    emit dataChanged(index(0), index(rowCount() - 1));
}

void TokenListModel::onTokensChanged()
{
    m_cache.clear();
    beginResetModel();
    rebuildFilter();
    endResetModel();
    emit countChanged();
}
