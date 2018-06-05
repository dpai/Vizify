#include "cthresholdfactory.h"
#include "cthresholdobject.h"
#include "thresholdobjects.h"

CThresholdFactory::CThresholdFactory()
{
}

CThresholdFactory::~CThresholdFactory()
{
}

CThresholdObject *CThresholdFactory::CreateThresholdObject(int type)
{
    switch(type)
    {
    case Otsu:  /** Otsu Threshold **/
		{
			return COtsuThresholdObject::CreateNew();
			break;
		}
	case Triangle: /** Triangle Threshold **/
		{
			 return CTriangleThresholdObject::CreateNew();
			break;
		}
	default: /** Should Never Reach Here **/
		{
			std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
			std::cerr << "Unknown THreshold Object Selection" << std::endl ;
		}
    }

	return 0;
}
