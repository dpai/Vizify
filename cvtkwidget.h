#pragma once

#include <memory>

/** VTK Includes **/
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageData.h>
#include <vtkImageBlend.h>
#include <vtkImageProperty.h>
#include <vtkFileOutputWindow.h>
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkPropPicker.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRendererCollection.h>
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageTracerWidget.h>
#include <vtkImageStencilToImage.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageCast.h>
#include <vtkVersion.h>


class vtkImageInteractionCallback;

/** CVTKWidget: A VTK Widget for Viewing . THe class decorates the QVTKOpenGLNativeWidget class defined in the UI **/

class CVTKWidget final : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
private:
    explicit CVTKWidget(QWidget *parent = nullptr);

    enum {
        X = 0, Y = 1, Z = 2
    };

/** Data **/
private:
    QVTKOpenGLNativeWidget *m_vtkWidget;
    // VTK Specific
    vtkSmartPointer<vtkRenderer> m_MPRRenderer;
    vtkSmartPointer<vtkRenderWindowInteractor> m_MPRRenderInteractor;
    vtkSmartPointer<vtkInteractorStyleImage> m_RenderStyle;
    vtkSmartPointer<vtkImageInteractionCallback> m_EventCallback;
    vtkSmartPointer<vtkImageData> m_ImageData;
    vtkSmartPointer<vtkImageActor> m_ImageActor;
    vtkSmartPointer<vtkImageReslice> m_ImageReslice;
    vtkSmartPointer<vtkWindowLevelLookupTable> m_Lut;
    //vtkSmartPointer<vtkImageData> m_OverlayData;
    vtkSmartPointer<vtkImageActor> m_OverlayActor;
    vtkSmartPointer<vtkPropPicker> m_PixelPicker;
    vtkSmartPointer<vtkCornerAnnotation> m_CornerAnnotation;
    double m_XYImageOrientation[2];
    int m_Direction;

/** Function Members (Private) **/
private:
    void MakeMPRImage(double *);

/** Function Members (Public) **/
public:
    static CVTKWidget* CreateNew() {
        return new CVTKWidget();
    }

    void SetVTKWidget(QVTKOpenGLNativeWidget *pWidget);

    void Render();

    void SetInput(const vtkSmartPointer<vtkImageData>& cImage);

    void SetOverLay(const vtkSmartPointer<vtkImageData>& cImage);

    void CleanPipeline();

    void SetXYImageOrientation(double *dir);

    static double* GetPixelValueAtLocation(double, double, double, vtkImageData *Master, vtkImageData *Slave, bool);

private:

    void ResetCamera();

    void MakePointSphere(double, double, double, int);

signals:

public slots:

};

/** vtkImageInteractionCallback - Class defined for handling intereactions in QVTKOpenGLNativeWidget **/

class vtkImageInteractionCallback final : public vtkCommand
{
public:
    static vtkImageInteractionCallback *New() {
        return new vtkImageInteractionCallback;
    }

    void SetImageReslice(vtkImageReslice *cReslice) {
        this->m_ImageReslice = cReslice;
    }

    void SetImageActor(vtkImageActor *cImageActor) {
        this->m_ImageActor = cImageActor;
    }

    void SetLookupTable(vtkWindowLevelLookupTable *cLut) {
        this->m_LutTable = cLut;
    }

    void SetInteractor(vtkRenderWindowInteractor *cInteractor) {
        this->m_Interactor = cInteractor;
    }

    vtkImageReslice *GetImageReslice() {
        return m_ImageReslice;
    }

    vtkRenderWindowInteractor *GetInteractor()
    {
        return m_Interactor;
    }

    void SetWindowLevel(double fWindow, double fLevel) {
        this->m_WindowValue = fWindow;
        this->m_LevelValue = fLevel;
    }

    double GetWindowValue() {
        return m_WindowValue;
    }

    double GetLevelValue() {
        return m_LevelValue;
    }

    void SetTraits(double fCurSlice, int iMaxSlice, int iMinSlice, int iOffset, int iOrientation) {
        this->m_CurSliceValue = fCurSlice;
        this->m_MaxSliceValue = iMaxSlice;
        this->m_MinSliceValue = iMinSlice;
        this->m_PlaneOrientation = iOrientation;
        this->m_Offset = iOffset;
    }

    void SetAccumalate(int val) {
        m_AcculmalateValue = val;
    }

    void SetPixelPicker(vtkPropPicker *p) {
    m_PixelPicker = p;
    }

    void SetCornerAnnotation(vtkCornerAnnotation *c) {
        m_CornerAnnotation = c;
    }

    void SetSlaveReslice(vtkImageReslice *r) {
        m_SlaveImageReslice = r;
    }

    virtual void Execute(vtkObject *, unsigned long, void *);

    vtkImageData* GetTraceData();

private:
    vtkImageReslice *m_ImageReslice;
    vtkImageReslice *m_SlaveImageReslice;
    vtkImageActor *m_ImageActor;
    vtkWindowLevelLookupTable *m_LutTable;
    vtkRenderWindowInteractor *m_Interactor;
    vtkPropPicker *m_PixelPicker;
    vtkCornerAnnotation *m_CornerAnnotation;
    vtkSmartPointer<vtkImageTracerWidget> m_ImageTracer;
    vtkSmartPointer<vtkImageData> m_ImageTraceData;
    double m_WindowValue;
    double m_LevelValue;
    double m_CurSliceValue;
    int m_MinSliceValue;
    int m_MaxSliceValue;
    int m_PlaneOrientation;
    int m_Offset;
    int m_AcculmalateValue;
    int m_Slicing;
    int m_WindowLevel;
    int m_SliceIndex;

private:
    vtkImageInteractionCallback();
    vtkImageInteractionCallback(const vtkImageInteractionCallback& rhs);
    vtkImageInteractionCallback& operator =(const vtkImageInteractionCallback& rhs);

    void MakeStencilOutput(vtkImageData *);
#if VTK_MAJOR_VERSION > 5
    void UpdateImageActor(vtkImageReslice *);
#endif
};
