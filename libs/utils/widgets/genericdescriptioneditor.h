/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef GENERICDESCRIPTIONEDITOR_H
#define GENERICDESCRIPTIONEDITOR_H

#include <utils/global_exporter.h>
#include <utils/genericdescription.h>

#include <QWidget>

namespace Utils {
class GenericDescription;
namespace Internal {
namespace Ui {
    class GenericDescriptionEditor;
}  // End namespace Ui
}  // End namespace Internal

class UTILS_EXPORT GenericDescriptionEditor : public QWidget
{
    Q_OBJECT
public:
    explicit GenericDescriptionEditor(QWidget *parent = 0);
    ~GenericDescriptionEditor();

    void setDescription(const Utils::GenericDescription &desc);

public Q_SLOTS:
    bool submit();

private Q_SLOTS:
    void on_langSelector_activated(const QString &text);

private:
    Internal::Ui::GenericDescriptionEditor *ui;
    Utils::GenericDescription m_desc;
};

}  // End namespace Utils

#endif // GENERICDESCRIPTIONEDITOR_H
