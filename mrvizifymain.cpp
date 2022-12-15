#include "mrvizifymain.h"
#include "ui_mrvizifymain.h"
#include "citk.h"
#include "cvtkwidget.h"
#include "cmrvizthreshold.h"
#include "MrVizifyConfig.h"
//#include "compareimageui.h"

#ifdef USE_CIMGPROC
#include "CImgProcWidget.h"
#endif

#ifdef USE_CIMGCONV
#include "ConverterWidget.h"
#endif

#ifdef USE_CIMGSTAT
#include "StatsWidget.h"
#endif

/** Qt Includes **/
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>

MrVizifyMain::MrVizifyMain(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MrVizifyMain),
m_viewLoaded(false)
{
    ui->setupUi(this);

    // Composition Initializations
    m_itk = std::unique_ptr<CITK>(CITK::CreateNew());
    m_vtkwidget = CVTKWidget::CreateNew();

    m_vtkwidget->SetVTKWidget(ui->wGet_ViewWindow);

    LinkInputModels(ui->cBox_Volumes->model());

    SetupModuleButtons();

    /** Now All chosen modules can be attached one after one **/
#ifdef USE_CIMGPROC
    if (!RegisterModule((QWidget *) new CImgProcWidget(ui->cBox_Volumes->model(), this)))
    {
        std::cout << "Module Not Loaded" << std::endl;
    }
#endif

#ifdef USE_CIMGCONV
    if (!RegisterModule((QWidget *) new Widget(ui->cBox_Volumes->model(), this)))
    {
        std::cout << "Module Not Loaded" << std::endl;
    }
#endif

#ifdef USE_CIMGSTAT
    if (!RegisterModule((QWidget *) new StatWidget(ui->cBox_Volumes->model(), this)))
    {
        std::cout << "Module Not Loaded" << std::endl;
    }
#endif
}

MrVizifyMain::~MrVizifyMain()
{
    delete ui;
    delete m_vtkwidget;
}

void MrVizifyMain::LinkInputModels(QAbstractItemModel *model)
{
    m_itk->SetImageListModel(model);
    m_ThresholdWidget = std::make_unique<CMrVizThreshold>(model, m_itk.get(), this);
    m_ThresholdWidget->setWindowFlags(Qt::Window);
}

void MrVizifyMain::SetupModuleButtons()
{
    /** Make a Stack of all available buttons **/
    m_ModuleBtns.push(ui->pushButton_6);
    m_ModuleBtns.push(ui->pushButton_5);
    m_ModuleBtns.push(ui->pushButton_4);
    //m_ModuleBtns.push(ui->pushButton_3);
    m_ModuleBtns.push(ui->pushButton_2);

    ui->pushButton_2->setProperty("Module",2);
    ui->pushButton_3->setProperty("Module",3);
    ui->pushButton_4->setProperty("Module",4);
    ui->pushButton_5->setProperty("Module",5);
    ui->pushButton_6->setProperty("Module",6);
}

bool MrVizifyMain::RegisterModule(QWidget *lWidget)
{
    if (m_ModuleBtns.size() == 0)
    {
        std::cout << "No buttons available to load Module " << lWidget->objectName().toStdString() << std::endl;
        delete lWidget;
        return false;
    }

    QPushButton *pBtn = m_ModuleBtns.top();
    pBtn->setText(lWidget->objectName());
    m_ModuleBtns.pop();
    m_Modules[pBtn->property("Module").toInt()] = lWidget;
    lWidget->setWindowFlags(Qt::Window);

    return true;
}

void MrVizifyMain::Display()
{
    m_vtkwidget->SetInput( m_itk->GetVTKData<InputImageType>(m_itk->GetImage(ui->cBox_Volumes->currentText()) ) );
    m_vtkwidget->SetXYImageOrientation(m_itk->GetXYImageOrientation());
    m_vtkwidget->Render();
}

void MrVizifyMain::on_pBtn_LoadDicom_clicked()
{
    /**
     * Load DICOM MR into ITK Memory
    **/

    m_itk->LoadDicom();

    if (m_itk->isDataLoaded())
    {
        ui->lEdit_Dicom->setText(m_itk->GetImageDirectory());
    }
    else
    {
        QMessageBox msg;
        msg.setText("No Dicom Images Found.");
        msg.exec();
    }

    /** If an old view is loaded . Refresh to the new image **/
    if (m_viewLoaded)
    {
        Display();
    }
}

void MrVizifyMain::on_pBtn_ViewImage_clicked()
{

    /** Visualize the selected image (if loaded) in the vtkwidget **/
    if (m_itk->isDataLoaded())
    {
        /** Get the image as vtkImageData for visualization **/
        Display();
        m_viewLoaded = true;
    }
}

void MrVizifyMain::on_pBtn_Threshold_clicked()
{
    /** Initialize the threshold module and show the widget in a new window **/
    m_ThresholdWidget->Init();
    m_ThresholdWidget->show();
}

void MrVizifyMain::on_pBtn_LoadData_clicked()
{
    /**
     * Load Data MR into ITK Memory
    **/

    m_itk->LoadData();

    if (m_itk->isDataLoaded())
    {
        ui->lEdit_Dicom->setText(m_itk->GetImageDirectory());
    }
    else
    {
        QMessageBox msg;
        msg.setText("File Not Selected");
        msg.exec();
    }

    /** If an old view is loaded . Refresh to the new image **/
    if (m_viewLoaded)
    {
        Display();
    }

}

void MrVizifyMain::on_pushButton_2_clicked()
{
    /** Open the Module associated with the button **/
    try
    {
        QWidget *widget = m_Modules.at(ui->pushButton_2->property("Module").toInt());
        widget->show();
    }
    catch (std::exception)  // Accessing NULL objects is going to throw . We ignore that error .
    {}
}

void MrVizifyMain::on_pushButton_3_clicked()
{
    // CompareImageUI *cImageCompare = new CompareImageUI(this);
    // cImageCompare->setWindowFlags(Qt::Window);

    // cImageCompare->Init();
    // cImageCompare->show();
}

void MrVizifyMain::on_pushButton_4_clicked()
{
    /** Open the Module associated with the button **/
    try
    {
        QWidget *widget = m_Modules.at(ui->pushButton_4->property("Module").toInt());
        widget->show();
    }
    catch (std::exception)  // Accessing NULL objects is going to throw . We ignore that error .
    {}
}
