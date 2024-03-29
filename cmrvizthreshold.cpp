#include "cmrvizthreshold.h"
#include "ui_cmrvizthreshold.h"
#include "cthresholdfactory.h"
#include "cthresholdobject.h"
#include "cvtkwidget.h"
#include "citk.h"

#include <iostream>

CMrVizThreshold::~CMrVizThreshold() = default;

CMrVizThreshold::CMrVizThreshold(QAbstractItemModel *model, CITK* const c_itk, QWidget *parent) :
QWidget(parent),
ui(std::make_unique<Ui::CMrVizThreshold>()),
m_ThresholdObject(nullptr),
m_itk(c_itk)
{
    ui->setupUi(this);
    ui->cBox_InputImg->setModel(model);

    m_inputvtkwidget = std::unique_ptr<CVTKWidget>(CVTKWidget::CreateNew());
    m_inputvtkwidget->SetVTKWidget(ui->qWidget_InputView);

    m_thresholdvtkwidget = std::unique_ptr<CVTKWidget>(CVTKWidget::CreateNew());
    m_thresholdvtkwidget->SetVTKWidget(ui->qWidget_OutputView);

    /** Supported Threshold Algorithms **/
    ui->cBox_ThreshAlg->addItem("Otsu", static_cast<int>(Algorithms::Otsu));
    ui->cBox_ThreshAlg->addItem("Triangle", static_cast<int>(Algorithms::Triangle));
}

void CMrVizThreshold::Init()
{
    if (m_itk->isDataLoaded())
    {
        m_inputvtkwidget->SetInput(m_itk->GetVTKData<InputImageType>(m_itk->GetImage(ui->cBox_InputImg->currentText()))) ;
        m_inputvtkwidget->Render();
    }
    m_thresholdvtkwidget->CleanPipeline();

    ui->lEdit_Threshlevels->setText("1");
}

void CMrVizThreshold::ProcessThreshold()
{
    /** Initializations **/
    auto outputKey = ui->cBox_OutputImg->currentText();
    auto level = ui->lEdit_Threshlevels->text().toInt();

    m_InputProcessedImageName = ui->cBox_InputImg->currentText();

    /** Retrieve the selected Threshold Algorithm **/
    m_ThresholdObject = std::unique_ptr<CThresholdObject>(CThresholdFactory::CreateThresholdObject(static_cast<Algorithms>(ui->cBox_ThreshAlg->itemData(ui->cBox_ThreshAlg->currentIndex()).toInt())));
    /** Set the Input Image **/
    m_ThresholdObject->SetInput(m_itk->GetImage(m_InputProcessedImageName));

    /** Sanity Check for Level Value **/
    if ( (level < 1) || (level > 4) )
    {
        ui->lEdit_Threshlevels->setText("1");
        level = 1;
    }

    m_ThresholdObject->SetThresholdLevels(level);

    /** Threshold Algorithm Fucntion Call **/
    if ( m_ThresholdObject->Process() == -1)
    {
        return;
    }

    /** Sanity Check for Output Name **/
    if (outputKey.isEmpty())
    {
        outputKey = "Thresholded";
    }

    ui->cBox_OutputImg->clear();

    /** Add To Output Combo List **/
    outputKey.append(m_ThresholdObject->GetDescription());
    ui->cBox_OutputImg->addItem(outputKey, FINAL_OUTPUT_IMAGE);

    /** Add Image to the Input Images List **/
    m_itk->AddImageToList<OutputImageType>(outputKey, m_ThresholdObject->GetOutput());

    /** Add To Output Combo List **/
    outputKey.append("_Overlay");
    ui->cBox_OutputImg->addItem(outputKey, FINAL_OUTPUT_WITH_OVERLAY);

    /** Visualize the Threshold Output in the Output VTK Widget **/
    m_thresholdvtkwidget->SetInput(m_itk->GetVTKData<OutputImageType>( m_ThresholdObject->GetOutput())) ;
    m_thresholdvtkwidget->Render();
    
}

///// SLOTS Here
void CMrVizThreshold::on_pBtn_Apply_clicked()
{
    /** Button Click - Apply **/
    ProcessThreshold();
}

void CMrVizThreshold::on_cBox_InputImg_activated(int index)
{
    /** Update the input image to perform threshold on **/
    /** Not Checking if Image is present. Checking is performed elsewhere **/
    m_inputvtkwidget->SetInput(m_itk->GetVTKData<InputImageType>(m_itk->GetImage(ui->cBox_InputImg->currentText())));
    m_inputvtkwidget->Render();
}

void CMrVizThreshold::on_cBox_OutputImg_activated(int index)
{
    /** Initializations **/
    auto iChoice = ui->cBox_OutputImg->itemData(index).toInt();

    /** Choice of what Output image to visualize . For Threshold we only have two options **/
    switch(iChoice)
    {
        case FINAL_OUTPUT_IMAGE: /** Final Image **/
        {
            m_thresholdvtkwidget->SetInput( m_itk->GetVTKData<OutputImageType>(m_ThresholdObject->GetOutput())) ;
            m_thresholdvtkwidget->Render();
            break;
        }
        case FINAL_OUTPUT_WITH_OVERLAY: /** Final Image overlaid on the input image **/
        {
            m_thresholdvtkwidget->SetInput(m_itk->GetVTKData<InputImageType>(m_itk->GetImage(m_InputProcessedImageName))) ;
            m_thresholdvtkwidget->Render();
            m_thresholdvtkwidget->SetOverLay(m_itk->GetVTKData<OutputImageType>(m_ThresholdObject->GetOutput()));
            //m_thresholdvtkwidget->SetOverLay( m_itk->GetVTKData<RGBImageType>( m_ThresholdObject->GetRGBOutput() ) ) ;
            break;
        }
        default:
            /** Should never reach here **/
        {
            std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
            std::cerr << "Should never reach here " << std::endl;
            return;
        }
    }
}
