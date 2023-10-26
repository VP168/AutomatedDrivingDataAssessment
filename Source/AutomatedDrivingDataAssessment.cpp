#include "Headers/InputProcessing.h"
#include "Headers/Transformator.h"
#include "Headers/DataAssessment.h"
#include "Headers/OutputProcessing.h"


int main()
{
    std::vector<AssessmentDataOutput> outputData;

    // Read input data from JSON file
    InputData data;
    data.readJsonData();
    auto egoData = data.getEgoData();
    if (egoData.size() == 0) {
        outputData.push_back({ 0 , 0,  "empty Ego Data" });
    }

    auto objectObservations = data.getObservationsSortedBySceneId();
    if (objectObservations.size() == 0) {
        outputData.push_back({ 0 , 0,  "empty Object Observations" });
    }

    // Iterate through each Scene ID and analyse data for each scene
    // 1. Transform data to vehicle coordinate
    // 2. Check necessary conditions as per set parameters
    // 3. Send output data if any condition fails

    for (auto& eachEgoScene : egoData) {

        Transformator transformator;
        transformator.setEgoOrigin(eachEgoScene.x,eachEgoScene.y, eachEgoScene.yaw);

        auto observationsAsPerSceneId = data.getObservationsAsPerSceneID(objectObservations, eachEgoScene.sceneId);
        for (auto& eachObject : observationsAsPerSceneId) {

            transformator.setObjectLocationToTransform(eachObject.x, eachObject.y, eachObject.yaw);
            auto transformedObjectPosition = transformator.getTransforedObjectLocation();

            // Check necessary conditions
            DataAssessment assessData;
            DataAssessmentParameters parameters;

            auto headingAngleDiff = eachEgoScene.yaw - eachObject.yaw;
            auto distToEgo = assessData.getDistanceToEgo(transformedObjectPosition);
            auto objLocation = assessData.getObjectLocation(headingAngleDiff, transformedObjectPosition);

            if (parameters.checkCarNearbyCondition && eachObject.objType == ObjectType::VEHICLE_PASSENGER_CAR) {
                if (objLocation == ObjectLocation::FRONT && distToEgo < parameters.minimumAllowedFrontDistanceToObject)
                {
                    outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Vehicle too close in Front, " + std::to_string(distToEgo) + "meter from ego" });
                }

                if (objLocation == ObjectLocation::BEHIND && distToEgo < parameters.minimumAllowedRearDistanceToObject)
                {
                    outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Vehicle too close behind, " + std::to_string(distToEgo) + "meter from ego" });
                }

                if (distToEgo < parameters.minimumAllowedLateralDistanceToObject)
                {
                    if (objLocation == ObjectLocation::LEFT) {
                        outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Vehicle too close to left, " + std::to_string(distToEgo) + "meter from ego" });
                    }
                    if (objLocation == ObjectLocation::RIGHT) {
                        outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Vehicle too close to right, " + std::to_string(distToEgo) + "meter from ego" });
                    }
                }
            }

            if (parameters.checkPedestrianNearbyCondition && eachObject.objType == ObjectType::PEDESTRIAN && eachObject.speed > parameters.lowSpeedLimit) {
                outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Decelerate Ego Vehicle, pedestrian nearby" });
            }

            if (parameters.checkCyclistNearbyCondition && eachObject.objType == ObjectType::CYCLIST && distToEgo < parameters.minimumAllowedLateralDistanceToObject) {
                outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Keep Safe Distance, cyclist nearby" });
            }
            

            if (parameters.checkObjectIdentification && eachObject.objType == ObjectType::UNKNOWN) {
                outputData.push_back({ eachEgoScene.sceneId , eachObject.objectId,  "Object is not identified correctly" });
            }


        }

    }

    // Write output data to new JSON file in project directory
    if (outputData.size() == 0) {
        outputData.push_back({ 0 , 0,  "No critical observations found!" });
    }

    OutputProcessing assessmentData;
    auto jsonOutputData = assessmentData.setAssessmentData(outputData);
    assessmentData.getAssessmentDataJson();

}
