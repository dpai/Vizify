#ifndef MRVIZIFYMAIN_H
#define MRVIZIFYMAIN_H

/** C++ Includes **/
#include <memory>
#include <stack>

/** Qt Includes **/
#include <QMainWindow>
#include <QAbstractItemModel>
#include <QPushButton>


class CITK;
class CVTKWidget;
class CMrVizThreshold;

namespace Ui {
class MrVizifyMain;
}

class MrVizifyMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit MrVizifyMain(QWidget *parent = 0);
    ~MrVizifyMain();

private slots:
    void on_pBtn_LoadDicom_clicked();

    void on_pBtn_ViewImage_clicked();

    void on_pBtn_Threshold_clicked();

    void on_pBtn_LoadData_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MrVizifyMain			            *ui;
    std::unique_ptr<CITK>				     m_itk;
    CVTKWidget					            *m_vtkwidget;
    std::unique_ptr<CMrVizThreshold>         m_ThresholdWidget;
    bool						             m_viewLoaded;
    std::stack<QPushButton*>	             m_ModuleBtns;
    std::map<unsigned short int, QWidget*>   m_Modules;

private:
    void LinkInputModels(QAbstractItemModel *);

    void SetupModuleButtons();

    bool RegisterModule(QWidget *);

    void Display();

};

#endif // MRVIZIFYMAIN_H
