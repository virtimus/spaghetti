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

#pragma once
#ifndef NODES_VALUES_CHARACTERISTIC_CURVE_EDITOR_WINDOW_H
#define NODES_VALUES_CHARACTERISTIC_CURVE_EDITOR_WINDOW_H

#include <QDialog>
#include <QGraphicsItem>

namespace QtCharts {
class QChart;
class QLineSeries;
} // namespace QtCharts

namespace spaghetti::nodes::values {

class CharacteristicCurve;

namespace characteristic_curve {

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QDialog {
  Q_OBJECT

 public:
  enum class ValueType{ eInt, eDouble };

  explicit EditorWindow(CharacteristicCurve *const a_characteristicCurve);
  ~EditorWindow();

  void showEvent(QShowEvent *a_event) override;
  void resizeEvent(QResizeEvent *a_event) override;

  void synchronizeToNode();
  void synchronizeFromNode();

  void setValue(qreal const a_value);

  void addPoint(int const a_index, QPointF const a_point);
  void changePoint(int const a_index, QPointF const a_point);
  void removePoint(int const a_index);

  void recreateSeries();

  void setXValueType(ValueType const a_type);
  ValueType xValueType() const { return m_xValueType; }
  void setYValueType(ValueType const a_type);
  ValueType yValueType() const { return m_yValueType; }

 private:
  void setLive(bool const a_live);

 private:
  Ui::EditorWindow *const m_ui{};
  CharacteristicCurve *const m_characteristicCurve{};
  ValueType m_xValueType{ ValueType::eInt };
  ValueType m_yValueType{ ValueType::eDouble };
  bool m_live{};
};

} // namespace characteristic_curve
} // namespace spaghetti::nodes::values

#endif // NODES_VALUES_CHARACTERISTIC_CURVE_EDITOR_WINDOW_H
