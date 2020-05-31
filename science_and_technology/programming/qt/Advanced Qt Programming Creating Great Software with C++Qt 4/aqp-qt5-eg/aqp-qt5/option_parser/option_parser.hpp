#ifndef OPTION_PARSER_HPP
#define OPTION_PARSER_HPP
/*
    Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version. It is provided
    for educational purposes and is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
    the GNU General Public License for more details.
*/

#include <QCoreApplication>
#include <QSet>
#if QT_VERSION >= 0x040500
#include <QSharedPointer>
#else
#include <tr1/memory>
#endif
#include <QString>
#include <QStringList>
#include <QVariant>


namespace AQP {

class PrivateOption
{
public:
    explicit PrivateOption(const QString &shortName=QString(),
                           const QString &longName=QString())
        : m_shortName(shortName), m_longName(longName),
          m_required(false)
        {
            Q_ASSERT(!(longName.isEmpty() && shortName.isEmpty()));
        }
    virtual ~PrivateOption() {}

    bool required() const { return m_required; }
    virtual void setRequired(bool on=true) { m_required = on; }
    QString help() const { return m_help; }
    void setHelp(const QString &help) { m_help = help; }
    bool hasHelp() const { return !m_help.isEmpty(); }

    QString shortName() const { return m_shortName; }
    QString longName() const { return m_longName; }

    bool hasValue() const { return m_value.isValid(); }
    bool hasDefaultValue() const { return m_defaultValue.isValid(); }

    virtual QVariant::Type type() const = 0;

protected:
    QVariant m_defaultValue;
    QVariant m_value;

private:
    const QString m_shortName;
    const QString m_longName;
    bool m_required;
    QString m_help;
};


class IntegerOption : public PrivateOption
{
public:
    explicit IntegerOption(const QString &shortName=QString(),
                           const QString &longName=QString())
        : PrivateOption(shortName, longName) {}

    int defaultValue() const { return m_defaultValue.toInt(); }
    void setDefaultValue(int value) { m_defaultValue = value; }
    bool hasDefaultValue() const { return m_defaultValue.isValid(); }
    int minimum() const { return m_minimum.toInt(); }
    void setMinimum(int minimum) { m_minimum = minimum; }
    bool hasMinimum() const { return m_minimum.isValid(); }
    int maximum() const { return m_maximum.toInt(); }
    void setMaximum(int maximum) { m_maximum = maximum; }
    bool hasMaximum() const { return m_maximum.isValid(); }
    void setRange(int minimum, int maximum)
        { m_minimum = minimum; m_maximum = maximum; }
    QSet<int> acceptableValues() const { return m_acceptableValues; }
    void setAcceptableValues(const QList<int> &acceptableValues)
        { m_acceptableValues = QSet<int>::fromList(acceptableValues); }
    void setAcceptableValues(const QSet<int> &acceptableValues)
        { m_acceptableValues = acceptableValues; }
    int value() const { return m_value.toInt(); }
    void setValue(int value) { m_value = value; }
    bool hasAcceptableValues() const
        { return !m_acceptableValues.isEmpty(); }

    int integer() const { return m_value.toInt(); }
    QVariant::Type type() const { return QVariant::Int; }

private:
    QVariant m_minimum;
    QVariant m_maximum;
    QSet<int> m_acceptableValues;
};


class RealOption : public PrivateOption
{
public:
    explicit RealOption(const QString &shortName=QString(),
                        const QString &longName=QString())
        : PrivateOption(shortName, longName) {}

    double defaultValue() const { return m_defaultValue.toInt(); }
    void setDefaultValue(double value) { m_defaultValue = value; }
    bool hasDefaultValue() const { return m_defaultValue.isValid(); }
    double minimum() const { return m_minimum.toInt(); }
    void setMinimum(double minimum) { m_minimum = minimum; }
    bool hasMinimum() const { return m_minimum.isValid(); }
    double maximum() const { return m_maximum.toInt(); }
    void setMaximum(double maximum) { m_maximum = maximum; }
    bool hasMaximum() const { return m_maximum.isValid(); }
    void setRange(double minimum, double maximum)
        { m_minimum = minimum; m_maximum = maximum; }
    double value() const { return m_value.toDouble(); }
    void setValue(double value) { m_value = value; }

    double real() const { return m_value.toDouble(); }
    QVariant::Type type() const { return QVariant::Double; }

private:
    QVariant m_minimum;
    QVariant m_maximum;
};


class BooleanOption : public PrivateOption
{
public:
    explicit BooleanOption(const QString &shortName=QString(),
                           const QString &longName=QString())
        : PrivateOption(shortName, longName) {}

    bool defaultValue() const { return m_defaultValue.toInt(); }
    void setDefaultValue(bool value) { m_defaultValue = value; }
    bool hasDefaultValue() const { return m_defaultValue.isValid(); }
    bool value() const { return m_value.toBool(); }
    void setValue(bool value) { m_value = value; }

    bool boolean() const { return m_value.toBool(); }
    QVariant::Type type() const { return QVariant::Bool; }

    void setRequired(bool=true) { Q_ASSERT(false); }
};


class StringOption : public PrivateOption
{
public:
    explicit StringOption(const QString &shortName=QString(),
                          const QString &longName=QString())
        : PrivateOption(shortName, longName) {}

    QString defaultValue() const { return m_defaultValue.toString(); }
    void setDefaultValue(const QString &value) { m_defaultValue = value; }
    bool hasDefaultValue() const { return m_defaultValue.isValid(); }
    QSet<QString> acceptableValues() const { return m_acceptableValues; }
    void setAcceptableValues(const QStringList &acceptableValues)
        { m_acceptableValues = QSet<QString>::fromList(acceptableValues); }
    void setAcceptableValues(const QSet<QString> &acceptableValues)
        { m_acceptableValues = acceptableValues; }
    bool hasAcceptableValues() const
        { return !m_acceptableValues.isEmpty(); }
    QString value() const { return m_value.toString(); }
    void setValue(const QString &value) { m_value = value; }

    QString string() const { return m_value.toString(); }
    QVariant::Type type() const { return QVariant::String; }

private:
    QSet<QString> m_acceptableValues;
};


#if QT_VERSION >= 0x040500
typedef QSharedPointer<PrivateOption> OptionPtr;
typedef QSharedPointer<IntegerOption> IntegerOptionPtr;
typedef QSharedPointer<RealOption> RealOptionPtr;
typedef QSharedPointer<BooleanOption> BooleanOptionPtr;
typedef QSharedPointer<StringOption> StringOptionPtr;
#else
typedef std::tr1::shared_ptr<PrivateOption> OptionPtr;
typedef std::tr1::shared_ptr<IntegerOption> IntegerOptionPtr;
typedef std::tr1::shared_ptr<RealOption> RealOptionPtr;
typedef std::tr1::shared_ptr<BooleanOption> BooleanOptionPtr;
typedef std::tr1::shared_ptr<StringOption> StringOptionPtr;
#endif


class OptionParser
{
    Q_DECLARE_TR_FUNCTIONS(OptionParser)

public:
    explicit OptionParser(const QStringList &arguments=QStringList(),
                          const QString &help=QString(),
                          const QString &copyright=QString());

    bool parse();
    int usage(const QString &error=QString());

    int integer(const QString &name) const;
    double real(const QString &name) const;
    bool boolean(const QString &name) const;
    QString string(const QString &name) const;
    bool hasValue(const QString &name) const
        { return m_options[name]->hasValue(); }
    QStringList remainder() const { return m_remainder; }
    OptionPtr option(const QString &name)
        { return m_options[name]; }

    IntegerOptionPtr addIntegerOption(const QString &shortName=QString(),
                                      const QString &longName=QString());
    RealOptionPtr addRealOption(const QString &shortName=QString(),
                                const QString &longName=QString());
    BooleanOptionPtr addBooleanOption(const QString &shortName=QString(),
                                      const QString &longName=QString());
    StringOptionPtr addStringOption(const QString &shortName=QString(),
                                    const QString &longName=QString());

private:
    BooleanOptionPtr addHelpOption();
    void addOption(OptionPtr option);

    QStringList m_arguments;
    QString m_help;
    QString m_copyright;
    QStringList m_remainder;
    QMap<QString, OptionPtr> m_options;
};

} // namespace AQP

#endif // OPTION_PARSER_HPP
