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

#include "option_parser.hpp"
#include <QCoreApplication>
#include <QFileInfo>
#include <QString>
#include <QTextStream>


namespace AQP {

inline bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.toLower().localeAwareCompare(b.toLower()) < 0;
}


OptionParser::OptionParser(const QStringList &arguments,
        const QString &help, const QString &copyright)
    : m_arguments(arguments), m_help(help), m_copyright(copyright)
{
    if (m_arguments.isEmpty())
        m_arguments = QCoreApplication::arguments();
}


IntegerOptionPtr OptionParser::addIntegerOption(const QString &shortName,
                                                const QString &longName)
{
    Q_ASSERT_X(longName != tr("help") &&
               !m_options.contains(longName) &&
               !m_options.contains(shortName), "addIntegerOption",
               "invalid long name (help) or duplicate short or long name");
    IntegerOptionPtr option(new IntegerOption(shortName, longName));
    addOption(option);
    return option;
}


RealOptionPtr OptionParser::addRealOption(const QString &shortName,
                                          const QString &longName)
{
    Q_ASSERT_X(longName != tr("help") &&
               !m_options.contains(longName) &&
               !m_options.contains(shortName), "addRealOption",
               "invalid long name (help) or duplicate short or long name");
    RealOptionPtr option(new RealOption(shortName, longName));
    addOption(option);
    return option;
}


BooleanOptionPtr OptionParser::addBooleanOption(const QString &shortName,
                                                const QString &longName)
{
    Q_ASSERT_X(longName != tr("help") &&
               !m_options.contains(longName) &&
               !m_options.contains(shortName), "addBooleanOption",
               "invalid long name (help) or duplicate short or long name");
    BooleanOptionPtr option(new BooleanOption(shortName, longName));
    addOption(option);
    return option;
}


StringOptionPtr OptionParser::addStringOption(const QString &shortName,
                                              const QString &longName)
{
    Q_ASSERT_X(longName != tr("help") &&
               !m_options.contains(longName) &&
               !m_options.contains(shortName), "addStringOption",
               "invalid long name (help) or duplicate short or long name");
    StringOptionPtr option(new StringOption(shortName, longName));
    addOption(option);
    return option;
}


void OptionParser::addOption(OptionPtr option)
{
    if (!option->longName().isEmpty())
        m_options[option->longName()] = option;
    if (!option->shortName().isEmpty())
        m_options[option->shortName()] = option;
}


BooleanOptionPtr OptionParser::addHelpOption()
{
    QString shortName(m_options.contains(tr("h")) ? "" : "h");
    QString longName(tr("help"));
    BooleanOptionPtr option(new BooleanOption(shortName, longName));
    option->setHelp(tr("show this information and terminate"));
    addOption(option);
    return option;
}


int OptionParser::usage(const QString &error)
{
    QTextStream err(stderr);
    QString program = QFileInfo(m_arguments.at(0)).baseName();
    QString message;
    if (!m_help.isEmpty())
        err << m_help.replace("{program}", program) << "\n";
    else
        err << tr("usage: %1 [options]\n").arg(program);
    err << tr("options:\n");
    (void) addHelpOption();
    QMap<QVariant::Type, int> lengthForType;
    lengthForType[QVariant::String] = QString("STRING").length();
    lengthForType[QVariant::Bool] = 0;
    lengthForType[QVariant::Int] = QString("INTEGER").length();
    lengthForType[QVariant::Double] = QString("REAL").length();
    int longest = 0;
    int anyRequired = 0;
    QMapIterator<QString, OptionPtr> i(m_options);
    QMap<QString, OptionPtr> options;
    while (i.hasNext()) {
        i.next();
        OptionPtr option = i.value();
        const QString &name = option->longName();
        int length = name.length() + lengthForType[option->type()];
        if (length > longest)
            longest = length;
        if (i.value()->required())
            anyRequired = 1;
        const QString longName = (option->longName().isEmpty()
                ? option->shortName() : option->longName()).toLower();
        const QString shortName = (option->shortName().isEmpty()
                ? option->longName() : option->shortName()).toLower();
        const QString key = QString("%1:%2").arg(longName).arg(shortName);
        options[key] = option;
    }
    longest += anyRequired;
    bool required = false;
    const int Width = QString(" -x  --= ").length() + longest;
    QMapIterator<QString, OptionPtr> j(options);
    while (j.hasNext()) {
        j.next();
        OptionPtr option = j.value();
        QString line;
        if (option->shortName().isEmpty())
            line += "   ";
        else
            line += " -" + option->shortName();
        if (!option->longName().isEmpty())
            line += "  --" + option->longName();
        bool close = false;
        if (option->type() == QVariant::Bool &&
            option->longName() != tr("help")) {
            QString offset(Width - line.length(), ' ');
            line += QString("%1(default off)").arg(offset);
            close = true;
        }
        else if (option->type() == QVariant::Int) {
            IntegerOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<IntegerOption*>(option.data());
#else
                dynamic_cast<IntegerOption*>(option.get());
#endif
            Q_ASSERT(opt);
            line += "=INTEGER";
            if (opt->required()) {
                line += "*";
                required = true;
            }
            else if (anyRequired)
                line += " ";
            QString offset(Width - line.length(), ' ');
            if (opt->hasMinimum() && opt->hasMaximum()) {
                line += QString("%1(%2-%3").arg(offset)
                        .arg(opt->minimum()).arg(opt->maximum());
                close = true;
            }
            else if (opt->hasMinimum()) {
                line += tr("%1(min. %2").arg(offset).arg(opt->minimum());
                close = true;
            }
            else if (opt->hasMaximum()) {
                line += tr("%1(max. %2").arg(offset).arg(opt->maximum());
                close = true;
            }
            else if (opt->hasAcceptableValues()) {
                line += offset + "(";
                QList<int> list = QList<int>::fromSet(
                        opt->acceptableValues());
                qSort(list);
                int i = 0;
                for (; i < list.count() - 1; ++i)
                    line += QString::number(list.at(i)) + ", ";
                line += QString::number(list.at(i));
                close = true;
            }
            if (opt->hasDefaultValue()) {
                if (close)
                    line += tr("; default %1)").arg(opt->defaultValue());
                else
                    line += tr(" (default %1)").arg(opt->defaultValue());
            }
            else if (close)
                line += ")";
        }
        if (option->type() == QVariant::Double) {
            RealOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<RealOption*>(option.data());
#else
                dynamic_cast<RealOption*>(option.get());
#endif
            Q_ASSERT(opt);
            line += "=REAL";
            if (opt->required()) {
                line += "*";
                required = true;
            }
            else if (anyRequired)
                line += " ";
            QString offset(Width - line.length(), ' ');
            if (opt->hasMinimum() && opt->hasMaximum()) {
                line += QString("%1(%2-%3").arg(offset)
                        .arg(opt->minimum()).arg(opt->maximum());
                close = true;
            }
            else if (opt->hasMinimum()) {
                line += tr("%1(min. %2").arg(offset)
                        .arg(opt->minimum());
                close = true;
            }
            else if (opt->hasMaximum()) {
                line += tr("%1(max. %2").arg(offset).arg(opt->maximum());
                close = true;
            }
            if (opt->hasDefaultValue()) {
                if (close)
                    line += tr("; default %1)").arg(opt->defaultValue());
                else
                    line += tr(" (default %1)").arg(opt->defaultValue());
            }
            else if (close)
                line += ")";
        }
        else if (option->type() == QVariant::String) {
            StringOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<StringOption*>(option.data());
#else
                dynamic_cast<StringOption*>(option.get());
#endif
            Q_ASSERT(opt);
            line += "=STRING";
            if (opt->required()) {
                line += "*";
                required = true;
            }
            else if (anyRequired)
                line += " ";
            if (opt->hasAcceptableValues()) {
                line += QString(Width - line.length(), ' ') + "(";
                QStringList list = QStringList::fromSet(
                        opt->acceptableValues());
                qSort(list.begin(), list.end(), caseInsensitiveLessThan);
                int i = 0;
                for (; i < list.count() - 1; ++i)
                    line += list.at(i) + ", ";
                line += list.at(i);
                close = true;
            }
            if (opt->hasDefaultValue()) {
                if (close)
                    line += "; ";
                else {
                    line += "(";
                    close = true;
                }
                line += QString("default %1").arg(opt->defaultValue());
            }
            if (close)
                line += ")";
        }
        err << line;
        if (option->hasHelp()) {
            if (close)
                err << " ";
            else
                err << QString(Width - line.length(), ' ');
            err << option->help();
        }
        err << "\n";
    }
    if (required)
        err << tr("(* required)\n");
    if (!m_copyright.isEmpty())
        err << m_copyright << "\n";
    if (!error.isEmpty())
        err << tr("\nERROR: ") << error << "\n";
    return 2;
}


bool OptionParser::parse()
{
    int index = 1;
    for (; index < m_arguments.count(); ++index) {
        QString name = m_arguments.at(index);
        QString arg(name);
        if (!arg.startsWith("-")) // First of non-option args
            break;
        if (arg == "--") { // Skip over to first of non-option args
            ++index;
            break;
        }
        QString value;
        int i = arg.indexOf("=");
        if (i > -1) {
            value = arg.mid(i + 1);
            arg = arg.left(i);
        }
        else if (arg.startsWith("-") && arg.length() > 2 &&
                 arg.at(1) != '-') {
            value = arg.mid(2);
            arg = arg.left(2);
        }
        name = arg;
        if (arg.startsWith("--"))
            arg = arg.mid(2);
        else
            arg = arg.mid(1);
        if (arg == "help" || (!m_options.contains("h") && arg == "h"))
            return !usage();
        if (!m_options.contains(arg))
            return !usage(tr("unrecognized option %1")
                          .arg(name));
        OptionPtr option = m_options[arg];
        if (option->type() == QVariant::Bool) {
            BooleanOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<BooleanOption*>(option.data());
#else
                dynamic_cast<BooleanOption*>(option.get());
#endif
            Q_ASSERT(opt);
            if (!value.isEmpty())
                return !usage(tr("unwanted value for %1").arg(name));
            opt->setValue(true);
            continue;
        }
        if (value.isEmpty()) {
            if (index + 1 >= m_arguments.count())
                return !usage(tr("missing value for %1").arg(name));
            value = m_arguments.at(++index);
        }
        if (option->type() == QVariant::Double) {
            RealOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<RealOption*>(option.data());
#else
                dynamic_cast<RealOption*>(option.get());
#endif
            Q_ASSERT(opt);
            bool ok;
            double v = value.toDouble(&ok);
            if (!ok)
                return !usage(tr("invalid value for %1").arg(name));
            if (opt->hasMinimum()) {
                if (v < opt->minimum())
                    return !usage(tr("too small a value for %1")
                                  .arg(name));
            }
            if (opt->hasMaximum()) {
                if (v > opt->maximum())
                    return !usage(tr("too big a value for %1").arg(name));
            }
            opt->setValue(v);
        }
        else if (option->type() == QVariant::Int) {
            IntegerOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<IntegerOption*>(option.data());
#else
                dynamic_cast<IntegerOption*>(option.get());
#endif
            Q_ASSERT(opt);
            bool ok;
            int v = value.toInt(&ok);
            if (!ok)
                return !usage(tr("invalid value for %1").arg(name));
            if (opt->hasMinimum()) {
                if (v < opt->minimum())
                    return !usage(tr("too small a value for %1")
                                  .arg(name));
            }
            if (opt->hasMinimum()) {
                if (v > opt->maximum())
                    return !usage(tr("too big a value for %1").arg(name));
            }
            if (opt->hasAcceptableValues() &&
                !opt->acceptableValues().contains(v))
                    return !usage(tr("invalid value for %1").arg(name));
            opt->setValue(v);
        }
        else if (option->type() == QVariant::String) {
            StringOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<StringOption*>(option.data());
#else
                dynamic_cast<StringOption*>(option.get());
#endif
            Q_ASSERT(opt);
            if (opt->hasAcceptableValues() &&
                !opt->acceptableValues().contains(value))
                    return !usage(tr("invalid value for %1").arg(name));
            opt->setValue(value);
        }
    }

    QMapIterator<QString, OptionPtr> i(m_options);
    while (i.hasNext()) {
        i.next();
        OptionPtr option = i.value();
        if (!option->hasValue()) {
            if (option->required())
                return !usage(tr("missing mandatory option %1")
                        .arg(option->longName().isEmpty()
                             ? QString("-%1").arg(option->shortName())
                             : QString("--%1").arg(option->longName())));
            if (!option->hasDefaultValue())
                continue;
            if (option->type() == QVariant::Int) {
                IntegerOption *opt =
#if QT_VERSION >= 0x040500
                    dynamic_cast<IntegerOption*>(option.data());
#else
                    dynamic_cast<IntegerOption*>(option.get());
#endif
                Q_ASSERT(opt);
                opt->setValue(opt->defaultValue());
            }
            else if (option->type() == QVariant::Double) {
                RealOption *opt =
#if QT_VERSION >= 0x040500
                    dynamic_cast<RealOption*>(option.data());
#else
                    dynamic_cast<RealOption*>(option.get());
#endif
                Q_ASSERT(opt);
                opt->setValue(opt->defaultValue());
            }
            else if (option->type() == QVariant::Bool) {
                BooleanOption *opt =
#if QT_VERSION >= 0x040500
                    dynamic_cast<BooleanOption*>(option.data());
#else
                    dynamic_cast<BooleanOption*>(option.get());
#endif
                Q_ASSERT(opt);
                opt->setValue(opt->defaultValue());
            }
            else if (option->type() == QVariant::String) {
                StringOption *opt =
#if QT_VERSION >= 0x040500
                    dynamic_cast<StringOption*>(option.data());
#else
                    dynamic_cast<StringOption*>(option.get());
#endif
                Q_ASSERT(opt);
                opt->setValue(opt->defaultValue());
            }
        }
    }
    m_remainder = m_arguments.mid(index);

    return true;
}


int OptionParser::integer(const QString &name) const
{
    Q_ASSERT(m_options.contains(name));
    OptionPtr option = m_options[name];
    Q_ASSERT(option->type() == QVariant::Int);
    IntegerOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<IntegerOption*>(option.data());
#else
                dynamic_cast<IntegerOption*>(option.get());
#endif
    Q_ASSERT(opt);
    return opt->value();
}


double OptionParser::real(const QString &name) const
{
    Q_ASSERT(m_options.contains(name));
    OptionPtr option = m_options[name];
    Q_ASSERT(option->type() == QVariant::Double);
    RealOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<RealOption*>(option.data());
#else
                dynamic_cast<RealOption*>(option.get());
#endif
    Q_ASSERT(opt);
    return opt->value();
}


bool OptionParser::boolean(const QString &name) const
{
    Q_ASSERT(m_options.contains(name));
    OptionPtr option = m_options[name];
    Q_ASSERT(option->type() == QVariant::Bool);
    BooleanOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<BooleanOption*>(option.data());
#else
                dynamic_cast<BooleanOption*>(option.get());
#endif
    Q_ASSERT(opt);
    return opt->value();
}


QString OptionParser::string(const QString &name) const
{
    Q_ASSERT(m_options.contains(name));
    OptionPtr option = m_options[name];
    Q_ASSERT(option->type() == QVariant::String);
    StringOption *opt =
#if QT_VERSION >= 0x040500
                dynamic_cast<StringOption*>(option.data());
#else
                dynamic_cast<StringOption*>(option.get());
#endif
    Q_ASSERT(opt);
    return opt->value();
}

} // namespace AQP
