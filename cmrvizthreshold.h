#ifndef CMRVIZTHRESHOLD_H
#define CMRVIZTHRESHOLD_H


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
    explicit CMrVizThreshold( QAbstractItemModel *model, QWidget *parent = 0);
    ~CMrVizThreshold();

/** Data **/
private:
    Ui::CMrVizThreshold  *ui;

    CThresholdFactory    *m_ThresholdFactory;

	CVTKWidget           *m_inputvtkwidget;

	CVTKWidget           *m_thresholdvtkwidget;

	CITK			     *const m_itk;

	CThresholdObject     *m_ThresholdObject;

    QString               m_InputProcessedImageName;

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

#endif // CMRVIZTHRESHOLD_H
