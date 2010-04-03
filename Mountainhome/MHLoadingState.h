#ifndef _MHLOADINGSTATE_H_
#define _MHLOADINGSTATE_H_
#include "RubyStateProxy.h"
#include "MHUIManager.h"

class MHLoadingState: public RubyStateProxy {
public:
#pragma mark MHLoadingState ruby bindings
    static void SetupBindings();

    static VALUE Initialize(VALUE self);
    static VALUE Teardown(VALUE self);

    static VALUE SetManager(VALUE self, VALUE manager);
    static VALUE GetManager(VALUE self);

public:
#pragma mark MHLoadingState declarations
    MHLoadingState(VALUE robj);
    virtual ~MHLoadingState();

private:
    MHUIManager *_manager;
};

#endif;
