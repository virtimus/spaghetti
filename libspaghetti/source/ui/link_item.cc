// MIT License
//
// Copyright (c) 2017-2018 Artur Wyszyński, aljen at hitomi dot pl
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ui/link_item.h"

#include <cmath>

#include <QStyleOptionGraphicsItem>

#include "colors.h"
#include "spaghetti/socket_item.h"
#include "spaghetti/node.h"
//#include "spaghetti/element.h"
#include "spaghetti/logger.h"

namespace spaghetti {

LinkItem::LinkItem(QGraphicsItem *a_parent)
  : QGraphicsPathItem{ a_parent }
{
  setFlags(ItemSendsGeometryChanges | ItemIsFocusable | ItemIsSelectable);
  setZValue(-1);
  setAcceptHoverEvents(true);
}

QRectF LinkItem::boundingRect() const
{
  return m_shape.boundingRect();
}

QPainterPath LinkItem::shape() const
{
  return m_shape;
}

void LinkItem::paint(QPainter *a_painter, QStyleOptionGraphicsItem const *a_option, QWidget *a_widget)
{
  (void)a_option;
  (void)a_widget;

  QColor const signalColor{ (m_isSignalOn ? m_colorSignalOn : m_colorSignalOff) };
  QColor const notActive{ (isSelected() ? get_color(Color::eSelected) : signalColor) };
  QColor const hover{ get_color(Color::eSocketHover) };

  QPen pen{ (m_isHover ? hover : notActive) };
  pen.setStyle((m_to ? Qt::SolidLine : Qt::DashDotLine));
  pen.setWidth(2);

  if (m_valueType != ValueType::eBool) {
    QPen dash = pen;
    QColor hover2 = signalColor;
    hover2.setAlpha(85);
    dash.setColor(hover2);
    dash.setStyle(Qt::DotLine);
    dash.setWidth(6);
    dash.setDashOffset(m_dashOffset);
    a_painter->setPen(dash);
    a_painter->drawPath(m_path);
  }

  a_painter->setPen(pen);
  a_painter->drawPath(m_path);
}

void LinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event)
{
  (void)a_event;

  setHover(true);
}

void LinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event)
{
  (void)a_event;
  setHover(false);
}

void LinkItem::advance(int a_phase)
{
  if (!a_phase) return;

  if (m_valueType != ValueType::eBool) m_dashOffset -= 0.1;

  update();
}

void LinkItem::setFrom(SocketItem *const a_from)
{
  m_from = a_from;

  QPointF const position{ mapFromScene(m_from->scenePos()) };
  setPos(position);

  trackNodes();
}

void LinkItem::setTo(SocketItem *const a_to)
{
  m_to = a_to;

  m_isSnapped = a_to != nullptr;

  trackNodes();
}

void LinkItem::setTo(QPointF const a_to)
{
  m_toPoint = mapFromScene(a_to);

  trackNodes();
}

void LinkItem::setHover(bool const a_hover)
{
  m_isHover = a_hover;
  if (m_from) m_from->setHover(m_isHover);
  if (m_to) m_to->setHover(m_isHover);
}

void LinkItem::setColors(QColor const a_signalOff, QColor const a_signalOn)
{
  m_colorSignalOff = a_signalOff;
  m_colorSignalOn = a_signalOn;
}

void LinkItem::setSignal(bool const a_signal)
{
  m_isSignalOn = a_signal;

  if (m_to) m_to->setSignal(a_signal);
}

/*template<typename ... Args>
void LinkItem::consoleAppendF( const std::string& format, Args ... args )
{
	//spaghetti::log::info(format, args ...);
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    std::string str = std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    m_from->node()->element()->consoleAppend(cstr);
}*/

template<typename ... Args>
void LinkItem::consoleAppendF( const std::string& format, Args ... args )
  {
      char buff[300];
      std::string sformat = format+"\n";
      //sprintf(buff,format.c_str(), args ...);
      //auto formatter = std::make_unique<spdlog::pattern_formatter>();
      //std::stringstream sstr;
      //std::streambuf sbuf;
      //sstr << std::format(format,args ...);

      //outbuf ob;
      //std::streambuf *sb = std::cout.rdbuf(&sbuf);

      // do some work here
      //spaghetti::log::info(format.c_str(), args ...);
      spdlog::level::level_enum lvl =  spdlog::level::info;
      const std::string nme = "xx";
      spdlog::details::log_msg log_msg(&nme, lvl);
      log_msg.raw.write(sformat, args...);
      //spdlog::pattern_time_type pattern_time = spdlog::pattern_time_type::local;
      //auto formatter = std::make_shared<spdlog::pattern_formatter>(format, pattern_time);
      //formatter->format(log_msg);
      //std::ostringstream ss;
      //std::ostream ss;
      //fmt::print(ss,format,args ...);
      //ss << log_msg.raw;
      strcpy(buff, log_msg.raw.c_str());
      m_from->node()->element()->consoleAppend(buff);
      //consoleAppend(buff);
      //consoleAppend(ss.str().c_str());

      // make sure to restore the original so we don't get a crash on close!
      //std::cout.rdbuf(sb);
  }

void LinkItem::trackNodes()
{
  prepareGeometryChange();

  QPointF const linkItemPos{ m_from->scenePos() };
  setPos(linkItemPos);

  QPointF const toPoint{ (m_to ? mapFromScene(m_to->scenePos()) : m_toPoint) };
  m_toPoint = toPoint;

  double x = toPoint.x() < 0. ? toPoint.x() : 0.;
  double y = toPoint.y() < 0. ? toPoint.y() : 0.;
  double w = fabs(toPoint.x());
  double h = fabs(toPoint.y());

  m_boundingRect.setX(x);
  m_boundingRect.setY(y);
  m_boundingRect.setWidth(w);
  m_boundingRect.setHeight(h);

  QPointF c1{}, c2{};

  double distW{ fabs(m_toPoint.x()) * 0.5 };
  double distH{ fabs(m_toPoint.y()) * 0.5 };

  bool fromOrientU = m_from->node()->getRotation()==-90;
  bool toOrientU = m_to!=NULL && m_to->node()->getRotation()==-90;
  bool fromOrientL = EOrientation::eLeft == m_from->node()->element()->orientation();
  bool fromInp = m_from->ioType() == IOSocketsType::eInputs;
  bool toInp = m_to!=NULL && m_to->ioType() == IOSocketsType::eInputs;

  m_path = QPainterPath{};
  bool cubic = true;
  std::string slog = "none:";
  //!TODO! about 8 cases to handle - dependance of from/to, inp/out/, direction etc
  /*if (fromOrientU && !toInp) {//u2l
	  c1.setX(0);
	  if (m_toPoint.y()>0){
		  c1.setY(distH);
	  } else {
		  c1.setY(0-distH);
	  }
	  if (m_toPoint.x()>0){
		  c1.setX(distW);
	  } else {
		  c1.setX(0-distW);
	  }
	  c2.setY(0);
	  //cubic = false;
	  //m_path.arcTo(m_boundingRect,0,-30);

  } else*/ if (fromOrientU) {//u2r
	  if (fromInp){
		  c1.setX(0);
		  c1.setY(distH);
		  c2.setX(0);
		  c2.setY(distH);
	  } else {
		  c1.setX(0);
		  c1.setY(0-distH);
		  c2.setX(0);
		  c2.setY(0-distH);
	  }
  } else if (toOrientU) {//u2r
	  if (fromInp){
		  c1.setX(0-distW);
		  c1.setY(0);
		  c2.setX(0-distW);
		  c2.setY(0);
	  } else {
		  c1.setX(distW);
		  c1.setY(0);
		  c2.setX(distW);
		  c2.setY(0);
	  }
 /* } else if (toOrientU) {
	  c1.setY(0);
	  //c1.setY(0);
	  if (m_toPoint.x()>0){
		  c1.setX(distW);
	  } else {
		  c1.setX(0-distW);
	  }
	  c2.setX(0);
	  if (m_toPoint.y()>0){
		  c2.setY(m_toPoint.y()-distH);
	  } else {
		  c2.setY(0-distH);
	  }*/
  } else {//standard case - "from" oriented right and "to" oriented right
	  if (fromOrientL){
		  c1.setX(0-distW);
		  c2.setX(m_toPoint.x() + distW);
		  c2.setY(m_toPoint.y());
	  } else {
		  c1.setX(distW);
		  c2.setX(m_toPoint.x() - distW);
		  c2.setY(m_toPoint.y());
		  slog="stand:";
	  }
  }
      slog += "{},{},{}";
	  m_path.cubicTo(c1, c2, m_toPoint);
	  //m_from->node()->element()->
	  //!TODO! memory acc problems (global ref invalid?)
	  //consoleAppendF("{},{},{},{},c1.x{},c1.y{},c2.x{},c2.y{}",linkItemPos.x(),linkItemPos.y(),m_toPoint.x(),m_toPoint.y(),c1.x(),c1.y(),c2.x(),c2.y());

  QPainterPathStroker stroker{};
  stroker.setWidth(15);
  m_shape = stroker.createStroke(m_path);
}

} // namespace spaghetti
