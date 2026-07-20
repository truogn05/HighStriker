#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "score_interface.h"

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
    ScoreDisplayData_t data;
    Score_GetDisplayData(&data);

    if (modelListener)
    {
        modelListener->onScoreUpdated(data);
    }

    if (data.isNewPeak)
    {
        Score_ClearNewPeakFlag();
    }
}
