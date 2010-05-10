/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "baseformwidgets.h"

#include <formmanagerplugin/iformitem.h>
#include <utils/global.h>

#include <QStringList>
#include <QMessageBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QPushButton>

using namespace BaseWidgets;
//using namespace BaseWidgets::Internal;

namespace {
    enum TypeEnum {
        Type_Undefined = 0,
        Type_Form,
        Type_Radio,
        Type_Check,
        Type_Combo,
        Type_MultiCheck,
        Type_UniqueList,
        Type_MultiList,
        Type_Spin,
        Type_ShortText,
        Type_LongText,
        Type_HelpText,
        Type_File,
        Type_Group,
        Type_Date,
        Type_Button,
        Type_MaxType
    };

    static const QStringList widgetsName =
            QStringList() << "undef" << "form" << "radio" << "check" << "combo"
            << "multicheck" << "uniquelist" << "multilist" << "spin"
            << "shorttext" << "longtext" << "helptext" << "file" << "group" << "date" << "button";

    const char * const  EXTRAS_KEY              = "option";
    const char * const  EXTRAS_KEY_COLUMN       = "column";
    const char * const  EXTRAS_COMPACT_VIEW     = "compact";
    const char * const  EXTRAS_GROUP_CHECKABLE  = "checkable";
    const char * const  EXTRAS_GROUP_CHECKED    = "checked";
    const char * const  EXTRAS_ALIGN_VERTICAL   = "vertical";
    const char * const  EXTRAS_ALIGN_HORIZONTAL = "horizontal";

    const char * const  DATE_EXTRAS_KEY         = "dateformat";

}

inline static int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1)
{
    if (!item->extraDatas().value(::EXTRAS_KEY_COLUMN).isEmpty())
        return item->extraDatas().value(::EXTRAS_KEY_COLUMN).toInt();
    else
        return defaultValue;
}

inline static int isCompactView(Form::FormItem *item, bool defaultValue = false)
{
    if (item->extraDatas().value(::EXTRAS_KEY).contains(::EXTRAS_COMPACT_VIEW))
        return true;
    else
        return defaultValue;
}

inline static int isGroupCheckable(Form::FormItem *item, bool defaultValue = false)
{
    if (item->extraDatas().value(::EXTRAS_KEY).contains(::EXTRAS_GROUP_CHECKABLE))
        return true;
    else
        return defaultValue;
}

inline static int isGroupChecked(Form::FormItem *item, bool defaultValue = false)
{
    if (item->extraDatas().value(::EXTRAS_KEY).contains(::EXTRAS_GROUP_CHECKED))
        return true;
    else
        return defaultValue;
}

inline static int isRadioHorizontalAlign(Form::FormItem *item, bool defaultValue = true)
{
    if (item->extraDatas().value(::EXTRAS_KEY).contains(::EXTRAS_ALIGN_HORIZONTAL))
        return true;
    else
        return defaultValue;
}

inline static QString getDateFormat(Form::FormItem *item, const QString & defaultValue = "dd MM yyyy")
{
    if (!item->extraDatas().value(::DATE_EXTRAS_KEY).isEmpty()) {
        return item->extraDatas().value(::DATE_EXTRAS_KEY);
    }
    return defaultValue;
}


BaseWidgetsFactory::BaseWidgetsFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

BaseWidgetsFactory::~BaseWidgetsFactory()
{}

bool BaseWidgetsFactory::initialize(const QStringList &arguments, QString *errorString)
{
    return true;
}

bool BaseWidgetsFactory::extensionInitialized()
{
    return true;
}

bool BaseWidgetsFactory::isInitialized() const
{
    return true;
}

QStringList BaseWidgetsFactory::providedWidgets() const
{
    return ::widgetsName;
}

bool BaseWidgetsFactory::isContainer( const int idInStringList ) const
{
    return ( idInStringList == ::Type_Form ) ||
            ( idInStringList == ::Type_Group );
}

Form::IFormWidget *BaseWidgetsFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    int id = ::widgetsName.indexOf(name);
    if (id == -1)
        return 0;
    switch (id)
    {
    case ::Type_Form : return new BaseForm(formItem,parent);
    case ::Type_Group : return new BaseGroup(formItem,parent);
    case ::Type_Check : return new BaseCheck(formItem,parent);
    case ::Type_Radio : return new BaseRadio(formItem,parent);
    case ::Type_ShortText : return new BaseSimpleText(formItem,parent,true);
    case ::Type_LongText : return new BaseSimpleText(formItem,parent,false);
    case ::Type_HelpText : return new BaseHelpText(formItem,parent);
    case ::Type_MultiList : return new BaseList(formItem,parent,false);
    case ::Type_UniqueList : return new BaseList(formItem,parent,true);
    case ::Type_Combo : return new BaseCombo(formItem,parent);
    case ::Type_Date : return new BaseDate(formItem,parent);
    case ::Type_Spin : return new BaseSpin(formItem,parent);
    case ::Type_Button : return new BaseButton(formItem,parent);
    default: return 0;
    }
    return 0;
}



//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseForm implementation ---------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \class BaseForm
   \brief Creates a form
   Manages some Form::FormItem::extraDatas() (that is a QHash<QString, QString>) :
   - "col=" ; "numberOfColumns"
*/

BaseForm::BaseForm(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_ContainerLayout( 0 )
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    QWidget *mainWidget = new QWidget;
    mainLayout->addWidget(mainWidget);
    mainLayout->addStretch();

    m_ContainerLayout = new QGridLayout(mainWidget);
    IFormWidget::createLabel(m_FormItem->spec()->label(), Qt::AlignCenter );
    m_Label->setFrameStyle(FormLabelFrame);
    QFont font = m_Label->font();
    font.setBold(true);
    m_Label->setFont(font);
    QSizePolicy sizePolicy = m_Label->sizePolicy();
    sizePolicy.setVerticalPolicy( QSizePolicy::Fixed );
    m_Label->setSizePolicy( sizePolicy );

    // Retrieve the number of columns for the gridlayout (lays in extraDatas() of linked FormItem)
    numberColumns = getNumberOfColumns(m_FormItem);
    if (isCompactView(m_FormItem)) {
        mainLayout->setMargin(0);
        mainLayout->setSpacing(2);
        m_ContainerLayout->setMargin(0);
        m_ContainerLayout->setSpacing(2);
    }
    m_ContainerLayout->addWidget(m_Label, 0, 0,  1, numberColumns);
    i = numberColumns;
    row = 0;
    col = 0;
}

BaseForm::~BaseForm()
{
}

void BaseForm::addWidgetToContainer(IFormWidget * widget)
{
    if (!widget)
        return;
    if (!m_ContainerLayout)
        return;
    // it is not possible to add a form inside a container
    if (widget->formItem()->spec()->pluginName() == ::widgetsName[::Type_Form])
        return;
    col = (i % numberColumns);
    row = (i / numberColumns);
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}

void BaseForm::retranslate()
{
    /** \todo iformitem --> one spec per language ? */
    //     m_Label->setText( m_FormItem->spec()->label() );
}



//--------------------------------------------------------------------------------------------------------
//-------------------------------------- BaseGroup implementation --------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseGroup::BaseGroup(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Group(0), m_ContainerLayout(0)
{
    QVBoxLayout * vblayout = new QVBoxLayout( this );
    m_Group = new QGroupBox( this );
    m_Group->setTitle(m_FormItem->spec()->label());
    vblayout->addWidget( m_Group );
    this->setLayout( vblayout );

    // Retrieve the number of columns for the gridlayout (lays in extraDatas() of linked FormItem)
    numberColumns = getNumberOfColumns(m_FormItem, 2);

    // Create the gridlayout with all the widgets
    m_ContainerLayout = new QGridLayout( m_Group );
    i = 0;
    row = 0;
    col = 0;
    if (isCompactView(m_FormItem)) {
        vblayout->setMargin(0);
        vblayout->setSpacing(2);
        m_ContainerLayout->setMargin(0);
        m_ContainerLayout->setSpacing(2);
    }

    if (isGroupCheckable(m_FormItem, false)) {
        m_Group->setCheckable( true );
        m_Group->setChecked(isGroupChecked(m_FormItem,false));
        //          connect( m_Group, SIGNAL( clicked( bool ) ),
        //                   this,    SLOT  ( updateObject( bool ) ) );
        //          connect( mfo(m_FormItem), SIGNAL( valueChanged() ),
        //                   this,     SLOT  ( updateWidget() ) );
    }
    m_Group->setLayout(m_ContainerLayout);
}

BaseGroup::~BaseGroup()
{
}

void BaseGroup::addWidgetToContainer(IFormWidget * widget)
{
    if (!widget)
        return;
    if (!m_ContainerLayout)
        return;
    // it is not possible to add a form inside a container
    if (widget->formItem()->spec()->pluginName() == ::widgetsName[::Type_Form])
        return;
    col = (i % numberColumns);
    row = (i / numberColumns);
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}


//void BaseGroup::updateObject( bool state )
//{
//     mfo(m_FormItem)->disconnect();
//     mfo(m_FormItem)->selectedValueChangedTo( state );
//     connect( mfo(m_FormItem), SIGNAL( valueChanged() ),
//              this,     SLOT  ( updateWidget() ) );
//}

//void BaseGroup::updateWidget()
//{
//     m_Group->disconnect();
//     m_Group->setChecked( mfo(m_FormItem)->isChecked() );
//     connect( m_Group, SIGNAL( clicked( bool ) ),
//              this,    SLOT  ( updateObject( bool ) ) );
//}

void BaseGroup::retranslate()
{
    //     m_Group->setTitle( mfo(m_FormItem)->label() );
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseCheck ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCheck::BaseCheck(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Check( 0 )
{
    QHBoxLayout * hb = new QHBoxLayout( this );
    // Add Buttons
    m_Check = new QCheckBox(this);
    m_Check->setObjectName( "Checkbox_" + m_FormItem->uuid());
    hb->addWidget(m_Check);
    retranslate();
    // create itemdata
    m_ItemData = new BaseCheckData(formItem);
    m_ItemData->setCheckBox(m_Check);
    formItem->setItemDatas(m_ItemData);
}

BaseCheck::~BaseCheck()
{
}

void BaseCheck::retranslate()
{
    m_Check->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseCheckData::BaseCheckData(Form::FormItem *item) :
        m_FormItem(item), m_Check(0)
{}

BaseCheckData::~BaseCheckData()
{}

void BaseCheckData::setCheckBox(QCheckBox *chk)
{
    m_Check = chk;
}

//    virtual void clear() = 0;

Form::FormItem *BaseCheckData::parentItem() const
{
    return m_FormItem;
}

bool BaseCheckData::isModified() const
{
    return m_IsModified;
}

void BaseCheckData::setData(const QVariant &data, const int role)
{
    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        if (data.canConvert(QVariant::Int))  { // Tristate
            m_Check->setCheckState(Qt::CheckState(data.toInt()));
        }
    }
}

QVariant BaseCheckData::data(const int role) const
{
    return m_Check->checkState();
}

void BaseCheckData::setStorableData(const QVariant &data)
{
}

QVariant BaseCheckData::storableData() const
{
    return QVariant();
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- BaseRadio ----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseRadio::BaseRadio(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent)
{
    // Prepare Widget Layout and label
    //     QBoxLayout * hb = getBoxLayout( Label_OnLeft, mfo(m_FormItem)->label(), this );
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this );

    // Add QLabel
    m_Label->setSizePolicy( QSizePolicy::Preferred , QSizePolicy::Preferred );
    hb->addWidget(m_Label);

    //     if ( !( mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop ) ) {
    //          Qt::Alignment alignment = m_Label->alignment();
    //          alignment &= ~( Qt::AlignVertical_Mask );
    //          alignment |= Qt::AlignVCenter;
    //          m_Label->setAlignment( alignment );
    //     }

    // Add Buttons
    QGroupBox *gb = new QGroupBox( this );
    //     gb->setFlat( true );
    //     QSizePolicy sizePolicy = gb->sizePolicy();
    //     sizePolicy.setHorizontalPolicy( QSizePolicy::Fixed );
    //     gb->setSizePolicy( sizePolicy );
    QBoxLayout *radioLayout = 0;
    if (isRadioHorizontalAlign(m_FormItem)) {
        radioLayout = new QBoxLayout( QBoxLayout::LeftToRight, gb );
    } else {
        radioLayout = new QBoxLayout( QBoxLayout::TopToBottom, gb );
    }
    radioLayout->setContentsMargins( 1, 0, 1, 0 );
    QRadioButton * rb = 0;
    int i = 0;
    foreach ( QString v, m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible) ) {
        rb = new QRadioButton(this);
        rb->setObjectName("Radio");
        rb->setText(v);
        rb->setProperty("id", i);
        i++;
        radioLayout->addWidget(rb);
        m_RadioList.append(rb);
        // Connect radio button enabled/disabled with mfObject
        //          connect( rb ,    SIGNAL( clicked( bool ) ),
        //                   this ,  SLOT  ( updateObject( bool ) ) );
        //          // Connect object value changed signal with radiobutton
        //          connect ( mfo(m_FormItem),   SIGNAL( valueChanged() ),
        //                    this,  SLOT  ( updateWidget() ) );
    }
    hb->addWidget( gb );

    // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();

}

BaseRadio::~BaseRadio()
{ }

//void BaseRadio::updateObject( bool )
//{
//     QRadioButton * but = qobject_cast<QRadioButton *>( sender() );
//     mfo(m_FormItem)->selectedValueChangedTo( but->property( "id" ).toInt() );
//}
//
//void BaseRadio::updateWidget()
//{
//     QList<QRadioButton*> finded = this->findChildren<QRadioButton *>( "Radio" );
//     if ( !finded.count() ) return;
//
//     foreach( QRadioButton* but , finded )
//     {
//          if ( but->property( "id" ).toInt() == *mfo(m_FormItem)->selectedValuesIds().constBegin() )
//               but->setChecked( true );
//          else
//               but->setChecked( false );
//     }
//}

void BaseRadio::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());

    if ( m_RadioList.size() ) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if ( list.count() != m_RadioList.count() ) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        int i = 0;
        foreach ( QRadioButton * button, m_RadioList ) {
            button->setText( list.at( i ) );
            i++;
        }
    }
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------- BaseSimpleText -------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSimpleText::BaseSimpleText(Form::FormItem *formItem, QWidget *parent, bool shortText)
        : Form::IFormWidget(formItem,parent), m_Line(0), m_Text(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add List and manage size
    if (shortText) {
        //          if (!(mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop))
        //          {
        //               Qt::Alignment alignment = m_Label->alignment();
        //               alignment &= ~( Qt::AlignVertical_Mask );
        //               alignment |= Qt::AlignVCenter;
        //               m_Label->setAlignment( alignment );
        //          }

        m_Line = new QLineEdit( this );
        m_Line->setObjectName( "Line_" + m_FormItem->uuid());
        m_Line->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
        //          m_Line->setInputMask( mfo(m_FormItem)->mask() );
        //          m_Line->setCursorPosition(0);
        hb->addWidget(m_Line);
        // Connect list selection changed with mfObject value changed
        //          connect( m_Line,       SIGNAL( textChanged( const QString & ) ),
        //                   this ,      SLOT  ( updateObject( const QString & ) ) );
        //          connect( mfo(m_FormItem),        SIGNAL( valueChanged() ),
        //                   this ,      SLOT  ( updateWidget() ) );

    } else {
        m_Text = new QTextEdit(this);
        m_Text->setObjectName( "Text_" + m_FormItem->uuid());
        m_Text->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
        hb->addWidget(m_Text);
        // Connect list selection changed with mfObject value changed
        //          connect( m_Text,       SIGNAL( textChanged () ),
        //                   this ,      SLOT  ( updateObject() ) );
        //          connect( mfo(m_FormItem),        SIGNAL( valueChanged() ),
        //                   this ,      SLOT  ( updateWidget() ) );
    }

    // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();

}

BaseSimpleText::~BaseSimpleText()
{
}

//void BaseSimpleText::updateObject( const QString & value )
//{
//     mfo(m_FormItem)->disconnect();
//     mfo(m_FormItem)->selectedValueChangedTo( value );
//     connect( mfo(m_FormItem),   SIGNAL( valueChanged() ),
//              this ,      SLOT  ( updateWidget() ) );
//}
//
//void BaseSimpleText::updateObject()
//{
//     if ( !m_Text ) return;
//     mfo(m_FormItem)->disconnect();
//     mfo(m_FormItem)->selectedValueChangedTo( m_Text->toPlainText() );
//     connect( mfo(m_FormItem),   SIGNAL( valueChanged() ),
//              this ,      SLOT  ( updateWidget() ) );
//
//}

//void BaseSimpleText::updateWidget()
//{
//     // Get text object
//     if ( m_Line )
//     {
//          m_Line->disconnect();
//          m_Line->setText( mfo(m_FormItem)->value().toString() );
//          m_Line->repaint();
//          connect( m_Line,  SIGNAL( textChanged( const QString & ) ),
//                   this ,   SLOT  ( updateObject( const QString & ) ) );
//     }
//     else if ( m_Text )
//     {
//          m_Text->disconnect();
//          m_Text->setPlainText( mfo(m_FormItem)->value().toString() );
//          connect( m_Text,  SIGNAL( textChanged () ),
//                   this ,   SLOT  ( updateObject() ) );
//     }
//}

void BaseSimpleText::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseHelpText -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseHelpText::BaseHelpText(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent)
{
    QHBoxLayout * hb = new QHBoxLayout( this );
    // Add QLabel
    createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
    // Setting objectName to hide/show via a simple option button
    m_Label->setObjectName("HelpText_" + m_FormItem->uuid());
    hb->addWidget(m_Label);
}

BaseHelpText::~BaseHelpText()
{
}

void BaseHelpText::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseLists --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseList::BaseList(Form::FormItem *formItem, QWidget *parent, bool uniqueList)
        : Form::IFormWidget(formItem,parent), m_List( 0 )
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);

    // Add List and manage size
    m_List = new QListWidget(this);
    m_List->setObjectName("List_" + m_FormItem->uuid());
    m_List->setUniformItemSizes(true);
    m_List->addItems( m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible) );
    m_List->setSortingEnabled(false);
    m_List->setAlternatingRowColors(true);
    m_List->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);

    if (uniqueList)
        m_List->setSelectionMode(QAbstractItemView::SingleSelection);
    else
        m_List->setSelectionMode(QAbstractItemView::MultiSelection);
    hb->addWidget(m_List);

    //     // Connect list selection changed with mfObject
    //     connect( m_List, SIGNAL( itemSelectionChanged() ),
    //              this ,      SLOT  ( updateObject() ) );
    //     connect( mfo(m_FormItem),        SIGNAL( valueChanged() ),
    //              this,       SLOT  ( updateWidget() ) );
    //
    //     // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();
}

BaseList::~BaseList()
{
}

//void BaseList::updateObject()
//{
//     if ( !m_List ) return;
//
//     QSet<int> values;
//     foreach( QListWidgetItem* item, m_List->selectedItems() )
//     {
//          values << m_List->row( item );
//     }
//     mfo(m_FormItem)->selectedValueChangedTo( values );
//}
//
//void BaseList::updateWidget()
//{
//     if ( !m_List ) return;
//
//     m_List->disconnect();
//
//     m_List->clearSelection();
//     foreach( int idx, mfo(m_FormItem)->selectedValuesIds() )
//     m_List->item( idx )->setSelected( true );
//
//     connect( m_List, SIGNAL( itemSelectionChanged() ),
//              this ,      SLOT  ( updateObject() ) );
//
//}

void BaseList::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
    if (m_List) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_List->count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        int i = 0;
        for ( i = 0; i < m_List->count(); i++ )
            m_List->item(i)->setText(list[i]);
    }
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseCombo --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseCombo::BaseCombo(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Combo( 0 )
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);
    //     if ( !( mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop ) )
    //     {
    //          Qt::Alignment alignment = m_Label->alignment();
    //          alignment &= ~( Qt::AlignVertical_Mask );
    //          alignment |= Qt::AlignVCenter;
    //          m_Label->setAlignment( alignment );
    //     }

    // Add List and manage size
    m_Combo = new QComboBox(this);
    m_Combo->setObjectName("Combo_" + m_FormItem->uuid());
    m_Combo->addItems(m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible));
    hb->addWidget(m_Combo);
    //     if ( mfo(m_FormItem)->options() & mfObjectFundamental::SizePreferred )
    //          m_Combo->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    //     else
    //          m_Combo->setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Fixed );


    //     // Connect list selection changed with mfObject value changed
    //     connect( m_Combo, SIGNAL( activated ( int ) ),
    //              this ,   SLOT  ( updateObject( int ) ) );
    //     connect( mfo(m_FormItem),     SIGNAL( valueChanged() ),
    //              this,    SLOT( updateWidget() ) );
    //
    //     // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();
}

BaseCombo::~BaseCombo()
{
}

//void BaseCombo::updateObject( int id )
//{
//     mfo(m_FormItem)->selectedValueChangedTo( id );
//}
//
//void BaseCombo::updateWidget()
//{
//     if ( !m_Combo ) return;
//     m_Combo->disconnect();
//
//     if ( mfo(m_FormItem)->selectedValuesIds().count() )
//          m_Combo->setCurrentIndex( *mfo(m_FormItem)->selectedValuesIds().constBegin() );
//     else
//          m_Combo->setCurrentIndex( 0 );
//
//     connect( m_Combo,  SIGNAL( activated ( int ) ),
//              this ,    SLOT  ( updateObject( int ) ) );
//
//}

void BaseCombo::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
    if (m_Combo) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Combo->count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        // refresh combo items
        int id = m_Combo->currentIndex();
        m_Combo->clear();
        m_Combo->addItems(list);
        m_Combo->setCurrentIndex(id);
    }
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- BaseDate ---------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseDate::BaseDate(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Date( 0 )
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget( m_Label );
    //     if ( !( mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop ) )
    //     {
    //          Qt::Alignment alignment = m_Label->alignment();
    //          alignment &= ~( Qt::AlignVertical_Mask );
    //          alignment |= Qt::AlignVCenter;
    //          m_Label->setAlignment( alignment );
    //     }

    // Add Date selector and manage date format
    m_Date = new QDateTimeEdit(this);
    m_Date->setObjectName( "Date_" + m_FormItem->uuid());
    m_Date->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    m_Date->setDisplayFormat(getDateFormat(m_FormItem));
    m_Date->setCalendarPopup(true);
    //     m_Date->setDateTime( mfo(m_FormItem)->dateTime() );
    hb->addWidget(m_Date);

    // Initialize mfo and dateedit with mfo options
    //     const QStringList &options = mfo(m_FormItem)->param( mfObject::Param_Options ).toStringList();
    //     if ( options.contains( "now" ) )
    //          m_Date->setDateTime( QDateTime::currentDateTime() );

    //     // Connect list selection changed with mfObject value changed
    //     connect( m_Date,   SIGNAL( dateTimeChanged ( const QDateTime & ) ),
    //              this ,  SLOT  ( updateObject( const QDateTime & ) ) );
    //     connect( m_Date,   SIGNAL( dateChanged ( const QDate & ) ),
    //              this ,  SLOT  ( updateObject( const QDate & ) ) );
    //     connect( mfo(m_FormItem),    SIGNAL( valueChanged() ),
    //              this,   SLOT  ( updateWidget() ) );
    //
    //     // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();
}

BaseDate::~BaseDate()
{
}

//void BaseDate::updateObject( const QDateTime & datetime )
//{
//     mfo(m_FormItem)->selectedValueChangedTo( datetime );
//}
//
//void BaseDate::updateObject( const QDate & date )
//{
//     mfo(m_FormItem)->selectedValueChangedTo( QDateTime( date ) );
//}
//
//void BaseDate::updateWidget()
//{
//     if ( !m_Date ) return;
//
//     m_Date->disconnect();
//
//     m_Date->setDateTime( mfo(m_FormItem)->dateTime() );
//
//     connect( m_Date, SIGNAL( dateTimeChanged ( const QDateTime & ) ),
//              this ,  SLOT  ( updateObject( const QDateTime & ) ) );
//     connect( m_Date, SIGNAL( dateChanged ( const QDate & ) ),
//              this ,  SLOT  ( updateObject( const QDate & ) ) );
//}

void BaseDate::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());

}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseSpin --------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseSpin::BaseSpin(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Spin( 0 )
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
    hb->addWidget(m_Label);
    //     if ( !( mfo(m_FormItem)->options() & mfObjectFundamental::LabelOnTop ) )
    //     {
    //          Qt::Alignment alignment = m_Label->alignment();
    //          alignment &= ~( Qt::AlignVertical_Mask );
    //          alignment |= Qt::AlignVCenter;
    //          m_Label->setAlignment( alignment );
    //     }

    // Add Date selector and manage date format
    m_Spin = new QSpinBox(this);
    m_Spin->setObjectName( "Spin_" + m_FormItem->uuid());
    m_Spin->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    //     m_Spin->setValue( mfo(m_FormItem)->value().toInt() );
    hb->addWidget(m_Spin);

    //     // Connect list selection changed with mfObject value changed
    //     connect( m_Spin,   SIGNAL( valueChanged ( int ) ),
    //              this ,  SLOT  ( updateObject( int ) ) );
    //     connect( mfo(m_FormItem),    SIGNAL( valueChanged() ),
    //              this,   SLOT  ( updateWidget() ) );
    //
    //     // if selected data exists fill the widget with
    //     if ( mfo(m_FormItem)->value() != QVariant() )
    //          updateWidget();
}

BaseSpin::~BaseSpin()
{}

//void BaseSpin::updateObject( int val )
//{
//     mfo(m_FormItem)->disconnect();
//     mfo(m_FormItem)->selectedValueChangedTo( val );
//     connect( mfo(m_FormItem),    SIGNAL( valueChanged() ),
//              this,   SLOT  ( updateWidget() ) );
//
//}
//
//void BaseSpin::updateWidget()
//{
//     if ( !m_Spin ) return;
//
//     m_Spin->disconnect();
//
//     m_Spin->setValue( mfo(m_FormItem)->value().toInt() );
//
//     connect( m_Spin,   SIGNAL( valueChanged ( int ) ),
//              this ,  SLOT  ( updateObject( int ) ) );
//}

void BaseSpin::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ BaseButton ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
BaseButton::BaseButton(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_Button( 0 )
{
    QHBoxLayout * hb = new QHBoxLayout(this);
    hb->addStretch();

    m_Button = new QPushButton( this );
    m_Button->setObjectName("Button_" + m_FormItem->uuid());
    m_Button->setText(m_FormItem->spec()->label() );
    m_Button->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    hb->addWidget(m_Button);
    connect( m_Button, SIGNAL(clicked()) , this , SLOT(buttonClicked()));
}

BaseButton::~BaseButton()
{}

void BaseButton::buttonClicked()
{
    /** \todo run script */
//    m_FormItem->scripts()->runScript();
}

void BaseButton::retranslate()
{
    m_Button->setText(m_FormItem->spec()->label());
}

