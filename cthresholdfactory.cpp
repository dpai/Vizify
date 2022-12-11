#include "cthresholdfactory.h"
#include "cthresholdobject.h"
#include "thresholdobjects.h"

CThresholdObject *CThresholdFactory::CreateThresholdObject(Algorithms type) const
{
    switch(static_cast<Algorithms>(type))
    {
    case Algorithms::Otsu:  /** Otsu Threshold **/
        {
            return COtsuThresholdObject::CreateNew();
            break;
        }
    case Algorithms::Triangle: /** Triangle Threshold **/
        {
            return CTriangleThresholdObject::CreateNew();
            break;
        }
    default: /** Should Never Reach Here **/
        {
            std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
            std::cerr << "Unknown Threshold Object Selection" << std::endl ;
        }
    }

    return 0;
}
