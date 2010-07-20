/*
 *  Renderable.h
 *  Mountainhome
 *
 *  Created by loch on 7/19/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

class Material;
class RenderContext;
class Renderable {
public:
    Renderable();
    virtual ~Renderable();

    void setMaterial(Material *mat);
	Material *getMaterial();

	virtual void render(RenderContext *context) = 0;
    virtual void postRenderNotice(RenderContext *context);
    virtual void preRenderNotice(RenderContext *context);

private:
    Material *_material;

};

#endif
