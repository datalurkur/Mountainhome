/*
 *  ShaderGLSL.h
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SHADERGLSL_H_
#define _SHADERGLSL_H_
#include <map>

#include "PTreeResourceFactory.h"
#include "GL_Helper.h"
#include "Shader.h"

class ShaderGLSL : public Shader {
public:
    class Factory : public PTreeResourceFactory<Shader> {
    public:
        Factory(ResourceGroupManager *manager);
        virtual ~Factory();

        bool canLoad(const std::string &args);
        Shader* load(const std::string &args);
    };

protected:
    friend class ShaderManager;
    template <typename Resource> friend class ResourceManager;
    
    ShaderGLSL(const std::string &vertexString, const std::string &geomString, const std::string &fragmentString);
    virtual ~ShaderGLSL();

    GLuint compile(const std::string &programString, GLenum type);
    void unload();

public:
    virtual void enable();

    virtual void disable();

    virtual void bindAttributeToChannel(const std::string &name, int channel);

    virtual void setParameter(const std::string &strVariable, ShaderParameter *param);

    bool hasVertexShader();

    bool hasGeometryShader();

    bool hasFragmentShader();

private:
    /*! Maps texture uniform IDs to paired texture channels and textures. When Textures
     *  are set, they're tied to a texture channel. Then, when we enable the Shader, we
     *  bind the textures to the proper channel. */
    struct TextureChannelBinding {
        TextureChannelBinding(): texture(NULL), channel(-1) {}
        Texture *texture;
        GLint channel;
    };

    typedef std::map<GLint, TextureChannelBinding> TextureChannelBindingMap;

    TextureChannelBindingMap _textures;

private:
    GLuint _vertexShader;
    GLuint _geometryShader;
    GLuint _fragmentShader;
    GLuint _programHandle;

    const std::string _vertString;
    const std::string _geomString;
    const std::string _fragString;

    GLint getVariable(const std::string &strVariable) const;

};

#endif
