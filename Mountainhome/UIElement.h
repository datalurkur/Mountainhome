#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_
#include "RubyBindings.h"
#include <string>

class Entity;

class UIElement : public ManyObjectBinding<UIElement> {
public:
  static void SetupBindings();

  static VALUE Initialize(VALUE self, VALUE name, VALUE width, VALUE height, VALUE mat);
  static VALUE SetPosition(VALUE self, VALUE x, VALUE y);

  UIElement(const std::string name, float width, float height, const std::string mat);
  virtual ~UIElement();

private:
  float _width, _height;
  UIElement *_parent; // The parent UI Element, for recursive menu destruction
  Entity *_entity;    // For rendering in the UI-specific scene
};

#endif
