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
#ifndef UI_COLORS_H
#define UI_COLORS_H

#include <QColor>

enum class Color {
  eNameBackground,
  eFontName,
  eFontType,
  eNodeHeader,
  eNodeContent,
  eSocketBorder,
  eSocketInput,
  eSocketOutput,
  eSocketDrop,
  eSocketHover,
  eBoolSignalOff,
  eBoolSignalOn,
  eIntegerSignalOff,
  eIntegerSignalOn,
  eFloatSignalOff,
  eFloatSignalOn,
  eLink,
  eSelected,
  eWord64SignalOn,
  eWord64SignalOff,
  eByteSignalOn,
  eByteSignalOff,
  eCount
};

QColor get_color(Color const a_color);

#endif // UI_COLORS_H
