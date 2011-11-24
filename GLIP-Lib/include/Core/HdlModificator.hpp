/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlModificator.hpp                                                                        */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Modificators for filters                                                                  */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlModificator.hpp
 * \brief   Shape modificators for filters
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
*/

#ifndef __HANDLE_MODIFICATOR_INCLUDE__
#define __HANDLE_MODIFICATOR_INCLUDE__

/// Include
	#include "OglInclude.hpp"

namespace Glip
{
    namespace CorePipeline
    {
        /// Structure
            class HdlModificator
            {
                private :
                    // Data :
                        float rotate, hscale, vscale;

                public :
                    // Methods
                        HdlModificator(void);
                        HdlModificator(const HdlModificator& m);

                        void copyModification(const HdlModificator& m);

                        void applyModification(void);

                        float getRotation(void) const;
                        float getHorizontalScale(void) const;
                        float getVerticalScale(void) const;

                        void setRotation(float r);
                        void setScale(float h, float v);
                        void setFlip(bool h, bool v);
            };
    }
}

#endif

