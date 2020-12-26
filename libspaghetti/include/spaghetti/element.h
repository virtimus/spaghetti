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
#ifndef SPAGHETTI_ELEMENT_H
#define SPAGHETTI_ELEMENT_H

// clang-format off
#ifdef _MSC_VER
# pragma warning(disable:4251)
# pragma warning(disable:4307)
#endif
// clang-format on

#include <chrono>
#include <functional>
#include <set>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <spaghetti/vendor/json.hpp>

#include <spaghetti/api.h>
#include <spaghetti/strings.h>

namespace spaghetti {

class Package;

enum class ValueType { eBool, eInt, eFloat, eByte, eWord64 };//IoType
enum class SocketItemType { eInput, eOutput, eDynamic };//SiType
enum class IOSocketsType { eInputs, eOutputs, eTop, eDown};//IoSide
enum class EOrientation { eRight, eLeft, eUp, eDown };//TBD

struct EventNameChanged {
  std::string from;
  std::string to;
};
struct EventConsoleTrig {
  std::string text;
};
struct EventIONameChanged {
  std::string from;
  std::string to;
  uint8_t id;
  bool input;
};
struct EventIOTypeChanged {
  bool input;
  uint8_t id;
  ValueType from;
  ValueType to;
};
struct EventEmpty {
};
using EventValue = std::variant<EventNameChanged, EventConsoleTrig, EventIONameChanged, EventIOTypeChanged, EventEmpty>;
enum class EventType {
  eElementNameChanged,
  eConsoleTrig,
  eIONameChanged,
  eIOTypeChanged,
  eInputAdded,
  eInputRemoved,
  eOutputAdded,
  eOutputRemoved
};

struct Event {
  EventType type{};
  EventValue payload{};
};

using EventCallback = std::function<void(Event const &)>;

class SPAGHETTI_API Element {
 public:
  using duration_t = std::chrono::duration<double, std::milli>;

  using Json = nlohmann::json;
  using Value = std::variant<bool, int32_t, float, uint8_t, uint64_t>;
  template<typename T>
  struct Vec2 {
    T x{};
    T y{};
  };
  using vec2 = Vec2<int32_t>;
  using vec2f = Vec2<float>;
  using vec2d = Vec2<double>;

  struct IOSocket {
    enum Flags {
      eCanHoldBool = 1 << 0,
      eCanHoldInt = 1 << 1,
      eCanHoldFloat = 1 << 2,
      eCanChangeName = 1 << 3,
      eCanHoldByte = 1 << 4,
	  eCanHoldWord64 = 1 << 5,
      eCanHoldAllValues = eCanHoldBool | eCanHoldInt | eCanHoldFloat | eCanHoldByte | eCanHoldWord64,
      eDefaultFlags = eCanHoldAllValues | eCanChangeName
    };
    Value value{};
    ValueType type{};

    size_t id{};
    uint8_t slot{};
    uint8_t inFlags{};
    uint8_t flags{};
    std::string name{};

    SocketItemType sItemType{};
  };

  using IOSockets = std::vector<IOSocket>;

  Element() = default;
  virtual ~Element() = default;




  void consoleAppend(char *text);
  template<typename... Args>
  void consoleAppendF(const std::string& format, Args ... args);

  virtual char const *type() const noexcept = 0;
  virtual string::hash_t hash() const noexcept = 0;

  virtual void serialize(Json &a_json);
  virtual void deserialize(Json const &a_json);
  virtual void deserialize(Json const &a_json, const bool isRootPackage);

  virtual void calculate() {}
  virtual void reset() {}

  virtual void update(duration_t const &a_delta) { (void)a_delta; }

  size_t id() const noexcept { return m_id; }

  void setName(std::string const &a_name);
  void setDesc(std::string const &a_desc);
  bool isRotate();
  bool isRoot() { return m_package == nullptr; }
  void setRotate(bool const &rotate);
  bool isInvertH();
  void setInvertH(bool const &invertH);

  std::string name() const noexcept { return m_name; }
  std::string description() const noexcept { return m_desc; }

  void setPosition(double const a_x, double const a_y)
  {
    m_position.x = a_x;
    m_position.y = a_y;
  }
  void setPosition(vec2d const &a_position) { m_position = a_position; }
  vec2d const &position() const { return m_position; }

  void iconify(bool const a_iconify) { m_isIconified = a_iconify; }
  bool isIconified() const { return m_isIconified; }

  void setIconifyingHidesCentralWidget(bool const a_hide) { m_iconifyingHidesCentralWidget = a_hide; }
  bool iconifyingHidesCentralWidget() const { return m_iconifyingHidesCentralWidget; }

  IOSockets &inputs() { return m_inputs; }
  IOSockets const &inputs() const { return m_inputs; }
  IOSockets &outputs() { return m_outputs; }
  IOSockets const &outputs() const { return m_outputs; }

  bool addInput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags);
  bool addInput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType);
  virtual size_t addInputS(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType);

  void setInputName(uint8_t const a_input, std::string const &a_name);
  void removeInput();
  void clearInputs();

  bool addOutput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags);
  bool addOutput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType);
  virtual size_t addOutputS(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType);
  void setOutputName(uint8_t const a_output, std::string const &a_name);
  void removeOutput();
  void clearOutputs();

  void setIOName(bool const a_input, uint8_t const a_id, std::string const &a_name);
  void setIOValueType(bool const a_input, uint8_t const a_id, ValueType const a_type);

  bool connect(size_t const a_sourceId, uint8_t const a_outputId, uint8_t const a_outputFlags, uint8_t const a_inputId, uint8_t const a_inputFlags);

  uint8_t minInputs() const { return m_minInputs; }
  uint8_t maxInputs() const { return m_maxInputs; }
  uint8_t defaultNewInputFlags() const { return m_defaultNewInputFlags; }
  uint8_t minOutputs() const { return m_minOutputs; }
  uint8_t maxOutputs() const { return m_maxOutputs; }
  uint8_t defaultNewOutputFlags() const { return m_defaultNewOutputFlags; }

  Package *package() const { return m_package; }

  void resetIOSocketValue(IOSocket &a_io);

  void setNode(void *const a_node) { m_node = a_node; }

  EOrientation orientation(){
	  EOrientation orient = EOrientation::eRight;
	  if (m_rotate) {
		  if (!m_invertH){
			  orient = EOrientation::eUp;
		  } else {
			  orient = EOrientation::eDown;
		  }
	  } else {
		  if (m_invertH){
			  orient = EOrientation::eLeft;
		  }
	  }
	  return orient;
  };

  template<typename T>
  T *node()
  {
    return static_cast<T *>(m_node);
  }

  void registerEventHandler(EventCallback const &a_handler) { m_handler = a_handler; }

 protected:
  void handleEvent(Event const &a_event);
  virtual void onEvent(Event const &a_event) { (void)a_event; }

  void setMinInputs(uint8_t const a_min);
  void setMaxInputs(uint8_t const a_max);
  void setDefaultNewInputFlags(uint8_t const a_flags) { m_defaultNewInputFlags = a_flags; }

  void setMinOutputs(uint8_t const a_min);
  void setMaxOutputs(uint8_t const a_max);
  void setDefaultNewOutputFlags(uint8_t const a_flags) { m_defaultNewOutputFlags = a_flags; }
  Package *m_package{};
 protected:
  IOSockets m_inputs{};
  IOSockets m_outputs{};

  friend class Package;

  bool m_rotate{ false };
  bool m_invertH{ false };
  size_t m_id{};
 private:

  std::string m_name{};
  std::string m_desc{};
  vec2d m_position{};
  bool m_isIconified{};
  bool m_iconifyingHidesCentralWidget{};
  uint8_t m_minInputs{};
  uint8_t m_maxInputs{ std::numeric_limits<uint8_t>::max() };
  uint8_t m_minOutputs{};
  uint8_t m_maxOutputs{ std::numeric_limits<uint8_t>::max() };
  uint8_t m_defaultNewInputFlags{};
  uint8_t m_defaultNewOutputFlags{};
  EventCallback m_handler{};
  void *m_node{};
};

template<typename T>
inline void to_json(Element::Json &a_json, Element::Vec2<T> const &a_value)
{
  a_json = Element::Json{ a_value.x, a_value.y };
}

template<typename T>
inline void from_json(Element::Json const &a_json, Element::Vec2<T> &a_value)
{
  a_value.x = a_json[0].get<T>();
  a_value.y = a_json[1].get<T>();
}

} // namespace spaghetti

#endif // SPAGHETTI_ELEMENT_H
