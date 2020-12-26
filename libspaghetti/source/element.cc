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

#include "spaghetti/element.h"

#include <cassert>
#include <iostream>
//#include <format>
#include <memory>
#include <string>
#include <stdexcept>

#include "spaghetti/package.h"
#include "spaghetti/logger.h"

namespace spaghetti {

void Element::serialize(Element::Json &a_json)
{
  auto &jsonElement = a_json["element"];
  jsonElement["id"] = m_id;
  jsonElement["name"] = m_name;
  jsonElement["description"] = m_desc;
  jsonElement["type"] = type();
  jsonElement["rotate"] = m_rotate;
  jsonElement["invertH"] = m_invertH;
  jsonElement["min_inputs"] = m_minInputs;
  jsonElement["max_inputs"] = m_maxInputs;
  jsonElement["min_outputs"] = m_minOutputs;
  jsonElement["max_outputs"] = m_maxOutputs;
  jsonElement["default_new_input_flags"] = m_defaultNewInputFlags;
  jsonElement["default_new_output_flags"] = m_defaultNewOutputFlags;

  auto getSocketType = [](ValueType const a_type) {
    switch (a_type) {
      case ValueType::eBool: return "bool";
      case ValueType::eInt: return "int";
      case ValueType::eFloat: return "float";
      case ValueType::eByte: return "byte";
      case ValueType::eWord64: return "word64";
    }
    assert(false && "Wrong socket type");
    return "unknown";
  };

  auto getSocketItemType = [](SocketItemType const a_type) {
    switch (a_type) {
      case SocketItemType::eInput: return "i";
      case SocketItemType::eOutput: return "o";
      case SocketItemType::eDynamic: return "d";
    }
    //spaghetti::log::info("[element.serialize] Wrong socket item type: {}",a_type);
    //assert(false && "[element.serialize] Wrong socket item type");
    return "unknown";
  };

  bool isPackageElem = false;
  std::string str1 ("logic/package");
  if ( str1.compare(this->type())==0 ) {
      isPackageElem = true;
  }

  auto jsonInputs = Json::array();
  size_t const INPUTS_COUNT{ m_inputs.size() };
  for (size_t i = 0; i < INPUTS_COUNT; ++i) {
    Json socket{};
    socket["socket"] = i;
    socket["type"] = getSocketType(m_inputs[i].type);
    if (isPackageElem){// for package allways output
        socket["siType"] = getSocketItemType(SocketItemType::eInput);
    } else {
        socket["siType"] = getSocketItemType(m_inputs[i].sItemType);
    }

    socket["name"] = m_inputs[i].name;
    socket["flags"] = m_inputs[i].flags;
    socket["inFlags"] = m_inputs[i].inFlags;
    jsonInputs.push_back(socket);
  }

  auto jsonOutputs = Json::array();
  size_t const OUTPUTS_COUNT{ m_outputs.size() };
  for (size_t i = 0; i < OUTPUTS_COUNT; ++i) {
    Json socket{};
    socket["socket"] = i;
    socket["type"] = getSocketType(m_outputs[i].type);
    if (isPackageElem){// f
        socket["siType"] = getSocketItemType(SocketItemType::eOutput);
    } else {
        socket["siType"] = getSocketItemType(m_outputs[i].sItemType);
    }

    socket["name"] = m_outputs[i].name;
    socket["flags"] = m_outputs[i].flags;
    socket["inFlags"] = m_outputs[i].inFlags;
    jsonOutputs.push_back(socket);
  }

  auto &jsonIo = jsonElement["io"];
  jsonIo["inputs"] = jsonInputs;
  jsonIo["outputs"] = jsonOutputs;

  auto &jsonNode = a_json["node"];
  jsonNode["position"]["x"] = m_position.x;
  jsonNode["position"]["y"] = m_position.y;
  jsonNode["iconify"] = m_isIconified;
  jsonNode["iconifying_hides_central_widget"] = m_iconifyingHidesCentralWidget;
}

void Element::deserialize(Json const &a_json){
    deserialize(a_json,false);
}

void Element::deserialize(Json const &a_json, const bool isRootPackage)
{
  auto const &ELEMENT = a_json["element"];
  auto const NAME = ELEMENT["name"].get<std::string>();
  auto const DESC = (ELEMENT.find("description")!=ELEMENT.end())?ELEMENT["description"].get<std::string>():"";
  auto const ROTATE = (ELEMENT.find("rotate")!=ELEMENT.end())?ELEMENT["rotate"].get<bool>():false;
  auto const INVERTH = (ELEMENT.find("invertH")!=ELEMENT.end())?ELEMENT["invertH"].get<bool>():false;
  auto const MIN_INPUTS = ELEMENT["min_inputs"].get<uint8_t>();
  auto const MAX_INPUTS = ELEMENT["max_inputs"].get<uint8_t>();
  auto const MIN_OUTPUTS = ELEMENT["min_outputs"].get<uint8_t>();
  auto const MAX_OUTPUTS = ELEMENT["max_outputs"].get<uint8_t>();
  auto const DEFAULT_NEW_INPUT_FLAGS = ELEMENT["default_new_input_flags"].get<uint8_t>();
  auto const DEFAULT_NEW_OUTPUT_FLAGS = ELEMENT["default_new_output_flags"].get<uint8_t>();

  auto const &IO = ELEMENT["io"];
  auto const &INPUTS = IO["inputs"];
  auto const &OUTPUTS = IO["outputs"];

  auto const &NODE = a_json["node"];
  auto const ICONIFY = NODE["iconify"].get<bool>();
  auto const ICONIFYING_HIDES_CENTRAL_WIDGET = NODE["iconifying_hides_central_widget"].get<bool>();
  auto const &POSITION = NODE["position"];
  auto const POSITION_X = POSITION["x"].get<double>();
  auto const POSITION_Y = POSITION["y"].get<double>();

  setName(NAME);
  setDesc(DESC);
  setRotate(ROTATE);
  setInvertH(INVERTH);
  setPosition(POSITION_X, POSITION_Y);
  clearInputs();
  clearOutputs();
  setMinInputs(MIN_INPUTS);
  setMaxInputs(MAX_INPUTS);
  setMinOutputs(MIN_OUTPUTS);
  setMaxOutputs(MAX_OUTPUTS);
  setDefaultNewInputFlags(DEFAULT_NEW_INPUT_FLAGS);
  setDefaultNewOutputFlags(DEFAULT_NEW_OUTPUT_FLAGS);
  iconify(ICONIFY);
  setIconifyingHidesCentralWidget(ICONIFYING_HIDES_CENTRAL_WIDGET);

  auto add_socket = [&](Json const &a_socket, bool const a_input, uint8_t &a_socketCount, bool const isRootPackage) {
    auto const SOCKET_ID = a_socket["socket"].get<uint8_t>();
    auto const SOCKET_STRING_TYPE = a_socket["type"].get<std::string>();
    auto const SOCKET_STRING_ITEM_TYPE = (a_socket.find("siType")!=a_socket.end())?a_socket["siType"].get<std::string>():"unknown";
    auto const SOCKET_NAME = a_socket["name"].get<std::string>();
    auto const SOCKET_FLAGS = a_socket["flags"].get<uint8_t>();

    assert(a_socketCount == SOCKET_ID);

    ValueType const SOCKET_TYPE = [](std::string_view const a_type) {
      if (a_type == "bool")
        return ValueType::eBool;
      else if (a_type == "int")
        return ValueType::eInt;
      else if (a_type == "float")
        return ValueType::eFloat;
      else if (a_type == "byte")
        return ValueType::eByte;
      else if (a_type == "word64")
        return ValueType::eWord64;
      assert(false && "Wrong socket type");
      return ValueType::eBool;
    }(SOCKET_STRING_TYPE);



    SocketItemType const SOCKET_ITEM_TYPE = [](std::string_view const a_type, bool const a_input, IOSockets &inputs, IOSockets &outputs, uint8_t socketId, const bool isRootPackage) {
       SocketItemType siType;
       bool isKnown = false;
       if (isRootPackage){
           siType = (a_input)?SocketItemType::eOutput:SocketItemType::eInput;
           isKnown = true;
       } else {
           if (a_input){
               if (socketId<inputs.size()){
                   siType = inputs[socketId].sItemType;
                   isKnown = true;
               }
           } else {
               if (socketId<outputs.size()){
                   siType = outputs[socketId].sItemType;
                   isKnown = true;
               }
           }
       }
      if (isKnown){
	  // if (true){
		   //getSocketItemTypeBySocketId(uint8_t socketId, bool const a_input,&siType)
		    //return (a_input)?m_inputs[socketId].sItemType:m_outputs[socketId].sItemType;
    	  //allRight -siTypeReady
      } else if (a_type == "i")
        siType = SocketItemType::eInput;
      else if (a_type == "o")
    	siType = SocketItemType::eOutput;
      else if (a_type == "d")
    	siType = SocketItemType::eDynamic;
      else
    	  return a_input ? SocketItemType::eInput : SocketItemType::eOutput;
	   return siType;
    }(SOCKET_STRING_ITEM_TYPE,a_input,m_inputs,m_outputs,SOCKET_ID,isRootPackage);

    //SocketItemType siType = (a_input)?m_inputs[SOCKET_ID].sItemType:m_outputs[SOCKET_ID].sItemType;
    //auto const SOCKET_NAME = a_input ? m_inputs[SOCKET_ID].name : m_outputs[SOCKET_ID].name;
    //!SocketItemType const SOCKET_ITEM_TYPE =  a_input ? m_inputs[SOCKET_ID].sItemType : m_outputs[SOCKET_ID].sItemType;
    a_input ? addInput(SOCKET_TYPE, SOCKET_NAME, SOCKET_FLAGS, SOCKET_ITEM_TYPE) : addOutput(SOCKET_TYPE,SOCKET_NAME , SOCKET_FLAGS, SOCKET_ITEM_TYPE);
    a_socketCount++;
  };

  uint8_t inputsCount{}, outputsCount{};
  for (auto &&socket : INPUTS) add_socket(socket, true, inputsCount, isRootPackage);
  for (auto &&socket : OUTPUTS) add_socket(socket, false, outputsCount, isRootPackage);
}

void Element::setName(std::string const &a_name)
{
  auto const OLD_NAME = m_name;
  m_name = a_name;

  handleEvent(Event{ EventType::eElementNameChanged, EventNameChanged{ OLD_NAME, a_name } });
}

void Element::setDesc(std::string const &a_desc)
{
    auto const OLD_DESC = m_desc;
    m_desc = a_desc;
}

/*void Element::setRotate(bool const &rotate){
	m_rotate = rotate;
}
void Element::setInvertH(bool const &invertH){
	m_invertH = invertH;
}*/

bool Element::addInput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags){
	return addInput(a_type,a_name,a_flags,SocketItemType::eInput);
}

size_t Element::addInputS(ValueType const a_type, std::string const &a_name, uint8_t const a_flags,SocketItemType sItemType){
	  size_t index = m_inputs.size();
	  if (index + 1 > m_maxInputs) return -1;

	  IOSocket input{};
	  input.name = a_name;
	  input.type = a_type;
	  input.flags = a_flags;
	  input.sItemType = sItemType;
	  input.inFlags = 1;

	  resetIOSocketValue(input);
	  m_inputs.emplace_back(input);

	  handleEvent(Event{ EventType::eInputAdded, EventEmpty{} });

	  return index;
}

bool Element::addInput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags,SocketItemType sItemType)
{
	return (addInputS(a_type,a_name,a_flags,sItemType)>=0);
}

void Element::setInputName(uint8_t const a_input, std::string const &a_name)
{
  auto const OLD_NAME = m_inputs[a_input].name;
  if (OLD_NAME == a_name) return;

  m_inputs[a_input].name = a_name;

  handleEvent(Event{ EventType::eIONameChanged, EventIONameChanged{ OLD_NAME, a_name, a_input, true } });
}

void Element::removeInput()
{
  m_inputs.pop_back();

  handleEvent(Event{ EventType::eInputRemoved, EventEmpty{} });
}

void Element::clearInputs()
{
  m_inputs.clear();
}

bool Element::addOutput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags){
	return addOutput(a_type,a_name,a_flags,SocketItemType::eOutput);
}

size_t Element::addOutputS(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType){
	  size_t index = m_outputs.size();
	  if (index + 1 > m_maxOutputs) return -1;

	  IOSocket output{};
	  output.name = a_name;
	  output.type = a_type;
	  output.flags = a_flags;
	  output.sItemType = sItemType;
	  output.inFlags = 2;

	  resetIOSocketValue(output);
	  m_outputs.emplace_back(output);

	  handleEvent(Event{ EventType::eOutputAdded, EventEmpty{} });

	  return index;

}

bool Element::addOutput(ValueType const a_type, std::string const &a_name, uint8_t const a_flags, SocketItemType sItemType)
{
	return (addOutputS(a_type, a_name, a_flags, sItemType)>=0);
}

void Element::setOutputName(uint8_t const a_output, std::string const &a_name)
{
  auto const OLD_NAME = m_outputs[a_output].name;
  if (OLD_NAME == a_name) return;

  m_outputs[a_output].name = a_name;

  handleEvent(Event{ EventType::eIONameChanged, EventIONameChanged{ OLD_NAME, a_name, a_output, false } });
}

void Element::removeOutput()
{
  m_outputs.pop_back();

  handleEvent(Event{ EventType::eOutputRemoved, EventEmpty{} });
}

void Element::clearOutputs()
{
  m_outputs.clear();
}

void Element::setIOName(bool const a_input, uint8_t const a_id, std::string const &a_name)
{
  if (a_input)
    setInputName(a_id, a_name);
  else
    setOutputName(a_id, a_name);
}

void Element::setIOValueType(bool const a_input, uint8_t const a_id, ValueType const a_type)
{
  auto &io = a_input ? m_inputs[a_id] : m_outputs[a_id];
  auto const OLD_TYPE = io.type;

  if (OLD_TYPE == a_type) return;

  io.type = a_type;
  resetIOSocketValue(io);

  handleEvent(Event{ EventType::eIOTypeChanged, EventIOTypeChanged{ a_input, a_id, OLD_TYPE, a_type } });
}

bool Element::connect(size_t const a_sourceId, uint8_t const a_outputId, uint8_t const a_outputFlags, uint8_t const a_inputId, uint8_t const a_inputFlags)
{
  return m_package->connect(a_sourceId, a_outputId, a_outputFlags, m_id, a_inputId, a_inputFlags);
}

void Element::resetIOSocketValue(IOSocket &a_io)
{
  switch (a_io.type) {
    case ValueType::eBool: a_io.value = false; break;
    case ValueType::eInt: a_io.value = 0; break;
    case ValueType::eFloat: a_io.value = 0.0f; break;
    case ValueType::eByte: a_io.value = (uint8_t)0; break;
    case ValueType::eWord64: a_io.value = (uint64_t)0; break;
  }
}

void Element::handleEvent(Event const &a_event)
{
  onEvent(a_event);
  if (m_handler) m_handler(a_event);
}

void Element::setMinInputs(uint8_t const a_min)
{
  if (a_min > m_maxInputs) return;
  m_minInputs = a_min;
}

void Element::setMaxInputs(uint8_t const a_max)
{
  if (a_max < m_minInputs) return;
  m_maxInputs = a_max;
}

void Element::setMinOutputs(uint8_t const a_min)
{
  if (a_min > m_maxOutputs) return;
  m_minOutputs = a_min;
}

void Element::setMaxOutputs(uint8_t const a_max)
{
  if (a_max < m_minOutputs) return;
  m_maxOutputs = a_max;
}

void Element::consoleAppend(char *text){
	spaghetti::log::info(text);
	handleEvent(Event{ EventType::eConsoleTrig, EventConsoleTrig{ text } });
}

/*template<typename... Args>
void Element::consoleAppendF(Args ... args){
  std::stringstream sstream;
  sstream << std::format(args...);
  char *cstr = new char[sstream.str().length() + 1];
  strcpy(cstr, sstream.str().c_str());
   consoleAppend(cstr);
}*/

template<typename ... Args>
void Element::consoleAppendF( const std::string& format, Args ... args )
{
	spaghetti::log::info(format, args ...);
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    std::string str = std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    consoleAppend(cstr);
}

bool Element::isInvertH(){
	return m_invertH;
}
void Element::setInvertH(bool const &invertH){
	if (invertH){
		m_invertH = true;
	} else {
		m_invertH = false;
	}
}

bool Element::isRotate(){
	return m_rotate;//EOrientation::eUp == orientation();
}
void Element::setRotate(bool const &rotate){
	//m_orient = (n)?EOrientation::eUp:EOrientation::eRight;
	if (rotate){
		m_rotate = true;
		//m_invertH = false;
	} else {
		m_rotate = false;
		//m_invertH = false;
	}
	//updateRotation();
}

} // namespace spaghetti
