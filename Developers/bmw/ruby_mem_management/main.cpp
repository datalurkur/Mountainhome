#include <ruby.h>

static VALUE require_wrap(VALUE arg) {
    return rb_require("main.rb");
}

class MyClass {
public:
    static void SetupBindings() {
        Class = rb_define_class("MyClassInRuby", rb_cObject);
        rb_define_alloc_func(Class, MyClass::Alloc);
        rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MyClass::Initialize), 1);
        rb_define_method(Class, "do", RUBY_METHOD_FUNC(MyClass::DoStuff), 1);
    }

    static VALUE Alloc(VALUE klass) {
        MyClass *ptr = new MyClass();
        printf("ALLOCING: %p\n", ptr);
        return Data_Wrap_Struct(klass, MyClass::Mark, MyClass::Free, ptr);
    }

    static VALUE Initialize(VALUE self, VALUE arg1) {
        MyClass *cSelf;
        Data_Get_Struct(self, MyClass, cSelf);
        printf("INITIALIZING: %p\n", cSelf);
        cSelf->arg1 = NUM2INT(arg1);
        return self;
    }

    static VALUE DoStuff(VALUE self, VALUE arg2) {
        MyClass *cSelf;
        Data_Get_Struct(self, MyClass, cSelf);
        printf("DOING: %p\n", cSelf);
        cSelf->doStuff(NUM2INT(arg2));
        return self;
    }

    static void Mark(MyClass *data) {
        printf("MARKING: %p\n", data);
    }

    static void Free(MyClass *data) {
        printf("FREEING: %p\n", data);
        delete data;
    }

protected:
    static VALUE Class;

public:
    MyClass(): arg1(0), arg2(0) {
        printf("Constructing: %p\n", this);
    }

    void doStuff(int newArg2) {
        arg2 = newArg2;
        printf("Doing: %p [%i %i]\n", this, arg1, arg2);
    }

protected:
    int arg1, arg2;

};

VALUE MyClass::Class;

int main(int argc, char **argv) {
    int state;
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();

        MyClass::SetupBindings();

        rb_protect(require_wrap, 0, &state);
        printf("Ending state: %i\n", state);
    }

    return ruby_cleanup(0);
}
