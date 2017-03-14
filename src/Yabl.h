/*
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 */

#pragma once

#include <Bounce2.h>
#include <cinttypes>

namespace Yabl {

typedef uint16_t Event;

static constexpr Event PRESS = 0x01;
static constexpr Event RELEASE = 0x02;
static constexpr Event SHORT_RELEASE = 0x04;
static constexpr Event SINGLE_TAP = 0x08;
static constexpr Event DOUBLE_TAP = 0x10;
static constexpr Event HOLD = 0x20;
static constexpr Event LONG_RELEASE = 0x40;
static constexpr Event USER_EVENT = 0x100;
static constexpr Event ALL_EVENTS = 0xFFFF;


class Button;

struct EventInfo {
  Button& button;
  Event event;
  bool operator==(const EventInfo& other) const;
};


class Button : public Bounce
{
public:
  typedef void (*CallbackSimple)();
  typedef void (*CallbackWithEventInfo)(const EventInfo&);
  
  Button() {}

  bool update();
  void reset();
  void sleep() { reset(); }
  void wakeup();
  
  bool down() { return read() != _inverted; }
  bool pressed() { return _inverted ? fell() : rose(); }
  bool released() { return _inverted ? rose() : fell(); }
  bool activity() const { return _currentEvents != 0; }
  bool triggered(Event event) const { return _currentEvents & event; }
  bool gestureStarted() const { return _gestureEvents != 0; }
  bool gestureIncludes(Event event) const { return _gestureEvents & event; }
  void suppressOnce(Event event) { _suppressEvents |= event; }
  
  void holdDuration(unsigned int ms) { _holdDuration = ms; }
  unsigned int holdDuration() const { return _holdDuration; }
  void doubleTapInterval(unsigned int ms) { _doubleTapInterval = ms; }
  unsigned int doubleTapInterval() const { return _doubleTapInterval; }
  void inverted(bool inverted) { _inverted = inverted; }
  bool inverted() const { return _inverted; }
  void callback(CallbackSimple callback, Event forEvents);
  void callback(CallbackWithEventInfo callback, Event forEvents = ALL_EVENTS);

  bool operator==(const Button& other) const { return this == &other; }

protected:
  void triggerEvent(Event event);
  unsigned long previousMillis() const { return previous_millis; }
  
private:
  struct Callback {
    Callback() : type(NONE) {}
    enum {
      NONE = 0,
      SIMPLE,
      WITH_EVENT_INFO
    } type;
    union {
      CallbackSimple simple;
      CallbackWithEventInfo withEventInfo;
    } callback;
  };
  
  void clearEvents() { _currentEvents = 0; }
  Callback* callback(Event forEvent);

  static constexpr int EVENT_COUNT = sizeof(Event) * 8;

  bool _inverted = true;
  unsigned int _holdDuration = 400;
  unsigned int _doubleTapInterval = 150;
  bool _reset = false;
  Event _currentEvents = 0;
  Event _gestureEvents = 0;
  Event _suppressEvents = 0;
  Callback _callbacks[EVENT_COUNT];
};


} // namespace Yabl

using namespace Yabl;
