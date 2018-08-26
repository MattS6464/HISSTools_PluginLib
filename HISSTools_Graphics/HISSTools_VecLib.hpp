

#ifndef __HISSTOOLS_VECLIB__
#define __HISSTOOLS_VECLIB__

#include "HISSTools_Color.hpp"
#include "HISSTools_Shadow.hpp"
#include "HISSTools_LICE_Text.hpp"
#include "IGraphics.h"
#include <algorithm>
#include <vector>
#include "cairo/cairo.h"
#include <cmath>

static HISSTools_Color_Spec defaultColor;

class HISSTools_VecLib
{
    double radToDeg(double radians) { return (180.0 * (radians / PI)) + 90.0; }
    
    struct Area
    {
        Area() : x1(0), x2(0), y1(0), y2(0) {}
        Area(double xL, double xH, double yL, double yH) : x1(xL), x2(xH), y1(yL), y2(yH) {}
    
        double x1, x2, y1, y2;
    };
    
public:
	
    HISSTools_VecLib(cairo_t *cairo) : mGraphics(nullptr), mContext(cairo), mShadow(NULL), mWidth(0), mHeight(0), mForceGradientBox(false), mCSOrientation(kCSOrientHorizontal), mScale(1.0), mTextBitmap(NULL)
    {
        setColor(&defaultColor);
    }
    
    ~HISSTools_VecLib()
    {
        delete mTextBitmap;
    }
    
    void setIGraphics(IGraphics* graphics)
    {
        double scale = graphics->GetDisplayScale();
        mGraphics = graphics;
        setSize(graphics->Width(), graphics->Height());
        
        // FIX - clip and other state etc. when pushing and popping?
        
        mContext = (cairo_t *)graphics->GetDrawContext();
        cairo_set_operator(mContext, CAIRO_OPERATOR_OVER);
        mScale = scale;
    }
    
    void setSize(int w, int h)
    {
        mWidth = w;
        mHeight = h;
    }
    
    void setClip()  { cairo_reset_clip(mContext); }
    
    void setClip(HISSTools_Bounds clip)
    {
        //cairo_reset_clip(mContext);
        cairo_rectangle(mContext, clip.mRECT.L, clip.mRECT.T, clip.mRECT.W(), clip.mRECT.H());
        cairo_clip(mContext);
    }
    
    void setClip(double xLo, double yLo, double xHi, double yHi)
    {
        setClip(HISSTools_Bounds(xLo, yLo, xHi - xLo, yHi - yLo));
    }
    
    void setClip(IRECT rect)
    {
        setClip(rect.L, rect.T, rect.R, rect.B);
    }
    
    cairo_t *getContext() const { return mContext; }
    
    void startGroup()
    {
        cairo_push_group(mContext);
    }
    
    cairo_pattern_t *endGroup()
    {
        return cairo_pop_group(mContext);
    }
    
    void renderPattern(cairo_pattern_t *pattern)
    {
        cairo_set_source(mContext, pattern);
        cairo_paint_with_alpha(mContext, 1.0);
    }
    
    double getScale() const { return mScale; }
    
    void setColor(HISSTools_Color_Spec *color)
    {
        mColor = color;
        
        if (mContext)
            mColor->setAsSource(mContext);
    }
    
    // Orientation allows gradient rotation ONLY for relevant Color Specs
    
    void setColorOrientation(ColorOrientation CSOrientation)
    {
        mCSOrientation = CSOrientation;
    }
    
    void forceGradientBox()     { mForceGradientBox = false; }
    
    void forceGradientBox(double xLo, double yLo, double xHi, double yHi)
    {
        mGradientArea = Area(xLo, xHi, yLo, yHi);
        mForceGradientBox = true;
    }
    
    double getX() const
    {
        double x, y;
        
        cairo_get_current_point(mContext, &x, &y);
        
        return x;
    }
    
    double getY() const
    {
        double x, y;
        
        cairo_get_current_point(mContext, &x, &y);
        
        return y;
    }
   
    void startMultiLine(double x, double y, double thickness)
    {
        mGraphics->PathStart();
        setLineThickness(thickness);
        mGraphics->PathMoveTo(x, y);
    }
    
    void continueMultiLine(double x, double y)
    {
        mGraphics->PathLineTo(x, y);
    }
    
    void finishMultiLine()
    {
        stroke(cairo_get_line_width(mContext));
    }
    
    void circleIntersection(double cx, double cy, double ang, double r, double *retX, double *retY)
    {
        *retX = cos(2.0 * ang * PI) * r + cx;
        *retY = sin(2.0 * ang * PI) * r + cy;
    }
    
    void frameArc(double cx, double cy, double r, double begAng, double arcAng, double thickness)
    {
        setLineThickness(thickness);
        arc(cx, cy, r, begAng, arcAng);
        stroke(thickness);
    }
    
    void fillArc(double cx, double cy, double r, double begAng, double arcAng)
    {
        mGraphics->PathStart();
        arc(cx, cy, r, begAng, arcAng);
        mGraphics->PathLineTo(cx, cy);
        mGraphics->PathClose();
        fill();
    }

    void fillCircle(double cx, double cy, double r)
    {
        fillArc(cx, cy, r, 0.0, 1.0);
    }
    
    void frameCircle(double cx, double cy, double r, double thickness)
    {
        frameArc(cx, cy, r, 0.0, 1.0, thickness);
    }
    
    void frameTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        triangle(x1, y1, x2, y2, x3, y3);
        stroke(true);
    }
    
    void fillTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        triangle(x1, y1, x2, y2, x3, y3);
        fill(true);
    }
    
    void fillRect(double x, double y, double w, double h)
    {
        rectangle(x, y, w, h);
        fill();
    }
    
    void frameRect(double x, double y, double w, double h, double thickness)
    {
        setLineThickness(thickness);
        rectangle(x, y, w, h);
        stroke(thickness);
    }
    
    void fillRoundRect(double x, double y, double w, double h, double rtl, double rtr, double rbl, double rbr)
    {
        roundedRectangle(x, y, w, h, rtl, rtr, rbl, rbr);
        fill();
    }
    
    void frameRoundRect(double x, double y, double w, double h, double rtl, double rtr, double rbl, double rbr, double thickness)
    {
        setLineThickness(thickness);
        roundedRectangle(x, y, w, h, rtl, rtr, rbl, rbr);
        stroke(thickness);
    }
    
    void fillRoundRect(double x, double y, double w, double h, double r)
    {
        fillRoundRect(x, y, w, h, r, r, r, r);
    }
    
    void frameRoundRect(double x, double y, double w, double h, double r, double thickness)
    {
        frameRoundRect(x, y, w, h, r, r, r, r, thickness);
    }

    void fillCPointer(double cx, double cy, double r, double pr, double ang, double pAng)
    {
        cPointer(cx, cy, r, pr, ang, pAng);
        fill();
    }
    
    void frameCPointer(double cx, double cy, double r, double pr, double ang, double pAng, double thickness)
    {
        setLineThickness(thickness);
        cPointer(cx, cy, r, pr, ang, pAng);
        stroke(thickness);
    }
    
    void line(double x1, double y1, double x2, double y2, double thickness)
    {
        mGraphics->PathLine(x1, y1, x2, y2);
        stroke(thickness, true);
    }
    
    void text(HISSTools_Text *pTxt, const char *str, double x, double y, double w, double h, HTextAlign hAlign = kHAlignCenter, VTextAlign vAlign = kVAlignCenter)
    {
        bool useCairoText = true;
        
        if (useCairoText)
        {
            HISSTools_Color color = mColor->getColor();
            IColor textColor(color.a * 255.0, color.r * 255.0, color.g * 255.0, color.b * 255.0);
            
            IText textSpec(pTxt->mSize, textColor, pTxt->mFont, (IText::EStyle) pTxt->mStyle, (IText::EAlign) hAlign, (IText::EVAlign) vAlign, 0, IText::kQualityAntiAliased);
            IRECT rect(x, y, x + w, y + h);
            mGraphics->DrawText(textSpec, str, rect);
            
            updateDrawBounds(floor(x), ceil(x + w) - 1, floor(y), ceil(y + h) - 1, true);
            return;
        }
        
        LICE_IBitmap *bitmap = mTextBitmap;
        
        int width = mWidth * mScale;
        int height = mHeight * mScale;
        
        // This allows the window to be any size...
        
        width = (width + 3 ) &~ 3;

        if (!bitmap || bitmap->getWidth() != width || bitmap->getHeight() != height)
        {
            delete mTextBitmap;
            bitmap = mTextBitmap = new LICE_SysBitmap(width, height);
        }
        
        LICE_Clear(bitmap, 0);
        HISSTools_LICE_Text::text(bitmap, pTxt, str, x, y, w, h, mScale, hAlign, vAlign);
        
        updateDrawBounds(floor(x), ceil(x + w) - 1, floor(y), ceil(y + h) - 1, true);

        HISSTools_Bounds clip(x, y, w, h);
        
        cairo_save(mContext);
        setClip(clip);
        int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
        cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char *) bitmap->getBits(), CAIRO_FORMAT_ARGB32, width, height, stride);
        cairo_scale(mContext, 1.0/mScale, 1.0/mScale);
        cairo_mask_surface(mContext, surface, 0, 0);
        cairo_restore(mContext);
        cairo_surface_destroy(surface);
    }

    static double getTextLineHeight(HISSTools_Text *pTxt)
    {
        return HISSTools_LICE_Text::getTextLineHeight(pTxt);
    }
    
    void startShadow(HISSTools_Shadow *shadow)
    {
        double x1, x2, y1, y2;

        mShadow = shadow;
        mDrawArea = HISSTools_Bounds();
        cairo_save(mContext);
        cairo_clip_extents(mContext, &x1, &y1, &x2, &y2);
        HISSTools_Bounds clip(x1, y1, x2 - x1, y2 - y1);
        double enlargeBy = (shadow->getBlurSize() + 2) * 2;
        clip.include(HISSTools_Bounds(clip.mRECT.L - shadow->getXOffset(), clip.mRECT.T - shadow->getYOffset(), clip.mRECT.W() + enlargeBy, clip.mRECT.W() + enlargeBy));
        cairo_reset_clip(mContext);
        setClip(clip);
        cairo_push_group(mContext);
    }
    
    void renderShadow(bool renderImage = true)
    {
        // Sanity Check
        
        if (mDrawArea.mRECT.Empty())
            return;
        
        cairo_pattern_t *shadowRender = cairo_pop_group(mContext);
        cairo_restore(mContext);
        
        // Check there is a shadow specified (otherwise only render original image)
        
        if (mShadow)
        {
            mShadow->setScaling(mScale);
            
            int kernelSize = mShadow->getKernelSize();
            
            HISSTools_Bounds bounds = mDrawArea;
            bounds.mRECT.Scale(mScale);
            IRECT draw = bounds.iBounds();
           
            int width = (draw.R - draw.L) + (2 * kernelSize - 1);
            int height = (draw.B - draw.T) + (2 * kernelSize - 1);
            int alphaSurfaceStride = cairo_format_stride_for_width(CAIRO_FORMAT_A8, width);
            
            // Alloc and zero blur memory
            
            mBlurTempAlpha1.resize(alphaSurfaceStride * height);
            mBlurTempAlpha2.resize(alphaSurfaceStride * height);
            std::fill(mBlurTempAlpha1.begin(), mBlurTempAlpha1.end(), 0);
            std::fill(mBlurTempAlpha2.begin(), mBlurTempAlpha2.end(), 0);
            
            // Copy alpha values from the temp bitmap
            
            cairo_surface_t *mask = cairo_image_surface_create_for_data(&mBlurTempAlpha1[0], CAIRO_FORMAT_A8, width, height, alphaSurfaceStride);
            cairo_t* maskContext = cairo_create(mask);
            cairo_scale(maskContext, mScale, mScale);
            cairo_translate(maskContext, (kernelSize - 1 - draw.L) / mScale, (kernelSize - 1 - draw.T) / mScale);
            cairo_set_source(maskContext, shadowRender);
            cairo_paint(maskContext);
            cairo_destroy(maskContext);
            
            // Do blur
            
            mShadow->blur(&mBlurTempAlpha1[0], &mBlurTempAlpha2[0], width, height, alphaSurfaceStride);
            
            // Draw shadow in correct place and color
            
            cairo_save(mContext);
            mShadow->getShadowColor()->setAsSource(mContext);
            cairo_scale(mContext, 1.0/mScale, 1.0/mScale);
            cairo_mask_surface(mContext, mask, mShadow->getXOffset() * mScale + ((draw.L - (kernelSize - 1))), mShadow->getYOffset() * mScale + ((draw.T - (kernelSize - 1))));
            cairo_restore(mContext);
            cairo_surface_destroy(mask);
        }
        
        // Render pattern
        
        if (renderImage)
        {
            cairo_set_source(mContext, shadowRender);
            cairo_paint_with_alpha(mContext, 1.0);
        }
        
        cairo_pattern_destroy(shadowRender);
    }
    
private:
    
    void fill(bool useExtents = false)
    {
        updateDrawBounds(true, useExtents);
        cairo_fill(mContext);
    }
    
    void stroke(double thickness, bool useExtents = false)
    {
        updateDrawBounds(false, useExtents);
        setLineThickness(thickness);
        cairo_stroke(mContext);
    }
    
    double sanitizeRadius(double r, double w, double h)
    {
        r = r < 0 ? 0 : r;
        r = ((r * 2.0) > w) ? w / 2.0: r;
        r = ((r * 2.0) > h) ? h / 2.0: r;
        
        return r;
    }

    void setLineThickness(double thickness)
    {
        cairo_set_line_width(mContext, thickness);
    }

    void arc(double cx, double cy, double r, double begAng, double arcAng)
    {
        begAng = begAng * 360.0;
        arcAng = begAng + (arcAng * 360.0);

        mGraphics->PathArc(cx, cy, r, std::min(begAng, arcAng), std::max(arcAng, begAng));
        setShapeGradient(cx - r, cx + r, cy - r, cy + r);
    }
    
    void rectangle(double x, double y, double w, double h)
    {
        mGraphics->PathRect(IRECT(x, y, x + w, y + h));
        setShapeGradient(x, x + w, y, y + h);
    }
    
    void roundedRectangle(double x, double y, double w, double h, double rtl, double rtr, double rbl, double rbr)
    {
        rtl = sanitizeRadius(rtl, w, h);
        rtr = sanitizeRadius(rtr, w, h);
        rbl = sanitizeRadius(rbl, w, h);
        rbr = sanitizeRadius(rbr, w, h);
        
        mGraphics->PathStart();
        mGraphics->PathArc(x + rtl, y + rtl, rtl, 180.0,  270.0);
        mGraphics->PathArc(x + w - rtr, y + rtr, rtr, 270.0,  360.0);
        mGraphics->PathArc(x + w - rbr, y + h - rbr, rbr, 0.0, 90.0);
        mGraphics->PathArc(x + rbl, y + h - rbl, rbl, 90.0, 180.0);
        mGraphics->PathClose();
        setShapeGradient(x, x + w, y, y + h);
    }
    
    void cPointer(double cx, double cy, double r, double pr, double ang, double pAng)
    {
        double xx = cx + cos(2.0 * PI * ang) * pr;
        double yy = cy + sin(2.0 * PI * ang) * pr;
        
        double begAng = (ang - pAng) * 360.0;
        double arcAng = (pAng * 2.0 * 360.0) + begAng;
        
        mGraphics->PathStart();
        mGraphics->PathArc(cx, cy, r, arcAng, begAng);
        mGraphics->PathLineTo(xx, yy);
        mGraphics->PathClose();

        // FIX - revise...
        setShapeGradient(cx - pr, cx + pr, cy - pr, cy + pr);
    }
    
    void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        mGraphics->PathTriangle(x1, y1, x2, y2, x3, y3);
    }
    
    void updateDrawBounds(bool fill, bool useExtents)
    {
        double xLo, xHi, yLo, yHi;
        
        if (fill)
            cairo_fill_extents(mContext, &xLo, &yLo, &xHi, &yHi);
        else
            cairo_stroke_extents(mContext, &xLo, &yLo, &xHi, &yHi);
        
        updateDrawBounds(xLo, xHi, yLo, yHi, useExtents);
    }
    
    void updateDrawBounds(double xLo, double xHi, double yLo, double yHi, bool useExtents)
    {
        // FIX - account for clip!
        
        mDrawArea.include(HISSTools_Bounds(xLo, yLo, xHi - xLo, yHi - yLo));
        
        if (useExtents)
            setShapeGradient(xLo, xHi, yLo, yHi);
    }
    
    void setShapeGradient(double xLo, double xHi, double yLo, double yHi)
    {
        if (mForceGradientBox)
            mColor->setRect(mGradientArea.x1, mGradientArea.x2, mGradientArea.y1, mGradientArea.y2, mCSOrientation);
        else
            mColor->setRect(xLo, xHi, yLo, yHi, mCSOrientation);

        setColor(mColor);
    }
    
    IGraphics* mGraphics;
    
    cairo_t *mContext;
    
    LICE_SysBitmap *mTextBitmap;
    
    // Boundaries
    
    int mWidth, mHeight;
    HISSTools_Bounds mDrawArea;
    Area mGradientArea;
    
    // Forced Gradient Bounds Flag
    
    bool mForceGradientBox;
    ColorOrientation mCSOrientation;
    
    // Color
    
    HISSTools_Color_Spec *mColor;
    
    // Shadow
    
    HISSTools_Shadow *mShadow;
    
    std::vector <unsigned char> mBlurTempAlpha1;
    std::vector <unsigned char> mBlurTempAlpha2;
    
    double mScale;
};

#endif /* __HISSTOOLS_VECLIB__ */
