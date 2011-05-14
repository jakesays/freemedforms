#include <QPainter>

#include "common.h"
#include "month_view.h"

using namespace Calendar;

QSize MonthHeader::sizeHint() const {
	return QSize(0, 20);
}

void MonthHeader::paintEvent(QPaintEvent *) {
	// fill all in light blue
	QPainter painter(this);
	painter.fillRect(rect(), QColor(220, 220, 255));

	// bottom line
	QPen pen = painter.pen();
	pen.setColor(QColor(200, 200, 255));
	painter.setPen(pen);
	painter.drawLine(0, rect().bottom(), rect().right(), rect().bottom());

	// text
	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);

	int containWidth = rect().width();
	for (int i = 0; i < 7; ++i) {
		QRect r(QPoint((i * containWidth) / 7, 0), QPoint(((i + 1) * containWidth) / 7 - 1 + 2, rect().height())); // +2 is a vertical correction to not be stucked to the top line
		painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, QDate::shortDayName(i + 1));
	}
}

/////////////////////////////////////////////////////////////

MonthView::MonthView(QWidget *parent) : View(parent) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(this, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
}

int MonthView::topHeaderHeight() const {
	return 0;
}

int MonthView::leftHeaderWidth() const {
	return 0;
}

ViewHeader *MonthView::createHeaderWidget(QWidget *parent) {
	MonthHeader *widget = new MonthHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}

void MonthView::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);

	int horiAmount = visibleRect.width() - 6; // total width without lines width (7 days => 6 lines)
	int vertiAmount = visibleRect.height() - m_weekCount + 1; // total height without lines height

	// draw current day?
	QDate now = QDate::currentDate();
	if (now >= m_monthBoundingDays.first && now <= m_monthBoundingDays.second)
		painter->fillRect(getDayRect(now), QColor(255, 255, 200));

	// vertical lines
	for (int i = 1; i < 7; ++i)
		painter->drawLine((i * horiAmount) / 7 + i - 1, 0,
						  (i * horiAmount) / 7 + i - 1, visibleRect.height());

	// horizontal lines
	for (int i = 1; i < m_weekCount; ++i)
		painter->drawLine(0, (i * vertiAmount) / m_weekCount + i - 1,
						  visibleRect.width(), (i * vertiAmount) / m_weekCount + i - 1);

	// day texts
	for (QDate day = m_monthBoundingDays.first; day <= m_monthBoundingDays.second; day = day.addDays(1)) {
		QRect dayRect = getDayRect(day);

		QString text;
		if (day.day() == 1)
			text = day.toString(tr("d MMM"));
		else
			text = day.toString(tr("d"));

		if (day.month() != m_firstDate.month())
			pen.setColor(QColor(180, 180, 180));
		else
			pen.setColor(QColor(100, 100, 100));
		painter->setPen(pen);

		painter->drawText(dayRect.adjusted(0, 2, -2, 0), Qt::AlignRight | Qt::AlignTop, text); // correction to not be stucked to the top/right lines
	}
}

/*void MonthView::paintEvents(QPainter &painter, const QDate &day, const QRect &dayRect) {
	QList<CalendarItem> items = model()->getItemsBetween(day, day);
	if (!items.count())
		return;

	qSort(items.begin(), items.end(), calendarItemLessThan);

	int itemHeight = QFontMetrics(painter.font()).height();
	int visibleItemsCount = dayRect.height() / itemHeight - 1;
	int top = 0;

	foreach (const CalendarItem &item, items) {
		if (visibleItemsCount < 0)
			break;

		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, QString("%1 %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title()));

		top += itemHeight;
		visibleItemsCount--;
	}
	}*/

void MonthView::resetItemWidgets() {
	// re-create all widgets
	deleteAllWidgets();

	
}

QRect MonthView::getDayRect(const QDate &day) const {
	int i = day.dayOfWeek() - 1;
	int j = m_monthBoundingDays.first.daysTo(day) / 7;

	int horiAmount = rect().width() - 6; // total width without lines width
	int vertiAmount = rect().height() - m_weekCount + 1; // total height without lines height
	return QRect((i * horiAmount) / 7 + i, (j * vertiAmount) / m_weekCount + j,
				 ((i + 1) * horiAmount) / 7 - (i * horiAmount) / 7,
				 ((j + 1) * vertiAmount) / m_weekCount - (j * vertiAmount) / m_weekCount);
}

void MonthView::firstDateChanged() {
	// refresh some internal variables

	// compute week counts
	m_weekCount = 0;
	m_monthBoundingDays = Calendar::getBoundingMonthDaysInterval(m_firstDate);
	m_weekCount = (m_monthBoundingDays.first.daysTo(m_monthBoundingDays.second) + 1) / 7;
}
