/*
 *  TerrainBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TerrainBindings.h"

#include <Content/Content.h>
#include <Content/MaterialManager.h>
#include <Content/ShaderManager.h>

#include <ruby/st.h>

//////////////////////////////////////
#pragma mark Terrain utility functions
//////////////////////////////////////

bool TerrainBindings::ConvertRubyParameter(VALUE rParameter, ParameterData &cParameter) {
    if(rParameter == Qtrue) {
        cParameter = ParameterData(true);
    } else if(rParameter == Qfalse) {
        cParameter = ParameterData(false);
    } else if(rParameter == Qnil) {
        Error("Can't set parameter to nil!");
        return false;
    }
    else {
        VALUE rClass = rb_class_of(rParameter);
        if(rClass == rb_cString) {
            cParameter = ParameterData(rb_string_value_cstr(&rParameter));
        } else if(rClass == rb_cFixnum) {
            cParameter = ParameterData(NUM2INT(rParameter));
        } else if(rClass == rb_cFloat) {
            cParameter = ParameterData(NUM2DBL(rParameter));
        } else if(rClass == rb_cNumeric || rClass == rb_cInteger) {
            cParameter = ParameterData(NUM2INT(rParameter));
            Warn("Might have difficulty parsing this parameter!");
        } else {
            Error("Can't parse parameter type " << rClass);
            return false;
        }
    }
    return true;
}

bool TerrainBindings::ConvertCParameter(const ParameterData &cParameter, VALUE &rParameter) {
    if(cParameter.type() == typeid(bool)) {
        rParameter = (boost::any_cast<bool>(cParameter)) ? Qtrue : Qfalse;
    } else if(cParameter.type() == typeid(int)) {
        rParameter = INT2NUM(boost::any_cast<int>(cParameter));
    } else if(cParameter.type() == typeid(double)) {
        rParameter = DBL2NUM(boost::any_cast<double>(cParameter));
    } else if(cParameter.type() == typeid(std::string)) {
        rParameter = rb_str_new2(boost::any_cast<std::string>(cParameter).c_str());
    } else {
        rParameter = Qnil;
        Error("Can't compare properties");
        return false;
    }
    return true;
}

PaletteIndex TerrainBindings::RegisterVoxel(Terrain *cSelf, const Voxel &cVoxel) {
    static ID shaderMethod = rb_intern("shader");
    static ID ambientFactorMethod = rb_intern("ambientFactor");
    static ID diffuseFactorMethod = rb_intern("diffuseFactor");
    static ID textureSetMethod = rb_intern("textureSet");
    static ID bottomTextureMethod = rb_intern("bottomTexture");
    static ID sideTextureMethod = rb_intern("sideTexture");
    static ID topTextureMethod = rb_intern("topTexture");
    static ID textureMethod = rb_intern("texture");
    static ID respondToMethod = rb_intern("respond_to?");
//    static ID toSMethod = rb_intern("to_s");

    VALUE rClass = cVoxel.getType();

    // Get the shader and texture from the class variables
    VALUE rShader = rb_funcall(rClass, shaderMethod, 0);
    std::string cShader = rb_string_value_cstr(&rShader);

//    VALUE rTypeName = rb_funcall(rClass, toSMethod, 0);
//    std::string cTypeName = rb_string_value_cstr(&rTypeName);

    // Begin by creating a new material for this voxel
    Material *newMat = new Material();
    newMat->setShader(Content::GetOrLoad<Shader>(cShader));

    // Deal with changing the color of selected voxels.
    if(cVoxel.hasParameter("selected") && boost::any_cast<bool>(cVoxel.getParameter("selected")) == true) {
        newMat->setShaderParameter("colorHint", new Vector4(0.9, 0.9, 0.6, 1.0));
    } else {
        newMat->setShaderParameter("colorHint", new Vector4(1.0, 1.0, 1.0, 1.0));
    }

    // Set the lighting characetistics.
    newMat->setShaderParameter("ambientFactor", new float(NUM2DBL(rb_funcall(rClass, ambientFactorMethod, 0))));
    newMat->setShaderParameter("diffuseFactor", new float(NUM2DBL(rb_funcall(rClass, diffuseFactorMethod, 0))));

    // Load up the texture information.
#define SET_MATERIAL_TEXTURE(name) \
do { \
    VALUE rTextureName = rb_funcall(rClass, name##Method, 0); \
    std::string cTextureName = cTextureSet + "_" + rb_string_value_cstr(&rTextureName); \
    newMat->setShaderParameter(#name, Content::GetOrLoad<Texture>(cTextureName)); \
} while (0)

    VALUE rTextureSet = rb_funcall(rClass, textureSetMethod, 0);
    std::string cTextureSet = rb_string_value_cstr(&rTextureSet);

    if (rb_funcall(rClass, respondToMethod, 1, ID2SYM(textureMethod)) == Qtrue) {
        SET_MATERIAL_TEXTURE(texture);
    } else {
        SET_MATERIAL_TEXTURE(topTexture);
        SET_MATERIAL_TEXTURE(bottomTexture);
        SET_MATERIAL_TEXTURE(sideTexture);
    }

#undef SET_MATERIAL_TEXTURE

    // Get the voxel type's name and register our new voxel with the voxel palette.
    VALUE rVoxelName = rb_funcall(rClass, rb_intern("to_s"), 0);
    std::string cVoxelName = rb_string_value_cstr(&rVoxelName);
    PaletteIndex index = cSelf->getPalette()->registerVoxel(cVoxelName, cVoxel, newMat);

    // FIXME: REQUIRES DUPLICATE CODE IN TILEPALETTE'S D'TOR.
    std::stringstream name;
    name << "voxel palette entry [" << (int)index << "]";
    newMat->setName(name.str());

    Content::GetMaterialManager()->registerResource(newMat);

    return index;
}

int TerrainBindings::FillInCParam(VALUE key, VALUE value, Voxel *cVoxel) {
    static ID toSMethod = rb_intern("to_s");

    ParameterData pData;
    if(ConvertRubyParameter(value, pData)) {
        VALUE keyName = rb_funcall(key, toSMethod, 0);
        cVoxel->addParameter(rb_string_value_cstr(&keyName), pData);
    }

    return (int)ST_CONTINUE;
}

void TerrainBindings::GenerateCVoxelFromRVoxelType(Terrain *cSelf, VALUE rVoxelType, Voxel &cVoxel) {
    cVoxel.setType(rVoxelType);

    // Get the voxel parameters from the instance attributes
    VALUE rParameters = rb_funcall(rVoxelType, rb_intern("default_parameters"), 0);
    st_foreach(rb_hash_tbl(rParameters), ((int (*)(ANYARGS))(TerrainBindings::FillInCParam)), (st_data_t)&cVoxel);
}

void TerrainBindings::SetAndRegisterVoxel(Terrain *cSelf, int x, int y, int z, const Voxel &cVoxel) {
    PaletteIndex index = cSelf->getPalette()->getPaletteIndex(cVoxel);

    if (index == VoxelPalette::EmptyVoxel) {
        index = RegisterVoxel(cSelf, cVoxel);
    }

    ASSERT(index != VoxelPalette::EmptyVoxel);

    cSelf->setPaletteIndex(x, y, z, index);
}

////////////////////////////////
#pragma mark Actual binding code
////////////////////////////////
ID TerrainBindings::parametersKey = NULL;

TerrainBindings::TerrainBindings()
: RubyBindings<Terrain, false>(
    rb_define_class("Terrain", rb_cObject),
    "TerrainBindings")
{
    parametersKey = rb_intern("parameters");

    rb_define_method(_class, "set_voxel_type", RUBY_METHOD_FUNC(TerrainBindings::SetVoxelType), 4);
    rb_define_method(_class, "get_voxel_type", RUBY_METHOD_FUNC(TerrainBindings::GetVoxelType), 3);
    rb_define_method(_class, "set_voxel_parameter", RUBY_METHOD_FUNC(TerrainBindings::SetVoxelParameter), 5);
    rb_define_method(_class, "get_voxel_parameter", RUBY_METHOD_FUNC(TerrainBindings::GetVoxelParameter), 4);

    rb_define_method(_class, "get_surface_level", RUBY_METHOD_FUNC(TerrainBindings::GetSurfaceLevel), 2);
    rb_define_method(_class, "get_surface_type", RUBY_METHOD_FUNC(TerrainBindings::GetSurfaceType), 2);

    rb_define_method(_class, "out_of_bounds?", RUBY_METHOD_FUNC(TerrainBindings::OutOfBounds), 3);
    rb_define_method(_class, "clear", RUBY_METHOD_FUNC(TerrainBindings::Clear), 0);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(TerrainBindings::GetWidth), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(TerrainBindings::GetHeight), 0);
    rb_define_method(_class, "depth", RUBY_METHOD_FUNC(TerrainBindings::GetDepth), 0);
    rb_define_method(_class, "poly_reduction=", RUBY_METHOD_FUNC(TerrainBindings::SetPolyReduction), 1);
    rb_define_method(_class, "auto_update=", RUBY_METHOD_FUNC(TerrainBindings::SetAutoUpdate), 1);
}

VALUE TerrainBindings::OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    int cX = NUM2INT(x);
    int cY = NUM2INT(y);
    int cZ = NUM2INT(z);

    return cSelf->isOutOfBounds(cX, cY, cZ) ? Qtrue : Qfalse;
}

VALUE TerrainBindings::SetVoxelType(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rVoxel) {
    Terrain *cSelf = Get()->getPointer(rSelf);

    if (rVoxel == Qnil) {
        cSelf->setPaletteIndex(NUM2INT(x), NUM2INT(y), NUM2INT(z), VoxelPalette::EmptyVoxel);
    } else {
        Voxel cVoxel;
        GenerateCVoxelFromRVoxelType(cSelf, rVoxel, cVoxel);
        SetAndRegisterVoxel(cSelf, NUM2INT(x), NUM2INT(y), NUM2INT(z), cVoxel);
    }

    return rSelf;
}

VALUE TerrainBindings::GetVoxelType(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    const Voxel *voxel = cSelf->getVoxel(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return voxel ? voxel->getType() : Qnil;
}

VALUE TerrainBindings::SetVoxelParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter, VALUE rParamValue) {
    static ID toSMethod = rb_intern("to_s");

    ParameterData pData;
    if (ConvertRubyParameter(rParamValue, pData)) {
        Terrain *cSelf = Get()->getPointer(rSelf);
        int cX = NUM2INT(x),
            cY = NUM2INT(y),
            cZ = NUM2INT(z);

        const Voxel *cVoxel = cSelf->getVoxel(cX, cY, cZ);
        if (cVoxel) {
            VALUE rParameterName = rb_funcall(rParameter, toSMethod, 0);
            char *cParameterName = rb_string_value_cstr(&rParameterName);

            if (cVoxel->hasParameter(cParameterName)) {
                Voxel tmpVoxel = *cVoxel;
                tmpVoxel.setParameter(cParameterName, pData);
                SetAndRegisterVoxel(cSelf, cX, cY, cZ, tmpVoxel);
            }
        }
    }

    return rSelf;
}

VALUE TerrainBindings::GetVoxelParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    static ID toSMethod = rb_intern("to_s");

    VALUE rParamValue = Qnil;
    VALUE rParameterName = rb_funcall(rParameter, toSMethod, 0);
    char *cParameterName = rb_string_value_cstr(&rParameterName);
    const Voxel * voxel = cSelf->getVoxel(NUM2INT(x), NUM2INT(y), NUM2INT(z));

    if (voxel->hasParameter(cParameterName)) {
        ConvertCParameter(voxel->getParameter(cParameterName), rParamValue);
    }

    return rParamValue;
}

VALUE TerrainBindings::GetSurfaceType(VALUE rSelf, VALUE x, VALUE y) {
    return GetVoxelType(rSelf, x, y, GetSurfaceLevel(rSelf, x, y));
}

VALUE TerrainBindings::GetSurfaceLevel(VALUE rSelf, VALUE x, VALUE y) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE TerrainBindings::Clear(VALUE rSelf) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    cSelf->clear();
    return rSelf;
}

VALUE TerrainBindings::GetWidth(VALUE rSelf) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE TerrainBindings::GetHeight(VALUE rSelf) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE TerrainBindings::GetDepth(VALUE rSelf) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getDepth());
}

VALUE TerrainBindings::SetPolyReduction(VALUE rSelf, VALUE val) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setPolyReduction(val != Qnil && val != Qfalse);
    return Qnil;
}

VALUE TerrainBindings::SetAutoUpdate(VALUE rSelf, VALUE val) {
    Terrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setAutoUpdate(val != Qnil && val != Qfalse);
    return Qnil;
}
