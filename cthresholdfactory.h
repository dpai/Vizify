#ifndef CTHRESHOLDFACTORY_H
#define CTHRESHOLDFACTORY_H

class CThresholdObject;

/** CThresholdFactory - A factory implementation for encapsulating multiple threshold algorithms . Choice of the algorithm comes from the UI **/

class CThresholdFactory
{
/** Constructors and Destructors **/
private:
    CThresholdFactory();
    CThresholdFactory(const CThresholdFactory& rhs);
    CThresholdFactory& operator =(const CThresholdFactory& rhs);

 public:
    ~CThresholdFactory();

/** Function Members (Public) **/
 public:
    static CThresholdFactory* CreateNew() {
        return new CThresholdFactory();
    }

    CThresholdObject *CreateThresholdObject(int type);

};

#endif // CTHRESHOLDFACTORY_H
