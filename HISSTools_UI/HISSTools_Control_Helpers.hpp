
#ifndef __HISSTOOLS_CONTROL_HELPERS__
#define __HISSTOOLS_CONTROL_HELPERS__

#include "../HISSTools_Graphics/HISSTools_VecLib.hpp"
#include "HISSTools_Design_Scheme.hpp"

#include <IControl.h>

// HISSTools_Graphics_Types
// A base struct to bring iplug and igraphics items into scope

struct HISSTools_Graphics_Types
{    
    using IEditorDelegate = iplug::IEditorDelegate;
    template <typename T>
    using IPlugQueue = iplug::IPlugQueue<T>;
    using IParam = iplug::IParam;
    using IKeyPress = iplug::IKeyPress;

    using IGraphics = iplug::igraphics::IGraphics;
    using IPopupMenu = iplug::igraphics::IPopupMenu;
    using IBitmap = iplug::igraphics::IBitmap;
    using IRECT = iplug::igraphics::IRECT;
    using IPattern = iplug::igraphics::IPattern;
    using IMatrix = iplug::igraphics::IMatrix;
    using IColor = iplug::igraphics::IColor;
    using IText = iplug::igraphics::IText;
    using IShadow = iplug::igraphics::IShadow;
    using IFillOptions = iplug::igraphics::IFillOptions;
    using IStrokeOptions = iplug::igraphics::IStrokeOptions;
    using IMouseMod = iplug::igraphics::IMouseMod;
    using IActionFunction = iplug::igraphics::IActionFunction;
    using ILayerPtr = iplug::igraphics::ILayerPtr;
    
    using EBlend = iplug::igraphics::EBlend;
    using EAlign = iplug::igraphics::EAlign;
    using EVAlign = iplug::igraphics::EVAlign;
    using ETextStyle = iplug::igraphics::ETextStyle;
    using EDirection = iplug::igraphics::EDirection;
    using EFileAction = iplug::igraphics::EFileAction;
    using EWinding = iplug::igraphics::EWinding;
    using EFillRule = iplug::igraphics::EFillRule;
    using ELineCap = iplug::igraphics::ELineCap;
    using ELineJoin = iplug::igraphics::ELineJoin;
    using EPatternType = iplug::igraphics::EPatternType;
    using EPatternExtend = iplug::igraphics::EPatternExtend;
    using ECursor = iplug::igraphics::ECursor;
};


// HISSTools_Control_Layers
// A class to cache layers in control drawing (currently only supports a single background...)

class HISSTools_Control_Layers : protected virtual HISSTools_Graphics_Types
{
    
public:
	
    bool startBackground(IGraphics& g, HISSTools_VecLib& vecDraw, IRECT area);
    void renderBackground(IGraphics& g, HISSTools_VecLib& vecDraw, IRECT area);
    void redrawBackground();
    
private:
    
    ILayerPtr mBackground;
    const static bool sNoCaching = false;
};


// HISSTools_Text_Helper_Block
// Helper class to deal with blocks of text 

class HISSTools_Text_Helper_Block : protected virtual HISSTools_Graphics_Types
{
		
public:
	
	// Constructors
	
    HISSTools_Text_Helper_Block(double x, double y, double w, double h,  HTextAlign hAlign, VTextAlign vAlign, const char *name, const char *type, HISSTools_Design_Scheme *scheme);
	
    HISSTools_Text_Helper_Block(HTextAlign hAlign, VTextAlign vAlign, const char *name, const char *type, HISSTools_Design_Scheme *scheme)
    : HISSTools_Text_Helper_Block(0, 0, 0, 0 , hAlign, vAlign, name, type, scheme)
    {}
	
    void Draw(HISSTools_VecLib& vecDraw);
	
    void resizeText(double x, double y, double w, double h);
	
    void setTextColor(HISSTools_Color_Spec *colorSpec);
    void setText(const char *str);
	
    HISSTools_Bounds bounds();
    
protected:
    
    // Dimensions
    
    double mX;
    double mY;
    double mW;
    double mH;
    
    // Appearance
    
    HISSTools_Text *mTextTS;
    HISSTools_Color_Spec *mTextCS;
    HISSTools_Shadow *mTextSD;
    HTextAlign mHAlign;
    VTextAlign mVAlign;
    
    // String
    
    WDL_String mStr;
};


// HISSTools_Text_Helper_Panel
// Helper class to deal with blocks of text with a panel

class HISSTools_Text_Helper_Panel : public HISSTools_Text_Helper_Block
{

public:
	
    HISSTools_Text_Helper_Panel(double x, double y, double w, double h, double wPad, double hPad, HTextAlign hAlign, VTextAlign vAlign, const char *name, const char *type, HISSTools_Design_Scheme *scheme);
	
    HISSTools_Text_Helper_Panel(HTextAlign hAlign, VTextAlign vAlign, const char *name, const char *type, HISSTools_Design_Scheme *scheme)
    : HISSTools_Text_Helper_Panel(0, 0, 0, 0, 0, 0, hAlign, vAlign, name, type, scheme)
    {}
	
    void Draw(HISSTools_VecLib& vecDraw, bool drawText);

    void changePadding(double lPad, double rPad, double hPad);
    void changePadding(double wPad, double hPad);

    void resize(double x, double y, double w, double h);
	
    void setPanelColor(HISSTools_Color_Spec *colorSpec);
    void setOutlineColor(HISSTools_Color_Spec *colorSpec);
	
    HISSTools_Bounds bounds();
		
protected:
    
    bool doDrawOutline();
    bool doDrawPanel();

    // Dimensions
    
    double mX;
    double mY;
    double mW;
    double mH;
    
    double mLPad;
    double mRPad;
    double mHPad;
    
    // Appearance
    
    HISSTools_Color_Spec *mPanelFillCS;
    HISSTools_Color_Spec *mPanelOutlineCS;
    HISSTools_Shadow *mPanelSD;
    double mPanelRoundness;
    double mPanelOutlineTK;
};


// HISSTools_Text_Helper_Param
// Helper class to deal with blocks of text displaying a parameter

class HISSTools_Text_Helper_Param : public HISSTools_Text_Helper_Panel
{
	
public:
	
	// Constructors
	
    HISSTools_Text_Helper_Param(iplug::igraphics::IControl *control, double x, double y, double w, double h, double pad, HTextAlign hAlign, VTextAlign vAlign, const char *name, const char *type, HISSTools_Design_Scheme *scheme);
	
    bool menuParam();
	
    void finishEdit();
    void hilite(bool on);

    void promptUserInput();
    bool promptUserInput(float x, float y);
		
    void Draw(HISSTools_VecLib& vecDraw);
    
private:
    
    void setControlText();
    
    double roundnessCompensate(double menuTriangleHeight);
    
    iplug::igraphics::IControl *mControl;
    
    double mTextHPad;
    double mSeparatorX;
    
    bool mDrawSeparator;
    bool mDrawTriangle;
    bool mMenuFlipTriangle;
    bool mInEdit;
    
    double mMenuTriangleTop;
    double mMenuTriangleBtm;
    double mMenuTriangleL;
    double mMenuTriangleR;
    
    HISSTools_Color_Spec *mTextHiliteCS;
    HISSTools_Color_Spec *mPanelHiliteCS;
    HISSTools_Color_Spec *mOutlineHiliteCS;
    HISSTools_Color_Spec *mTextCS;
    HISSTools_Color_Spec *mPanelFillCS;
    HISSTools_Color_Spec *mPanelOutlineCS;
    
    // Show Units
    
    bool mShowUnits;
};

// **** Controls **** //

// HISSTools_Button
// On/Off button with text on or off the handle
// FIX - Momentary action and extensibility!!

class HISSTools_Button: public iplug::igraphics::IControl, public HISSTools_Control_Layers
{
    
public:
    
    // Constructor
    
    HISSTools_Button(int paramIdx, double x, double y, double w = 0, double h = 0, const char *type = 0, HISSTools_Design_Scheme *designScheme = &DefaultDesignScheme, const char *label = "");
    
public:
    
    void OnInit() override;
    
    // Mousing Functions
    
    void OnMouseDown(float x, float y, const IMouseMod& pMod) override;
    
    // Draw
    
    void Draw(IGraphics& g) override;
    
protected:
    
    // Text
    
    const char *mName;
    
private:
    
    // Positioning / Dimensions
    
    double mX;
    double mY;
    double mW;
    double mH;
    double mTextPad;
    double mRoundness;
    
    // Line Thicknesses
    
    double mOutlineTK;
    
    // Shadow Spec
    
    HISSTools_Shadow *mShadow;
    
    // Text Spec
    
    HISSTools_Text *mTextStyle;
    
    // Color Specs
    
    HISSTools_Color_Spec *mOnCS;
    HISSTools_Color_Spec *mOffCS;
    HISSTools_Color_Spec *mOutlineCS;
    HISSTools_Color_Spec *mHandleLabelCS;
    HISSTools_Color_Spec *mHandleLabelOffCS;
    HISSTools_Color_Spec *mBackgroundLabelCS;
    HISSTools_Color_Spec *mInactiveOverlayCS;
    
    // Label Mode
    
    bool mLabelMode;
};

#include "HISSTools_Control_Helpers.cpp"

#endif /* __HISSTOOLS_CONTROL_HELPERS__ */
