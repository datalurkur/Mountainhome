#include "UIElement.h"

void UIElement::SetupBindings() {
  Class = rb_define_class("UIElement", rb_cObject);
  rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(UIElement::Initialize), 4);
  rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(UIElement::SetPosition), 2);
}

VALUE UIElement::Initialize(VALUE self, VALUE name, VALUE width, VALUE height, VALUE mat) {
  std::string strName = rb_string_value_cstr(&name);
  std::string strMat  = rb_string_value_cstr(&mat);
  float fWidth        = rb_num2dbl(width);
  float fHeight       = rb_num2dbl(height);

  UIElement::RegisterObject(self, new UIElement(strName, fWidth, fHeight, strMat));
  return self;
}

VALUE UIElement::SetPosition(VALUE self, VALUE x, VALUE y) {
  GetObject(self)->_entity->setPosition(NUM2INT(x), NUM2INT(y), 0);
}

UIElement::UIElement(const std::string name, float width, float height, const std::string material): _width = width, _height = height, _entity(NULL) {
  // FIXME - add in the scene that will manage the UI here
}

UIElement::~UIElement() {}
