#include "cvtkwidget.h"
#include <sstream>


#define _TRACE

/** Static Initializations - Not Sure if Best Option . Will remove later  **/
static double    axialElements[16] =
          {
            1, 0, 0, 0,            // Row Direction
            0, 1, 0, 0,            // Column Direction
            0, 0, 1, 0,            // Slice Direction
            0, 0, 0, 1 };          // W

static double    coronalElements[16] =
          {
            1, 0, 0, 0,            // Row Direction
            0, 0, 1, 0,            // Column Direction
            0, 1, 0, 0,            // Slice Direction
            0, 0, 0, 1 };          // W

static double    sagittalElements[16] =
          {
            0, 0, 1, 0,            // Row Direction
            1, 0, 0, 0,            // Column Direction
            0, 1, 0, 0,            // Slice Direction
            0, 0, 0, 1 };          // W

#ifdef WIN32
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

CVTKWidget::CVTKWidget(QWidget *parent) :
    QVTKWidget(parent)
{
    /*
     * VTK Setup
     */

    m_vtkWidget = 0;

    m_MPRRenderer          = vtkSmartPointer<vtkRenderer>::New();

    m_MPRRenderer->SetBackground(25.0/255.0,51.0/255.0,76.0/255.0);
    m_RenderStyle          = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_RenderStyle->SetInteractionModeToImage3D();
    m_PixelPicker          = vtkPropPicker::New();
    m_PixelPicker->PickFromListOn();
    m_CornerAnnotation     = vtkSmartPointer<vtkCornerAnnotation>::New();
    m_CornerAnnotation->SetLinearFontScaleFactor( 2 );
    m_CornerAnnotation->SetNonlinearFontScaleFactor( 1 );
    m_CornerAnnotation->SetMaximumFontSize( 12 );
    m_CornerAnnotation->GetTextProperty()->SetColor( 1,0,0);
    m_CornerAnnotation->GetTextProperty()->BoldOn();

    m_Direction = Z;

    m_ImageData = 0;

    vtkFileOutputWindow *win = vtkFileOutputWindow::New();
    win->SetFileName("Error.log");
    win->SetInstance(win);
}

void CVTKWidget::SetVTKWidget(QVTKWidget *pWidget)
{
    m_vtkWidget = pWidget;
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_MPRRenderer);
    m_MPRRenderInteractor  = m_vtkWidget->GetInteractor();
    m_MPRRenderInteractor->SetInteractorStyle(m_RenderStyle);

    vtkCamera *acamera = m_MPRRenderer->GetActiveCamera();

    acamera->SetFocalPoint(0, 0, 5);
    acamera->SetViewUp(0, -1, 0);
}

void CVTKWidget::MakeMPRImage(double *elements)
{
    /** Declarations **/
    double    icenter[3];
    int       iDims[3];
    double    iSpacing[3];
    double    iOrigin[3];
    double    iDataRange[2];
    double    iLevel;
    double    iWindow;
    int       iCorrect[3];

    std::stringstream iStream;

    /** Retrieve Size and Spacing of the image **/
    m_ImageData->GetDimensions(iDims);
    m_ImageData->GetSpacing(iSpacing);
    m_ImageData->GetOrigin(iOrigin);
    m_ImageData->GetScalarRange(iDataRange);

#ifdef _TRACE
    std::cout << iDims[0] << " " << iDims[1] << " " << iDims[2] << std::endl;
    std::cout << iSpacing[0] <<  " " << iSpacing[1] << " " << iSpacing[2] << std::endl;
    std::cout << m_ImageData->GetOrigin()[0] << " " << m_ImageData->GetOrigin()[1] << " " << m_ImageData->GetOrigin()[2] << std::endl;
#endif

    iStream << "Dims: " << iDims[0] << "," << iDims[1] << "," << iDims[2] << std::endl;
    iStream << "Spacing: " << iSpacing[0] << "," << iSpacing[1] << "," << iSpacing[2] << std::endl;
    iStream << "Scalar Range: " << iDataRange[0] << "," << iDataRange[1] << std::endl;

    /** Get the Center of the 3D Image **/
    icenter[0] = iSpacing[0] * 0.5 * (iDims[0] - 1);
    icenter[1] = iSpacing[1] * 0.5 * (iDims[1] - 1);
    icenter[2] = iSpacing[2] * 0.5 * (iDims[2] - 1);

    /** Setup The Reslice Axis - Axial by Default **/
    vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();
    resliceAxes->DeepCopy( elements );

    /** Set it to the mid slice **/
    resliceAxes->SetElement(0, 3, 0);
    resliceAxes->SetElement(1, 3, 0);

    /** Edge Case: For 1 slice only **/
    //if (iDims[2] == 1)
        resliceAxes->SetElement(2, 3, iOrigin[2]);

    //std::cout << icenter[0] << " " << icenter[1] << " " << icenter[2] << std::endl;


    /** Setup the Reslicer **/
    m_ImageReslice = vtkSmartPointer<vtkImageReslice>::New();
#if VTK_MAJOR_VERSION <= 5
    m_ImageReslice->SetInputConnection(m_ImageData->GetProducerPort());
#else
    m_ImageReslice->SetInputData(m_ImageData);
#endif
    m_ImageReslice->SetOutputDimensionality(2);
    m_ImageReslice->SetResliceAxes(resliceAxes);
    m_ImageReslice->SetInterpolationModeToLinear();

    if (m_Direction == Z) // Axial
    {
        m_ImageReslice->SetOutputExtent(0,iDims[0]-1, 0, iDims[1]-1, 0, iDims[2]-1);
        m_ImageReslice->SetOutputSpacing(iSpacing);
        m_ImageReslice->SetOutputOrigin(iOrigin);
    }
    else if (m_Direction == X) // Sagittal
    {
        m_ImageReslice->SetOutputExtent(0,iDims[1]-1, 0, iDims[2]-1, 0, iDims[0]-1);
        m_ImageReslice->SetOutputSpacing(iSpacing[1], iSpacing[2], iSpacing[0]);
        m_ImageReslice->SetOutputOrigin(iOrigin[1], iOrigin[2], iOrigin[0]);
    }
    else //Coronal
    {
        m_ImageReslice->SetOutputExtent(0,iDims[0]-1, 0, iDims[2]-1, 0, iDims[1]-1);
        m_ImageReslice->SetOutputSpacing(iSpacing[0], iSpacing[2], iSpacing[1]);
        m_ImageReslice->SetOutputOrigin(iOrigin[0], iOrigin[2], iOrigin[1]);
    }

    m_ImageReslice->Update();

    iLevel = ( ( iDataRange[1] - ((iDataRange[0]>0.0)?iDataRange[0]:0.0) ) + iDataRange[0] ) / 2.0;
    iWindow = iDataRange[1] - iDataRange[0];

    /** Setup the Lookup Table based on the Data Range **/
    m_Lut = vtkSmartPointer<vtkWindowLevelLookupTable>::New();
    m_Lut->SetLevel(iLevel);
    m_Lut->SetWindow(iWindow);

	/** Remove Existing Actors **/
    m_PixelPicker->DeletePickList(m_ImageActor);
    m_MPRRenderer->RemoveActor(m_ImageActor);
    m_MPRRenderer->RemoveActor(m_CornerAnnotation);

    /** Setup the Colormap **/
    vtkSmartPointer<vtkImageMapToColors> ImageColors = vtkSmartPointer<vtkImageMapToColors>::New();
    //ImageColors->SetOutputFormatToLuminance();
    ImageColors->SetLookupTable(m_Lut);
    ImageColors->SetInputConnection(m_ImageReslice->GetOutputPort());
    ImageColors->Update();

    /** Setup The Image Actor **/
    m_ImageActor = vtkSmartPointer<vtkImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
    m_ImageActor->SetInput(ImageColors->GetOutput());
#else
    m_ImageActor->SetInputData(ImageColors->GetOutput());
#endif

    /** Setup the Annotator **/ 
    m_CornerAnnotation->SetText(1, iStream.str().c_str());
    m_CornerAnnotation->SetText(2,"Off Image");

    /** Connect Actor to Renderer **/
    m_MPRRenderer->AddViewProp(m_ImageActor);
    m_PixelPicker->AddPickList(m_ImageActor);
    m_MPRRenderer->AddViewProp(m_CornerAnnotation);

	/** Add Custom Interaction to the view **/
    m_EventCallback = vtkSmartPointer<vtkImageInteractionCallback>::New();
    m_EventCallback->SetImageReslice(m_ImageReslice);
    m_EventCallback->SetInteractor(m_MPRRenderInteractor);
    m_EventCallback->SetImageActor(m_ImageActor);
    m_EventCallback->SetWindowLevel(m_Lut->GetWindow(), m_Lut->GetLevel());
    m_EventCallback->SetLookupTable(m_Lut);
    m_EventCallback->SetPixelPicker(m_PixelPicker);
    m_EventCallback->SetCornerAnnotation(m_CornerAnnotation);

    m_RenderStyle->RemoveAllObservers();
    m_RenderStyle->AddObserver(vtkCommand::MouseMoveEvent, m_EventCallback);
    m_RenderStyle->AddObserver(vtkCommand::LeftButtonPressEvent, m_EventCallback);
    m_RenderStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, m_EventCallback);
    m_RenderStyle->AddObserver(vtkCommand::MiddleButtonPressEvent, m_EventCallback);
    m_RenderStyle->AddObserver(vtkCommand::MiddleButtonReleaseEvent, m_EventCallback);
    m_RenderStyle->AddObserver(vtkCommand::KeyPressEvent, m_EventCallback);

    iCorrect[2] = round( (  icenter[2] - fabs(iOrigin[2])  )/iSpacing[2] );
    iCorrect[1] = round( (  icenter[1] - fabs(iOrigin[1])  )/iSpacing[1] );
    iCorrect[0] = round( (  icenter[0] - fabs(iOrigin[0])  )/iSpacing[0] );

#ifdef _DEBUG
    std::cout << "Center is " << icenter[0] << " " << icenter[1] <<  " " << icenter[2] << std::endl;
    std::cout << "iCorrect is " << iCorrect[0] << " " << iCorrect[1] << " " << iCorrect[2] << std::endl;
#endif

    if (m_Direction == Z)      // Axial
    {
        m_EventCallback->SetTraits(iSpacing[2], iDims[2], 1, iCorrect[2], m_Direction);
        m_EventCallback->SetAccumalate((-1)*iOrigin[2]/fabs(iOrigin[2]));
    }
    else if (m_Direction == X) // Sagittal
    {
        m_EventCallback->SetTraits(iSpacing[0], iDims[0], 1, iCorrect[0], m_Direction);
        m_EventCallback->SetAccumalate((-1)*iOrigin[0]/fabs(iOrigin[0]));
    }
    else                        // Coronal
    {
        m_EventCallback->SetTraits(iSpacing[1], iDims[1], 1, iCorrect[1], m_Direction);
        m_EventCallback->SetAccumalate((-1)*iOrigin[1]/fabs(iOrigin[1]));
    }	
	
	/** Reset Camera to display image in Radiological convention  **/ 
    ResetCamera();

    MakePointSphere(iOrigin[0],iOrigin[1],0, 2);
}

/** The Reset Camera will make sure that the slice will be visualizied in radiological convention :- R -> L goes from left to right on the display 
    and A -> P goes from top to bottom **/
void CVTKWidget::ResetCamera()
{
    vtkCamera *aCamera = m_MPRRenderer->GetActiveCamera();
    /**Init**/
    //aCamera->SetViewUp(0,-1,0);
    //aCamera->SetPosition(0,0,1);

    /** m_XYImageOrientation[0] = m_XYImageOrientation[1] = 1  ensures radiological convention **/

    //if ((m_XYImageOrientation[0] < 0) && (m_XYImageOrientation[1] < 0) ) // display L->R , P->A
    //{
    //        aCamera->Roll(180);
    //}
    //else if (m_XYImageOrientation[0] < 0) // display L->R , flip X
    //{
    //        aCamera->Azimuth(180);
    //}
    //else if (m_XYImageOrientation[1] < 0) // display P->A , flip Y
    //{
    //        aCamera->SetViewUp(0,1,0);
    //        aCamera->Elevation(180);
    //}

    std::cout << "Image XY Orientation " << m_XYImageOrientation[0] << " " << m_XYImageOrientation[1] << std::endl;
}

void CVTKWidget::Render()
{
    /** Check if Widget is initialized **/
    if (!m_vtkWidget)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << std::endl;
        std::cerr << "VTK Widget not initialized - Please use setVTKWidget() method " << std::endl;
        return;
    }

    if (m_ImageData == NULL)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "No Input Image to visualize " << std::endl;
		m_MPRRenderer->RemoveAllViewProps();
		m_MPRRenderInteractor->Render();
        return;
    }

    /** Visualize appropirate plane **/
    switch (m_Direction)
    {
    case Z:
            MakeMPRImage(axialElements);
            break;
    case X:
            MakeMPRImage(sagittalElements);
            break;
    case Y:
            MakeMPRImage(coronalElements);
            break;
    default:
            std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
            std::cerr << "Should Never reach here. m_Direction not set " << std::endl;
            return;
    }

    /** VTK Rendering Calls **/
	
    m_MPRRenderer->ResetCamera();
    m_MPRRenderer->ResetCameraClippingRange();
    m_MPRRenderInteractor->Render();
}

void CVTKWidget::SetInput(vtkImageData *cImage)
{
    m_ImageData = vtkSmartPointer<vtkImageData>::Take(cImage);
}

void CVTKWidget::SetXYImageOrientation(double *dir)
{
    m_XYImageOrientation[0] = dir[0];
    m_XYImageOrientation[1] = dir[1];
}

void CVTKWidget::CleanPipeline()
{
    /** Cleanup all actors - Convenience  Function **/
    m_MPRRenderer->RemoveAllViewProps();
    m_MPRRenderInteractor->Render();
    m_ImageData = NULL;
    m_ImageActor = NULL;
    m_ImageReslice = NULL;
    m_Lut = NULL;
}

void CVTKWidget::SetOverLay(vtkImageData *cImage)
{
	/** Check if Image is Loaded **/
    if (!m_ImageData)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "Input should be set before the overlay" << std::endl;
        return;
    }

    vtkSmartPointer<vtkImageData> cImageData = vtkSmartPointer<vtkImageData>::Take(cImage);

    /** Testing **/
    vtkSmartPointer<vtkWindowLevelLookupTable> cLut = vtkSmartPointer<vtkWindowLevelLookupTable>::New();
    cLut->DeepCopy(m_Lut);
    cLut->SetAlpha(0.5);
    cLut->Build();

    /** Setup the Colormap **/
    vtkSmartPointer<vtkImageMapToColors> ImageColors = vtkSmartPointer<vtkImageMapToColors>::New();
    ImageColors->SetLookupTable(cLut);
    ImageColors->SetInputConnection(m_ImageReslice->GetOutputPort());

    /** Generate a Reslice **/
    vtkSmartPointer<vtkImageReslice> cReslice = vtkSmartPointer<vtkImageReslice>::New();
#if VTK_MAJOR_VERSION <= 5
    cReslice->SetInputConnection(cImageData->GetProducerPort());
#else
    cReslice->SetInputData(cImageData);
#endif
    cReslice->SetOutputDimensionality(2);
    cReslice->SetResliceAxes(m_ImageReslice->GetResliceAxes());
    cReslice->SetInterpolationModeToLinear();

	/** Blend the original and secondary Volume as overlay **/
    vtkSmartPointer<vtkImageBlend> blend = vtkSmartPointer<vtkImageBlend>::New();
#if VTK_MAJOR_VERSION <= 5
    blend->AddInput(cReslice->GetOutput());
    blend->AddInput(ImageColors->GetOutput());
#else
    blend->AddInputData(cReslice->GetOutput());
    blend->AddInputData(ImageColors->GetOutput());
#endif
    blend->Update();

    /** Retrieve VTK Image Data **/
    //m_OverlayData = vtkSmartPointer<vtkImageData>::New();
    //m_OverlayData->DeepCopy(blend->GetOutput());

    /** Setup the Overlay Actor **/
    m_MPRRenderer->RemoveViewProp(m_OverlayActor);
    m_OverlayActor = vtkSmartPointer<vtkImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
    m_OverlayActor->SetInput(blend->GetOutput());
#else
    m_OverlayActor->SetInputData(blend->GetOutput());
#endif
    m_OverlayActor->GetProperty()->SetOpacity(1.0);

    /** Add to REnderer **/
    m_MPRRenderer->RemoveViewProp(m_ImageActor);
    m_MPRRenderer->AddViewProp(m_OverlayActor);

    m_EventCallback->SetImageReslice(cReslice);
    m_EventCallback->SetLookupTable(cLut);

    m_MPRRenderInteractor->Render();
}

void CVTKWidget::MakePointSphere(double X, double Y, double Z, int radius)
{
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();

    sphereSource->SetCenter(X,Y,Z);
    sphereSource->SetRadius(radius);

    mapper->SetInputConnection(sphereSource->GetOutputPort());

    sphereActor->SetMapper(mapper);

    m_MPRRenderer->AddViewProp(sphereActor);
}

double* CVTKWidget::GetPixelValueAtLocation(double pX, double pY, double pZ, vtkImageData *cImageData, vtkImageData *cImageSlaveData, bool Interpolate)
{
    vtkPointData			*cPointData;
    vtkPointData			*cImagePointData;
    vtkPointData            *cSlavePointData;
    int						 subId;
    double					 pcoords[3], weights[8];
    double					 pos[3];

    cPointData = vtkPointData::New();
    cImagePointData = cImageData->GetPointData();

    cPointData->InterpolateAllocate( cImagePointData, 1, 1 );

    // Use tolerance as a function of size of source data
    double tol2 = cImageData->GetLength();
    tol2 = tol2 ? tol2*tol2 / 1000.0 : 0.001;

    // Find the cell that contains pos
    pos[0] = pX;pos[1] = pY;pos[2] = pZ;

    if (Interpolate)
    {

        vtkCell* cell = cImageData->FindAndGetCell(pos, NULL, -1, tol2, subId, pcoords, weights );

        if( cell )
        {
            // Interpolate the point data
            cPointData->InterpolatePoint( cImagePointData, 0, cell->PointIds, weights );
            int components = cPointData->GetScalars()->GetNumberOfComponents();
            double* tuple = cPointData->GetScalars()->GetTuple( 0 );

            int nIDs = cell->PointIds->GetNumberOfIds();
            std::cout << "Reference Data Interpolate Values " << std::endl;
            for (int k = 0 ; k < 8 ; k++)
                    std::cout << weights[k] <<  " " ;
            std::cout << std::endl;
            if (cImageSlaveData)
            {
                cSlavePointData = cImageSlaveData->GetPointData();
                for (int k = 0 ; k < nIDs ; k++)
                {
                    double *iValue1 = cImagePointData->GetScalars()->GetTuple(cell->PointIds->GetId(k));
                    double *iValue2 = cSlavePointData->GetScalars()->GetTuple(cell->PointIds->GetId(k));
                    std::cout << cell->PointIds->GetId(k) << " " << iValue1[0] << 
                                                                                                    " " << iValue2[0] << 
                                                                                                    " " << fabs(iValue1[0] - iValue2[0]) << std::endl;
                }
            }
            else
                for (int k = 0 ; k < nIDs ; k++)
                    std::cout << cell->PointIds->GetId(k) << " " << cImagePointData->GetScalars()->GetTuple(cell->PointIds->GetId(k))[0] << std::endl;

            return tuple;
        }
    }
    else
    {
        int cId = cImageData->FindPoint(pos);
        if (cImageSlaveData)
        {
            cSlavePointData = cImageSlaveData->GetPointData();
            double *iValue1 = cImagePointData->GetScalars()->GetTuple(cId);
            double *iValue2 = cSlavePointData->GetScalars()->GetTuple(cId);
            std::cout << cId <<  " " << iValue1[0] << 
                                                     " " << iValue2[0] << 
                                                     " " << fabs(iValue1[0] - iValue2[0]) << std::endl;
        }
        else
            std::cout << cId << " " << cImagePointData->GetScalars()->GetTuple(cId)[0] << std::endl;

        return cImagePointData->GetScalars()->GetTuple(cId) ;
    }

    return 0;
}


//////////////////Code for vtkCommand
///
/////////////////////////////////////////
///
/// vtkImageInteractionCallback Functions

vtkImageInteractionCallback::vtkImageInteractionCallback()
{
    this->m_Slicing = 0;
    this->m_WindowLevel = 0;
    this->m_ImageReslice = 0;
    this->m_Interactor = 0;
    this->m_ImageActor = 0;
    this->m_SliceIndex = 0;
    this->m_PixelPicker = 0;
    this->m_CornerAnnotation = 0;
    this->m_SlaveImageReslice = 0;
    this->m_ImageTracer = 0;
    this->m_ImageTraceData = 0;
}

vtkImageInteractionCallback::~vtkImageInteractionCallback()
{
    // All Pointers are owned by VTKCall. No Deletes are needed.
}

void vtkImageInteractionCallback::Execute(vtkObject *cObject, unsigned long cEvent, void *cData)
{
    /** Declarations **/
    int              iLastPos[2];
    int              iCurPos[2];
    double           iDeltaX;
    double           iDeltaY;
    double           fRange[2];
    vtkImageReslice *cReslice;
    double           fSliceSpacing;
    vtkMatrix4x4    *cMatrix;
    double           fCenter[4];
    int              iSize[2];

    /** Initializations **/
    vtkRenderWindowInteractor *cInteractor = this->GetIneteractor();
    cInteractor->GetLastEventPosition(iLastPos);
    cInteractor->GetEventPosition(iCurPos);
	cInteractor->GetSize(iSize);
    double           fPoint[4] = {0.0, 0.0, 0.0, 1.0};

    /** Switch on the different events **/
    switch (cEvent)
    {
        case vtkCommand::MiddleButtonPressEvent: /** Handler for MiddleButton - This Event Handles Window Level **/
        {
           this->m_WindowLevel = 1;
           break;
        }

        case vtkCommand::MiddleButtonReleaseEvent:
        {
           this->m_WindowLevel = 0;
           break;
        }

        case vtkCommand::LeftButtonPressEvent: /** Handler for LeftButton - THis Event Handles Out-of-Plane Slicing **/
        {
           this->m_Slicing = 1;
           break;
        }

        case vtkCommand::LeftButtonReleaseEvent:
        {
           this->m_Slicing = 0;
           break;
        }

        case vtkCommand::MouseMoveEvent: /** Mouse Move events - Handle the new event positions for window level and slicing **/
        {
            if (this->m_WindowLevel) /// Code referenced from vtkInteractorStyleImage 
            {
                cReslice = this->GetImageReslice();
#if VTK_MAJOR_VERSION <= 5
                cReslice->GetOutput()->UpdateInformation();
#endif


                /** Update Window and Level based on mouse movement **/
                iDeltaX = (iCurPos[0] - iLastPos[0]) * 4.0 / iSize[0];
                iDeltaY = (iLastPos[1] - iCurPos[1]) * 4.0 / iSize[1];

                if ( fabs( this->m_WindowValue ) > 0.01 )
                {
                    iDeltaX = iDeltaX * this->m_WindowValue;
                }
                else
                {
                    iDeltaX = iDeltaX * ( this->m_WindowValue < 0 ? -0.01 : 0.01 );
                }

                if ( fabs( this->m_LevelValue ) > 0.01 )
                {
                    iDeltaY = iDeltaY * this->m_LevelValue;
                }
                else
                {
                    iDeltaY = iDeltaY * ( this->m_LevelValue < 0 ? -0.01 : 0.01 );
                }


                if ( this->m_WindowValue < 0.0 )
                {
                    iDeltaX = -1 * iDeltaX;
                }
                if ( this->m_LevelValue < 0.0 )
                {
                    iDeltaY = -1 * iDeltaY;
                }

                /** Update Window and Level Values **/
                m_LevelValue -= iDeltaY ;
                m_WindowValue += iDeltaX;

                /** Window value is not allowed to go negative **/
                if ( m_WindowValue < 0.01 )
                {
                    m_WindowValue = 0.01;
                }
	//////////////////////////////////////////////////////

                m_LutTable->SetLevel(m_LevelValue);
                m_LutTable->SetWindow(m_WindowValue);

                std::stringstream cStr;
                cStr << " Slice: " << m_SliceIndex << std::endl << "[Window,Level]: (" << m_WindowValue << ", " << m_LevelValue << ")" << std::endl;
                m_CornerAnnotation->SetText(0, cStr.str().c_str());
                                 
#if VTK_MAJOR_VERSION > 5
                UpdateImageActor(cReslice);
#endif

                /** Render **/
                m_Interactor->Render();

            } // end if (this->m_WindowLevel)

            else if (this->m_Slicing)
            {
                /** When Number of Slices is 1 **/
                if (m_MaxSliceValue == 1)
                    return;

                cReslice = this->GetImageReslice();
#if VTK_MAJOR_VERSION <= 5
                cReslice->GetOutput()->UpdateInformation();
#endif

                /** Get Out of Plane Spacing **/
                fSliceSpacing = m_CurSliceValue;
                cMatrix = cReslice->GetResliceAxes();

                /** Get Mouse Interaction **/
                iDeltaY = iLastPos[1] - iCurPos[1];
                fPoint[2] = fSliceSpacing *iDeltaY;

                /** Update Slice to Visualize **/
                cMatrix->MultiplyPoint(fPoint,fCenter);

                cMatrix->SetElement(0, 3, fCenter[0]);
                cMatrix->SetElement(1, 3, fCenter[1]);
                cMatrix->SetElement(2, 3, fCenter[2]);

                if (m_PlaneOrientation == 2)          // Axial
                    m_SliceIndex = round(fCenter[2]/m_CurSliceValue) + ((m_AcculmalateValue)*(m_MaxSliceValue/2 - m_Offset));
                else if (m_PlaneOrientation == 0)     // Sagittal
                    m_SliceIndex = round(fCenter[0]/m_CurSliceValue) + ((m_AcculmalateValue)*(m_MaxSliceValue/2 - m_Offset));
                else                                  // Coronal
                    m_SliceIndex = round(fCenter[1]/m_CurSliceValue) + ((m_AcculmalateValue)*(m_MaxSliceValue/2 - m_Offset));

                cReslice->Update();

                /** Get the scalar range to map the new Window and Level values **/
                cReslice->GetOutput()->GetScalarRange(fRange);

                m_WindowValue = fRange[1] - fRange[0];

                m_LevelValue = (fRange[1] - fRange[0])/2.0;

                m_LutTable->SetWindow(m_WindowValue);
                m_LutTable->SetLevel(m_LevelValue);

                /** Visualize values in the view **/
                std::stringstream cStr;
                cStr << " Slice: " << m_SliceIndex << std::endl << "[Window,Level]: (" << m_WindowValue << ", " << m_LevelValue << ")" << std::endl;
                m_CornerAnnotation->SetText(0, cStr.str().c_str());
#if VTK_MAJOR_VERSION > 5                                
                UpdateImageActor(cReslice);
#endif
                                

                /** Render **/
                m_Interactor->Render();

            } // end if (this->m_Slicing)

            else
            {
                /** All Other Events - Default Interactor Style **/
                vtkInteractorStyleImage *iStyle = vtkInteractorStyleImage::SafeDownCast(m_Interactor->GetInteractorStyle());

                if (iStyle)
                {
                    //
                    vtkRenderer *iRender = m_Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
                    if (!iRender)
                        return;

                    vtkPointData *cImagePointData = m_ImageReslice->GetOutput()->GetPointData();

                    if (m_PixelPicker && cImagePointData)
                    {
                        m_PixelPicker->Pick( m_Interactor->GetEventPosition()[0], m_Interactor->GetEventPosition()[1], 0.0, iRender);

                        if (m_ImageActor == vtkImageActor::SafeDownCast(m_PixelPicker->GetViewProp()))
                        {
                            double pos[3];
                            m_PixelPicker->GetPickPosition(pos);
                            //pos[0] = 32.7;
                            //pos[1] = 17.3;
                            pos[2] = 0;

                            double *tuple = CVTKWidget::GetPixelValueAtLocation(pos[0],pos[1],pos[2], m_ImageReslice->GetOutput(), m_SlaveImageReslice?(m_SlaveImageReslice->GetOutput()):0, false);

                            if (tuple)
                            {
                                std::string message = "Location: ( ";
                                message += vtkVariant( pos[0] ).ToString();
                                message += ", ";
                                message += vtkVariant( pos[1] ).ToString();
                                message += ", ";
                                message += vtkVariant( pos[2] ).ToString();
                                message += " )\nValue: ( ";
                                message += vtkVariant( tuple[ 0 ] ).ToString();
                                message += ")";
                                m_CornerAnnotation->SetText(2, message.c_str());
                                m_Interactor->Render();
                            }
                        }
                    }
						
                    iStyle->OnMouseMove();
                }
            } // End if

            break;
        } // End Case vtkCommand::MouseMoveEvent

        case vtkCommand::KeyPressEvent:
        {
            /** Reset the Window Level to the default **/
            if (m_Interactor->GetKeyCode() == 'r')  
            {
                cReslice = this->GetImageReslice();
#if VTK_MAJOR_VERSION <= 5
                cReslice->GetOutput()->UpdateInformation();
#endif

                cReslice->GetOutput()->GetScalarRange(fRange);

                m_WindowValue = fRange[1] - fRange[0];

                m_LevelValue = (fRange[1] - fRange[0])/2.0;

                m_LutTable->SetWindow(m_WindowValue);
                m_LutTable->SetLevel(m_LevelValue);

                std::stringstream cStr;
                cStr << " Slice: " << m_SliceIndex << std::endl << "[Window,Level]: (" << m_WindowValue << ", " << m_LevelValue << ")" << std::endl;
                m_CornerAnnotation->SetText(0, cStr.str().c_str());
#if VTK_MAJOR_VERSION > 5                                 
                UpdateImageActor(cReslice);
#endif

                /** Render **/
                m_Interactor->Render();
            }

            if (m_Interactor->GetKeyCode() == 't')
            {
                if (m_Interactor && !m_ImageTracer)
                {
                    m_ImageTracer = vtkSmartPointer<vtkImageTracerWidget>::New();
                    m_ImageTracer->SetInteractor(m_Interactor);
                    m_ImageTracer->SetViewProp(m_ImageActor);
                    m_ImageTracer->AutoCloseOn();
                    m_ImageTracer->SetCaptureRadius(3);
                    m_ImageTracer->AutoCloseOn();
                }

                m_ImageTracer->SetEnabled(!m_ImageTracer->GetEnabled());
            }
				  
            break;
        }

        case vtkCommand::EndPickEvent:
        {
            std::cout << "The pick event fires alright " << std::endl;
        }
    }; // End Switch(Event)

}

void vtkImageInteractionCallback::MakeStencilOutput(vtkImageData *cImageData)
{
    vtkSmartPointer<vtkPolyData> path = vtkSmartPointer<vtkPolyData>::New();
 
    if(!m_ImageTracer->IsClosed())
    {
        std::cout << "Path not closed!" << std::endl;
        m_ImageTraceData = 0;
        return;
    }
 
    m_ImageTracer->GetPath(path);
    std::cout << "There are " << path->GetNumberOfPoints() << " points in the path." << std::endl;

    vtkSmartPointer<vtkPolyDataToImageStencil> polyDataToImageStencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    polyDataToImageStencil->SetTolerance(0);
#if VTK_MAJOR_VERSION <= 5
    polyDataToImageStencil->SetInputConnection(path->GetProducerPort());
#else
    polyDataToImageStencil->SetInputData(path);
#endif
    polyDataToImageStencil->SetOutputOrigin(cImageData->GetOrigin());
    polyDataToImageStencil->SetOutputSpacing(cImageData->GetSpacing());
    polyDataToImageStencil->SetOutputWholeExtent(cImageData->GetExtent());
    polyDataToImageStencil->Update();

    vtkImageStencilToImage *imageStencilToImage = vtkImageStencilToImage::New();
    imageStencilToImage->SetInputConnection(polyDataToImageStencil->GetOutputPort());
    imageStencilToImage->SetInsideValue(255);
    imageStencilToImage->Update();

    vtkSmartPointer<vtkImageCast> castFilter = vtkSmartPointer<vtkImageCast>::New();
    castFilter->SetInputConnection(imageStencilToImage->GetOutputPort());
    castFilter->SetOutputScalarTypeToShort();
    castFilter->Update();

    m_ImageTraceData = vtkSmartPointer<vtkImageData>::New();
    m_ImageTraceData->SetOrigin(cImageData->GetOrigin());
    m_ImageTraceData->SetSpacing(cImageData->GetSpacing());
#if VTK_MAJOR_VERSION <= 5
    m_ImageTraceData->SetWholeExtent(cImageData->GetExtent());
#else
    m_ImageTraceData->SetExtent(cImageData->GetExtent());
#endif
    m_ImageTraceData->DeepCopy(castFilter->GetOutput());
}

vtkImageData * vtkImageInteractionCallback::GetTraceData()
{
    MakeStencilOutput(m_ImageReslice->GetOutput());
    return m_ImageTraceData;
}

#if VTK_MAJOR_VERSION > 5
void vtkImageInteractionCallback::UpdateImageActor(vtkImageReslice* cReslice)
{
    vtkSmartPointer<vtkImageMapToColors> ImageColors = vtkSmartPointer<vtkImageMapToColors>::New();
    ImageColors->SetLookupTable(m_LutTable);
    ImageColors->SetInputConnection(cReslice->GetOutputPort());
    ImageColors->Update();

    m_ImageActor->SetInputData(ImageColors->GetOutput());
    m_ImageActor->Update();
}
#endif