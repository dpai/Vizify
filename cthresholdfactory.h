#pragma once

class CThresholdObject;
enum class Algorithms;

/** CThresholdFactory - A factory implementation for encapsulating multiple threshold algorithms . Choice of the algorithm comes from the UI **/

class CThresholdFactory
{
/** Constructors and Destructors **/
public:
    CThresholdFactory(const CThresholdFactory& rhs) = delete;
    CThresholdFactory& operator =(const CThresholdFactory& rhs) = delete;

/** Function Members (Public) **/
 public:
    static CThresholdFactory* CreateNew() {
        return new CThresholdFactory{};
    }

    static CThresholdObject *CreateThresholdObject(Algorithms type);
};
