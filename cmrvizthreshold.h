#pragma once

/** C++ Includes **/
#include <memory>

/** Qt Includes **/
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>

class CVTKWidget;
class CITK;

namespace Ui {
class CMrVizThreshold;
}

class CThresholdFactory;
class CThresholdObject;

class CMrVizThreshold : public QWidget
{
    Q_OBJECT

/** Constructors and Destructors **/
public:
    explicit CMrVizThreshold(QAbstractItemModel *model, CITK* const c_itk, QWidget *parent = 0);
    ~CMrVizThreshold();

/** Data **/
private:
    QString m_InputProcessedImageName;
    std::unique_ptr<Ui::CMrVizThreshold> ui;
    std::unique_ptr<CVTKWidget> m_inputvtkwidget;
    std::unique_ptr<CVTKWidget> m_thresholdvtkwidget;
    std::unique_ptr<CThresholdObject> m_ThresholdObject;
    CITK *const m_itk;

/** Function Members (Public) **/
public:
    void Init();

/** Qt SLots **/
private slots:
    void on_pBtn_Apply_clicked();

    void on_cBox_InputImg_activated(int index);

    void on_cBox_OutputImg_activated(int index);

/** Function Members (Private) **/
private:
    void ProcessThreshold();
};
