/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlModificator.cpp                                                                        */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Shape modificators for filters                                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlModificator.cpp
 * \brief   Shape modificators for filters
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
*/

/*#include "HdlModificator.hpp"

using namespace Glip::CorePipeline;

/// Methods
    HdlModificator::HdlModificator(void)
    {
        rotate = 0.0;
        hscale = vscale = 1.0;
    }

    HdlModificator::HdlModificator(const HdlModificator& m)
    {
        copyModification(m);
    }

    void HdlModificator::copyModification(const HdlModificator& m)
    {
        rotate = m.getRotation();
        hscale = m.getHorizontalScale();
        vscale = m.getVerticalScale();
    }

    float HdlModificator::getRotation(void) const
    {
        return rotate;
    }

    float HdlModificator::getHorizontalScale(void) const
    {
        return hscale;
    }

    float HdlModificator::getVerticalScale(void) const
    {
        return vscale;
    }

    void HdlModificator::setRotation(float r)
    {
        rotate = r;
    }

    void HdlModificator::setScale(float h, float v)
    {
        hscale = h;
        vscale = v;
    }

    void HdlModificator::setFlip(bool h, bool v)
    {
        hscale *= -1.0;
        vscale *= -1.0;
    }

    void HdlModificator::applyModification(void)
    {
        glRotatef(rotate, 0.0, 0.0, 1.0);
        glScalef(hscale, vscale, 1.0);
    }

*/
